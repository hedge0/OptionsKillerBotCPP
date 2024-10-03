#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include <limits>
#include <cmath>

// Helper functions for L-BFGS-B algorithm
double objective_function(const Eigen::VectorXd &params, const Eigen::VectorXd &k, const Eigen::VectorXd &y_mid)
{
    Eigen::VectorXd residuals = k.array() - y_mid.array();
    return residuals.squaredNorm(); // Minimize the sum of squares of residuals
}

std::pair<Eigen::VectorXd, double> minimize_lbfgsb(const std::function<double(const Eigen::VectorXd &)> &func,
                                                   Eigen::VectorXd &x0, Eigen::VectorXd &lb, Eigen::VectorXd &ub,
                                                   int max_iter = 15000, double pgtol = 1e-5, double factr = 1e7)
{
    Eigen::VectorXd grad(x0.size());
    double fx = func(x0); // Evaluate function at initial guess
    double f_prev = std::numeric_limits<double>::infinity();
    int iter = 0;
    const double epsilon = 1e-5;

    while (iter < max_iter && std::abs(fx - f_prev) > pgtol)
    {
        f_prev = fx;

        // Approximate gradient using finite differences
        for (int i = 0; i < x0.size(); ++i)
        {
            Eigen::VectorXd x_temp = x0;
            x_temp[i] += epsilon;
            double f_temp = func(x_temp);
            grad[i] = (f_temp - fx) / epsilon;
        }

        // Projected gradient step (clamping to bounds)
        for (int i = 0; i < x0.size(); i++)
        {
            x0[i] = std::min(std::max(x0[i] - grad[i], lb[i]), ub[i]);
        }

        fx = func(x0); // Evaluate new point
        iter++;
    }

    return {x0, fx};
}
