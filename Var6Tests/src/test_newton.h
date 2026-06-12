#pragma once
#include <QtTest>
#include "newtonsolver.h"
#include <cmath>

/**
 * @brief Модульные тесты для класса NewtonSolver.
 */
class TestNewton : public QObject {
    Q_OBJECT

private slots:
    // TC-08: x³ − x − 2 = 0, корень ≈ 1.5214
    void test_cubic() {
        NewtonSolver s;
        s.setFunction  ([](double x){ return x*x*x - x - 2; });
        s.setDerivative([](double x){ return 3*x*x - 1; });
        bool ok;
        double root = s.solve(1.0, &ok);
        QVERIFY(ok);
        QVERIFY(std::abs(root - 1.5213797068) < 1e-6);
    }

    // TC-09: cos(x) − x = 0, корень ≈ 0.7391
    void test_cosine() {
        NewtonSolver s;
        s.setFunction  ([](double x){ return std::cos(x) - x; });
        s.setDerivative([](double x){ return -std::sin(x) - 1; });
        bool ok;
        double root = s.solve(1.0, &ok);
        QVERIFY(ok);
        QVERIFY(std::abs(root - 0.7390851332) < 1e-6);
    }

    // TC-10: нулевая производная → не сходится
    void test_zero_derivative() {
        NewtonSolver s;
        s.setFunction  ([](double x){ return x - 1; });
        s.setDerivative([](double  ){ return 0.0; }); // всегда 0
        bool ok;
        s.solve(0.5, &ok);
        QVERIFY(!ok);
    }

    // История итераций не пуста при успехе
    void test_iterations_not_empty() {
        NewtonSolver s;
        s.setFunction  ([](double x){ return x*x - 4; });
        s.setDerivative([](double x){ return 2*x; });
        bool ok;
        auto iters = s.solveDetailed(3.0, &ok);
        QVERIFY(ok);
        QVERIFY(!iters.isEmpty());
    }

    // Значение функции в найденном корне близко к нулю
    void test_residual_small() {
        NewtonSolver s;
        s.setFunction  ([](double x){ return x*x - 2; }); // √2
        s.setDerivative([](double x){ return 2*x; });
        bool ok;
        double root = s.solve(1.0, &ok);
        QVERIFY(ok);
        QVERIFY(std::abs(root*root - 2) < 1e-9);
    }

    // Без установки функции — не сходится
    void test_no_function_set() {
        NewtonSolver s;
        bool ok;
        s.solve(1.0, &ok);
        QVERIFY(!ok);
    }
};
