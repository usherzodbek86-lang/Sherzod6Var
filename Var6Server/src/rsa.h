#pragma once
#include <QString>
#include <QByteArray>
#include <cstdint>

/**
 * @brief Реализация алгоритма RSA.
 *
 * Использует простые числа для генерации ключей.
 * Для учебных целей работает с числами фиксированного размера (64-bit).
 */
class RSA {
public:
    struct KeyPair {
        uint64_t n = 0;   ///< Модуль
        uint64_t e = 0;   ///< Публичная экспонента
        uint64_t d = 0;   ///< Приватная экспонента
    };

    /** @brief Сгенерировать пару ключей RSA. */
    static KeyPair generateKeys();

    /** @brief Зашифровать байты публичным ключом (n, e). */
    static QByteArray encrypt(const QByteArray &data, uint64_t n, uint64_t e);

    /** @brief Расшифровать байты приватным ключом (n, d). */
    static QByteArray decrypt(const QByteArray &data, uint64_t n, uint64_t d);

    /** @brief Сериализовать KeyPair в JSON-строку. */
    static QString keyPairToString(const KeyPair &kp);

    /** @brief Десериализовать KeyPair из JSON-строки. */
    static KeyPair keyPairFromString(const QString &s);

private:
    static uint64_t modPow(uint64_t base, uint64_t exp, uint64_t mod);
    static uint64_t gcd(uint64_t a, uint64_t b);
    static uint64_t modInverse(uint64_t e, uint64_t phi);
    static bool     isPrime(uint64_t n);
    static uint64_t randomPrime(uint64_t min, uint64_t max);
};
