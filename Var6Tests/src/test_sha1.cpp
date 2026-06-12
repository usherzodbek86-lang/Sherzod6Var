#pragma once
#include <QtTest>
#include "sha1.h"

/**
 * @brief Модульные тесты для класса SHA1.
 */
class TestSHA1 : public QObject {
    Q_OBJECT

private slots:
    // TC-01
    void test_hash_hello() {
        QString result = SHA1::hash("hello");
        QCOMPARE(result, QString("aaf4c61ddcc5e8a2dabede0f3b482cd9aea9434d"));
    }

    // TC-02
    void test_hash_empty() {
        QString result = SHA1::hash("");
        QCOMPARE(result, QString("da39a3ee5e6b4b0d3255bfef95601890afd80709"));
    }

    // TC-03: хеш всегда 40 символов
    void test_hash_length() {
        QCOMPARE(SHA1::hash("anything").length(), 40);
        QCOMPARE(SHA1::hash("").length(), 40);
        QCOMPARE(SHA1::hash(QString(1000, 'x')).length(), 40);
    }

    // TC-03: верификация верного хеша
    void test_verify_correct() {
        QString h = SHA1::hash("testpassword");
        QVERIFY(SHA1::verify("testpassword", h));
    }

    // TC-04: верификация неверного хеша
    void test_verify_wrong() {
        QVERIFY(!SHA1::verify("testpassword", "0000000000000000000000000000000000000000"));
    }

    // Разные строки дают разные хеши
    void test_different_inputs() {
        QVERIFY(SHA1::hash("abc") != SHA1::hash("ABC"));
        QVERIFY(SHA1::hash("hello") != SHA1::hash("hello "));
    }

    // Известные эталонные значения
    void test_known_values() {
        QCOMPARE(SHA1::hash("sha1"), QString("415ab40ae9b7cc4e66d6769cb2c08106e8293b48"));
        QCOMPARE(SHA1::hash("abc"),  QString("a9993e364706816aba3e25717850c26c9cd0d89d"));
    }
};
