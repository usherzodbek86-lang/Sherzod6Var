#include "newtonsolver.h"
#include <cmath>

NewtonSolver::NewtonSolver(double epsilon, int maxIter)
    : m_epsilon(epsilon), m_maxIter(maxIter)
{}

void NewtonSolver::setFunction(Func f)   { m_f  = f; }
void NewtonSolver::setDerivative(Func df){ m_df = df; }

double NewtonSolver::solve(double x0, bool *ok) const {
    auto iters = solveDetailed(x0, ok);
    return iters.isEmpty() ? x0 : iters.last().x;
}

QList<NewtonSolver::Iteration> NewtonSolver::solveDetailed(double x0, bool *ok) const {
    QList<Iteration> history;

    if (!m_f || !m_df) {
        if (ok) *ok = false;
        return history;
    }

    double x = x0;
    for (int i = 0; i < m_maxIter; ++i) {
        double fx  = m_f(x);
        double dfx = m_df(x);
        history.append({x, fx});

        if (std::abs(fx) < m_epsilon) {
            if (ok) *ok = true;
            return history;
        }

        // Защита от деления на ноль (производная слишком мала)
        if (std::abs(dfx) < 1e-15) {
            if (ok) *ok = false;
            return history;
        }

        x = x - fx / dfx;
    }

    if (ok) *ok = false;
    return history;
}
