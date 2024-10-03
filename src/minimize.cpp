#include "minimize.h"
#include <iostream>
#include <Eigen/Dense>
#include <functional>
#include <cmath>
#include <vector>
#include <limits>

/**
 * @brief Perform bound-constrained minimization using the L-BFGS-B algorithm.
 *
 * @param func_grad Function that computes the objective function and its gradient.
 *                  It takes a vector `x` and outputs the function value and gradient at `x`.
 * @param x0 Initial guess for the variables.
 * @param bounds Vector of pairs specifying the lower and upper bounds for each variable.
 * @param maxiter Maximum number of iterations allowed.
 * @param ftol Relative tolerance for the function value convergence criterion.
 * @param gtol Tolerance for the gradient norm convergence criterion.
 * @return MinimizeResult Structure containing the optimization results:
 *         - x: The solution vector.
 *         - fun: Objective function value at the solution.
 *         - nfev: Number of function evaluations.
 *         - nit: Number of iterations performed.
 *         - status: Exit status (0 for success).
 *         - message: Exit message describing the cause of termination.
 */
MinimizeResult minimize(
    const std::function<double(const Eigen::VectorXd &, Eigen::VectorXd &)> &func_grad,
    const Eigen::VectorXd &x0,
    const std::vector<std::pair<double, double>> &bounds,
    int maxiter,
    double ftol,
    double gtol)
{
    Eigen::Index n = x0.size();
    Eigen::VectorXd x = x0;
    Eigen::VectorXd grad(n);
    double f = func_grad(x, grad);

    int m = 10;
    int iter = 0;
    int nfev = 1;
    int status = 0;
    std::string message = "Optimization terminated successfully.";
    double prev_f = f;

    std::vector<Eigen::VectorXd> s_list;
    std::vector<Eigen::VectorXd> y_list;
    std::vector<double> rho_list;

    while (iter < maxiter)
    {
        Eigen::VectorXd q = grad;
        Eigen::Index k = s_list.size();

        std::vector<double> alpha(k);

        for (Eigen::Index i = k - 1; i >= 0; --i)
        {
            alpha[i] = rho_list[i] * s_list[i].dot(q);
            q = q - alpha[i] * y_list[i];
        }

        Eigen::VectorXd r = q;

        for (Eigen::Index i = 0; i < k; ++i)
        {
            double beta = rho_list[i] * y_list[i].dot(r);
            r = r + s_list[i] * (alpha[i] - beta);
        }

        Eigen::VectorXd p = -r;

        for (Eigen::Index i = 0; i < n; ++i)
        {
            if (bounds[i].first == bounds[i].second)
            {
                p[i] = 0.0;
            }
            else
            {
                if (x[i] <= bounds[i].first && p[i] < 0)
                    p[i] = 0.0;
                if (x[i] >= bounds[i].second && p[i] > 0)
                    p[i] = 0.0;
            }
        }

        double alpha_step = 1.0;
        double c1 = 1e-4;
        double c2 = 0.9;
        int max_linesearch = 20;
        bool success = false;
        Eigen::VectorXd x_new(n);
        double f_new;
        Eigen::VectorXd grad_new(n);
        for (int ls_iter = 0; ls_iter < max_linesearch; ++ls_iter)
        {
            x_new = x + alpha_step * p;

            for (Eigen::Index i = 0; i < n; ++i)
            {
                if (bounds[i].first > -std::numeric_limits<double>::infinity())
                    x_new[i] = std::max(x_new[i], bounds[i].first);
                if (bounds[i].second < std::numeric_limits<double>::infinity())
                    x_new[i] = std::min(x_new[i], bounds[i].second);
            }

            f_new = func_grad(x_new, grad_new);
            nfev++;

            if (f_new <= f + c1 * alpha_step * grad.dot(p))
            {
                if (grad_new.dot(p) >= c2 * grad.dot(p))
                {
                    success = true;
                    break;
                }
            }

            alpha_step *= 0.5;
        }

        if (!success)
        {
            status = 1;
            message = "Line search failed.";
            break;
        }

        Eigen::VectorXd s = x_new - x;
        Eigen::VectorXd y = grad_new - grad;
        double ys = y.dot(s);
        if (ys > 1e-10)
        {
            if (s_list.size() == static_cast<std::size_t>(m))
            {
                s_list.erase(s_list.begin());
                y_list.erase(y_list.begin());
                rho_list.erase(rho_list.begin());
            }
            s_list.push_back(s);
            y_list.push_back(y);
            rho_list.push_back(1.0 / ys);
        }

        x = x_new;
        f = f_new;
        grad = grad_new;

        if (grad.cwiseAbs().maxCoeff() < gtol)
        {
            status = 0;
            message = "Optimization terminated successfully (gtol).";
            break;
        }

        if (std::abs(f - prev_f) < ftol * (1.0 + std::abs(f)))
        {
            status = 0;
            message = "Optimization terminated successfully (ftol).";
            break;
        }

        prev_f = f;
        iter++;
    }

    if (iter >= maxiter)
    {
        status = 1;
        message = "Maximum number of iterations exceeded.";
    }

    MinimizeResult result;
    result.x = x;
    result.fun = f;
    result.nfev = nfev;
    result.nit = iter;
    result.status = status;
    result.message = message;

    return result;
}
