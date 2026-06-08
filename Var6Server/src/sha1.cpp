#include "sha1.h"
#include <QByteArray>
#include <cstring>

SHA1::SHA1() {}

uint32_t SHA1::rotateLeft(uint32_t val, int shift) {
    return (val << shift) | (val >> (32 - shift));
}

void SHA1::processBlock(const uint8_t *block, uint32_t state[5]) {
    uint32_t w[80];

    // Загружаем первые 16 слов из блока (big-endian)
    for (int i = 0; i < 16; ++i) {
        w[i] = (uint32_t(block[i*4])   << 24) |
               (uint32_t(block[i*4+1]) << 16) |
               (uint32_t(block[i*4+2]) <<  8) |
               (uint32_t(block[i*4+3]));
    }
    // Расширяем до 80 слов
    for (int i = 16; i < 80; ++i)
        w[i] = rotateLeft(w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16], 1);

    uint32_t a = state[0], b = state[1], c = state[2],
             d = state[3], e = state[4];

    for (int i = 0; i < 80; ++i) {
        uint32_t f, k;
        if (i < 20) {
            f = (b & c) | (~b & d);
            k = 0x5A827999;
        } else if (i < 40) {
            f = b ^ c ^ d;
            k = 0x6ED9EBA1;
        } else if (i < 60) {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8F1BBCDC;
        } else {
            f = b ^ c ^ d;
            k = 0xCA62C1D6;
        }
        uint32_t tmp = rotateLeft(a, 5) + f + e + k + w[i];
        e = d; d = c;
        c = rotateLeft(b, 30);
        b = a; a = tmp;
    }

    state[0] += a; state[1] += b; state[2] += c;
    state[3] += d; state[4] += e;
}

QByteArray SHA1::compute(const QByteArray &data) {
    uint32_t state[5] = {
        0x67452301, 0xEFCDAB89, 0x98BADCFE,
        0x10325476, 0xC3D2E1F0
    };

    // Padding
    QByteArray msg = data;
    uint64_t bitLen = uint64_t(data.size()) * 8;
    msg.append(char(0x80));
    while ((msg.size() % 64) != 56)
        msg.append(char(0x00));

    // Длина в big-endian
    for (int i = 7; i >= 0; --i)
        msg.append(char((bitLen >> (i * 8)) & 0xFF));

    // Обрабатываем блоки по 64 байта
    for (int off = 0; off < msg.size(); off += 64)
        processBlock(reinterpret_cast<const uint8_t*>(msg.constData() + off), state);

    // Собираем результат
    QByteArray result;
    for (int i = 0; i < 5; ++i) {
        result.append(char((state[i] >> 24) & 0xFF));
        result.append(char((state[i] >> 16) & 0xFF));
        result.append(char((state[i] >>  8) & 0xFF));
        result.append(char( state[i]        & 0xFF));
    }
    return result;
}

QString SHA1::hash(const QString &input) {
    QByteArray raw = compute(input.toUtf8());
    return QString::fromLatin1(raw.toHex());
}

bool SHA1::verify(const QString &input, const QString &hexHash) {
    return hash(input) == hexHash;
}
