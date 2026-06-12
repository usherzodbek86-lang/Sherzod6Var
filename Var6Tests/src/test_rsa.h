#pragma once
#include <QtTest>
#include "rsa.h"

/**
 * @brief Модульные тесты для класса RSA.
 */
class TestRSA : public QObject {
    Q_OBJECT

private slots:
    // TC-05: генерация ключей
    void test_key_generation() {
        RSA::KeyPair kp = RSA::generateKeys();
        QVERIFY(kp.n > 0);
        QVERIFY(kp.e > 0);
        QVERIFY(kp.d > 0);
        QVERIFY(kp.n != kp.e);
        QVERIFY(kp.e != kp.d);
    }

    // TC-05: каждый раз генерируются разные ключи
    void test_keys_are_random() {
        RSA::KeyPair kp1 = RSA::generateKeys();
        RSA::KeyPair kp2 = RSA::generateKeys();
        // С высокой вероятностью n будет разным
        // (небольшой шанс совпадения — допустимо для учебного RSA)
        QVERIFY(kp1.n != kp2.n || kp1.d != kp2.d);
    }

    // TC-06: шифрование и дешифрование
    void test_encrypt_decrypt() {
        RSA::KeyPair kp = RSA::generateKeys();
        QByteArray original = "Hello";
        QByteArray enc = RSA::encrypt(original, kp.n, kp.e);
        QByteArray dec = RSA::decrypt(enc, kp.n, kp.d);
        QCOMPARE(dec, original);
    }

    // TC-06: разные строки
    void test_encrypt_decrypt_various() {
        RSA::KeyPair kp = RSA::generateKeys();
        for (const char* text : {"A", "test", "12345", "RSA works!"}) {
            QByteArray orig(text);
            QByteArray dec = RSA::decrypt(RSA::encrypt(orig, kp.n, kp.e), kp.n, kp.d);
            QCOMPARE(dec, orig);
        }
    }

    // TC-06: шифрованные данные отличаются от исходных
    void test_encrypted_differs() {
        RSA::KeyPair kp = RSA::generateKeys();
        QByteArray orig = "Hello";
        QByteArray enc  = RSA::encrypt(orig, kp.n, kp.e);
        QVERIFY(enc != orig);
    }

    // Сериализация ключей
    void test_key_serialization() {
        RSA::KeyPair kp  = RSA::generateKeys();
        QString      s   = RSA::keyPairToString(kp);
        RSA::KeyPair kp2 = RSA::keyPairFromString(s);
        QCOMPARE(kp.n, kp2.n);
        QCOMPARE(kp.e, kp2.e);
        QCOMPARE(kp.d, kp2.d);
    }
};
