#pragma once
#include <QString>
#include <QByteArray>

/**
 * @brief Стеганография в WAV-файлах методом LSB (Least Significant Bit).
 *
 * Сообщение кодируется в младших битах сэмплов аудиоданных.
 * Поддерживается только формат PCM WAV (16-bit, любое число каналов).
 */
class AudioSteganography {
public:
    explicit AudioSteganography(const QString &filePath = {});

    /** @brief Загрузить WAV-файл. Возвращает false при ошибке. */
    bool loadWav(const QString &filePath);

    /**
     * @brief Внедрить текстовое сообщение в аудиоданные.
     * @param message  текст для внедрения
     * @param outPath  путь для сохранения результирующего WAV
     * @return true при успехе
     */
    bool embedMessage(const QString &message, const QString &outPath);

    /**
     * @brief Извлечь внедрённое сообщение из загруженного WAV.
     * @param ok  флаг успеха
     * @return извлечённое сообщение
     */
    QString extractMessage(bool *ok = nullptr);

    /** @brief Максимальный размер сообщения (байты) для текущего файла. */
    int maxMessageBytes() const;

    QString lastError() const { return m_error; }

private:
    QString    m_filePath;
    QByteArray m_header;     // WAV-заголовок (44 байта)
    QByteArray m_audioData;  // Сырые PCM-данные
    QString    m_error;

    static const QByteArray MAGIC;   // маркер начала сообщения

    void setBit(int sampleIndex, int bit);
    int  getBit(int sampleIndex) const;
};
