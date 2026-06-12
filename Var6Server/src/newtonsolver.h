#pragma once
#include <QString>
#include <QList>
#include <functional>

/**
 * @brief Численное решение уравнений методом Ньютона (касательных).
 *
 * Итерационная формула: x_{n+1} = x_n - f(x_n) / f'(x_n)
 * Останов: |f(x_n)| < epsilon или число итераций > maxIter.
 */
class NewtonSolver {
public:
    using Func = std::function<double(double)>;

    explicit NewtonSolver(double epsilon = 1e-9, int maxIter = 1000);

    /** @brief Задать функцию f(x). */
    void setFunction(Func f);

    /** @brief Задать производную f'(x). */
    void setDerivative(Func df);

    /**
     * @brief Найти корень уравнения f(x) = 0.
     * @param x0     начальное приближение
     * @param ok     флаг успеха (false если не сошлось)
     * @return       найденный корень
     */
    double solve(double x0, bool *ok = nullptr) const;

    /** @brief Вернуть историю итераций (x_n, f(x_n)). */
    struct Iteration { double x, fx; };
    QList<Iteration> solveDetailed(double x0, bool *ok = nullptr) const;

    int    maxIter() const { return m_maxIter; }
    double epsilon() const { return m_epsilon; }

private:
    Func   m_f;
    Func   m_df;
    double m_epsilon;
    int    m_maxIter;
};
