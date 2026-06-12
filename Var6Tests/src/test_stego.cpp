#pragma once
#include <QtTest>
#include <QTemporaryFile>
#include <QDir>
#include "audiosteganography.h"

/**
 * @brief Модульные тесты для класса AudioSteganography.
 *
 * Создаём синтетический WAV-файл прямо в тесте — не нужен реальный файл.
 */
class TestStego : public QObject {
    Q_OBJECT

private:
    // Генерирует минимальный корректный WAV PCM 16-bit mono
    QString makeTempWav(int numSamples = 44100) {
        QTemporaryFile tmp;
        tmp.setAutoRemove(false);
        tmp.setFileTemplate(QDir::tempPath() + "/test_XXXXXX.wav");
        if (!tmp.open()) return {};

        // WAV заголовок (44 байта)
        uint32_t dataSize   = numSamples * 2;      // 16-bit = 2 байта на сэмпл
        uint32_t chunkSize  = 36 + dataSize;
        uint32_t sampleRate = 44100;
        uint16_t numChannels= 1;
        uint16_t bitsPerSample = 16;
        uint32_t byteRate   = sampleRate * numChannels * bitsPerSample / 8;
        uint16_t blockAlign = numChannels * bitsPerSample / 8;

        auto w2 = [&](uint16_t v){ tmp.write(reinterpret_cast<char*>(&v), 2); };
        auto w4 = [&](uint32_t v){ tmp.write(reinterpret_cast<char*>(&v), 4); };

        tmp.write("RIFF", 4);  w4(chunkSize);
        tmp.write("WAVE", 4);
        tmp.write("fmt ", 4);  w4(16);
        w2(1); w2(numChannels); w4(sampleRate); w4(byteRate); w2(blockAlign); w2(bitsPerSample);
        tmp.write("data", 4);  w4(dataSize);

        // Заполняем сэмплы нулями (тишина)
        QByteArray silence(dataSize, '\0');
        tmp.write(silence);
        tmp.close();
        return tmp.fileName();
    }

private slots:
    // TC-11: внедрение короткого сообщения
    void test_embed_success() {
        QString wav = makeTempWav();
        QVERIFY(!wav.isEmpty());

        AudioSteganography steg;
        QVERIFY(steg.loadWav(wav));

        QString outPath = wav + "_out.wav";
        bool ok = steg.embedMessage("Hello", outPath);
        QVERIFY2(ok, steg.lastError().toUtf8());
        QVERIFY(QFile::exists(outPath));

        QFile::remove(wav);
        QFile::remove(outPath);
    }

    // TC-12: извлечение после внедрения
    void test_embed_then_extract() {
        QString wav    = makeTempWav();
        QString outWav = wav + "_steg.wav";

        AudioSteganography steg;
        QVERIFY(steg.loadWav(wav));
        QVERIFY(steg.embedMessage("SecretMessage123", outWav));

        AudioSteganography steg2;
        QVERIFY(steg2.loadWav(outWav));
        bool ok;
        QString msg = steg2.extractMessage(&ok);
        QVERIFY2(ok, steg2.lastError().toUtf8());
        QCOMPARE(msg, QString("SecretMessage123"));

        QFile::remove(wav);
        QFile::remove(outWav);
    }

    // TC-12: кириллица тоже работает
    void test_embed_utf8() {
        QString wav    = makeTempWav();
        QString outWav = wav + "_utf.wav";

        AudioSteganography steg;
        QVERIFY(steg.loadWav(wav));
        QVERIFY(steg.embedMessage("Привет мир", outWav));

        AudioSteganography steg2;
        QVERIFY(steg2.loadWav(outWav));
        bool ok;
        QCOMPARE(steg2.extractMessage(&ok), QString("Привет мир"));
        QVERIFY(ok);

        QFile::remove(wav);
        QFile::remove(outWav);
    }

    // TC-13: сообщение превышает ёмкость
    void test_message_too_long() {
        QString wav = makeTempWav(100); // очень маленький файл — 100 сэмплов
        AudioSteganography steg;
        QVERIFY(steg.loadWav(wav));

        // Максимум ~4 байта при 100 сэмплах — шлём 50 символов
        bool ok = steg.embedMessage(QString(50, 'X'), wav + "_out.wav");
        QVERIFY(!ok);
        QVERIFY(!steg.lastError().isEmpty());

        QFile::remove(wav);
    }

    // Загрузка несуществующего файла
    void test_load_nonexistent() {
        AudioSteganography steg;
        QVERIFY(!steg.loadWav("/tmp/nonexistent_12345.wav"));
        QVERIFY(!steg.lastError().isEmpty());
    }

    // maxMessageBytes > 0 для нормального файла
    void test_max_message_bytes() {
        QString wav = makeTempWav(44100);
        AudioSteganography steg;
        QVERIFY(steg.loadWav(wav));
        QVERIFY(steg.maxMessageBytes() > 0);
        QFile::remove(wav);
    }
};
