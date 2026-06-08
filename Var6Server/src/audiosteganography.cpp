#include "audiosteganography.h"
#include <QFile>
#include <QDataStream>

// Маркер конца сообщения: 8 нулевых байт
const QByteArray AudioSteganography::MAGIC = QByteArray(8, '\0');

AudioSteganography::AudioSteganography(const QString &filePath) {
    if (!filePath.isEmpty())
        loadWav(filePath);
}

bool AudioSteganography::loadWav(const QString &filePath) {
    m_filePath = filePath;
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly)) {
        m_error = "Не удалось открыть файл: " + filePath;
        return false;
    }
    QByteArray all = f.readAll();
    f.close();

    if (all.size() < 44) {
        m_error = "Файл слишком мал для WAV";
        return false;
    }
    // Проверяем сигнатуру RIFF
    if (!all.startsWith("RIFF") || all.mid(8, 4) != "WAVE") {
        m_error = "Файл не является WAV (RIFF/WAVE)";
        return false;
    }

    m_header    = all.left(44);
    m_audioData = all.mid(44);
    return true;
}

int AudioSteganography::maxMessageBytes() const {
    // Каждый байт сообщения требует 8 сэмплов (по 1 биту на сэмпл)
    return m_audioData.size() / 8 - 8; // минус место под маркер
}

void AudioSteganography::setBit(int sampleIndex, int bit) {
    // Работаем с 16-bit сэмплами (little-endian)
    int byteIdx = sampleIndex * 2; // 2 байта на сэмпл
    if (byteIdx >= m_audioData.size()) return;
    unsigned char b = (unsigned char)m_audioData[byteIdx];
    b = (b & 0xFE) | (bit & 1);   // заменяем LSB
    m_audioData[byteIdx] = (char)b;
}

int AudioSteganography::getBit(int sampleIndex) const {
    int byteIdx = sampleIndex * 2;
    if (byteIdx >= m_audioData.size()) return 0;
    return (unsigned char)m_audioData[byteIdx] & 1;
}

bool AudioSteganography::embedMessage(const QString &message, const QString &outPath) {
    if (m_audioData.isEmpty()) {
        m_error = "WAV-файл не загружен";
        return false;
    }

    QByteArray payload = message.toUtf8() + MAGIC; // текст + маркер конца

    if (payload.size() > maxMessageBytes()) {
        m_error = QString("Сообщение слишком длинное. Максимум %1 байт").arg(maxMessageBytes());
        return false;
    }

    // Записываем биты сообщения в LSB сэмплов
    int sampleIdx = 0;
    for (unsigned char byte : payload) {
        for (int bit = 7; bit >= 0; --bit) {
            setBit(sampleIdx++, (byte >> bit) & 1);
        }
    }

    // Сохраняем результат
    QFile out(outPath);
    if (!out.open(QIODevice::WriteOnly)) {
        m_error = "Не удалось создать выходной файл: " + outPath;
        return false;
    }
    out.write(m_header);
    out.write(m_audioData);
    out.close();
    return true;
}

QString AudioSteganography::extractMessage(bool *ok) {
    if (m_audioData.isEmpty()) {
        m_error = "WAV-файл не загружен";
        if (ok) *ok = false;
        return {};
    }

    QByteArray result;
    int sampleIdx = 0;
    int totalSamples = m_audioData.size() / 2;

    while (sampleIdx + 8 <= totalSamples) {
        // Читаем 8 бит = 1 байт
        unsigned char byte = 0;
        for (int bit = 7; bit >= 0; --bit) {
            byte |= (getBit(sampleIdx++) << bit);
        }
        result.append((char)byte);

        // Проверяем маркер конца
        if (result.endsWith(MAGIC)) {
            result.chop(MAGIC.size());
            if (ok) *ok = true;
            return QString::fromUtf8(result);
        }
    }

    m_error = "Маркер конца сообщения не найден";
    if (ok) *ok = false;
    return {};
}
