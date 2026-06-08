#pragma once
#include <QString>
#include <QByteArray>
#include <cstdint>

/**
 * @brief Реализация алгоритма SHA-1 без использования сторонних библиотек.
 *
 * Используется для хеширования паролей и верификации целостности данных.
 */
class SHA1 {
public:
    SHA1();

    /** @brief Вычислить хеш строки, вернуть hex-строку (40 символов). */
    static QString hash(const QString &input);

    /** @brief Проверить, совпадает ли строка с хешем. */
    static bool verify(const QString &input, const QString &hexHash);

private:
    static QByteArray compute(const QByteArray &data);

    static uint32_t rotateLeft(uint32_t val, int shift);
    static void     processBlock(const uint8_t *block, uint32_t state[5]);
};
