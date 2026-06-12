#include <QtTest>

// Объявляем все тест-классы
class TestSHA1;
class TestRSA;
class TestNewton;
class TestStego;

#include "test_sha1.h"
#include "test_rsa.h"
#include "test_newton.h"
#include "test_stego.h"

int main(int argc, char *argv[]) {
    int status = 0;

    auto run = [&](QObject *obj) {
        status |= QTest::qExec(obj, argc, argv);
        delete obj;
    };

    run(new TestSHA1);
    run(new TestRSA);
    run(new TestNewton);
    run(new TestStego);

    return status;
}
