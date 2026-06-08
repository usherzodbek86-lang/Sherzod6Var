#include "rsa.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QRandomGenerator>
#include <cstring>

// ---------- арифметика ----------

uint64_t RSA::modPow(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1)
            result = (__uint128_t)result * base % mod;
        base = (__uint128_t)base * base % mod;
        exp >>= 1;
    }
    return result;
}

uint64_t RSA::gcd(uint64_t a, uint64_t b) {
    while (b) { a %= b; std::swap(a, b); }
    return a;
}

uint64_t RSA::modInverse(uint64_t e, uint64_t phi) {
    // Расширенный алгоритм Евклида
    int64_t old_r = phi, r = e;
    int64_t old_s = 0,  s = 1;
    while (r != 0) {
        int64_t q = old_r / r;
        int64_t tmp = r; r = old_r - q * r; old_r = tmp;
        tmp = s; s = old_s - q * s; old_s = tmp;
    }
    return old_s < 0 ? (uint64_t)(old_s + (int64_t)phi) : (uint64_t)old_s;
}

bool RSA::isPrime(uint64_t n) {
    if (n < 2)  return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (uint64_t i = 3; i * i <= n; i += 2)
        if (n % i == 0) return false;
    return true;
}

uint64_t RSA::randomPrime(uint64_t minV, uint64_t maxV) {
    uint64_t val;
    do {
        val = QRandomGenerator::global()->bounded((uint32_t)(maxV - minV)) + minV;
        if (val % 2 == 0) val++;
    } while (!isPrime(val));
    return val;
}

// ---------- публичный интерфейс ----------

RSA::KeyPair RSA::generateKeys() {
    // Используем небольшие простые для учебного варианта (16-bit диапазон)
    uint64_t p = randomPrime(200, 500);
    uint64_t q;
    do { q = randomPrime(200, 500); } while (q == p);

    uint64_t n   = p * q;
    uint64_t phi = (p - 1) * (q - 1);
    uint64_t e   = 65537;
    if (gcd(e, phi) != 1) e = 3;

    uint64_t d = modInverse(e, phi);

    return {n, e, d};
}

QByteArray RSA::encrypt(const QByteArray &data, uint64_t n, uint64_t e) {
    QByteArray result;
    result.reserve(data.size() * 8);
    for (unsigned char byte : data) {
        uint64_t enc = modPow(byte, e, n);
        // Сохраняем как 8 байт big-endian
        for (int i = 7; i >= 0; --i)
            result.append(char((enc >> (i * 8)) & 0xFF));
    }
    return result;
}

QByteArray RSA::decrypt(const QByteArray &data, uint64_t n, uint64_t d) {
    QByteArray result;
    // Каждые 8 байт = один зашифрованный байт
    for (int i = 0; i + 7 < data.size(); i += 8) {
        uint64_t enc = 0;
        for (int j = 0; j < 8; ++j)
            enc = (enc << 8) | (unsigned char)data[i + j];
        uint64_t dec = modPow(enc, d, n);
        result.append(char(dec & 0xFF));
    }
    return result;
}

QString RSA::keyPairToString(const KeyPair &kp) {
    QJsonObject o;
    o["n"] = QString::number(kp.n);
    o["e"] = QString::number(kp.e);
    o["d"] = QString::number(kp.d);
    return QJsonDocument(o).toJson(QJsonDocument::Compact);
}

RSA::KeyPair RSA::keyPairFromString(const QString &s) {
    QJsonObject o = QJsonDocument::fromJson(s.toUtf8()).object();
    return {
        o["n"].toString().toULongLong(),
        o["e"].toString().toULongLong(),
        o["d"].toString().toULongLong()
    };
}
