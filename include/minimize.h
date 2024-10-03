#ifndef MINIMIZE_H
#define MINIMIZE_H

#include <Eigen/Dense>
#include <functional>
#include <string>
#include <vector>

struct MinimizeResult
{
    Eigen::VectorXd x;
    double fun;
    int nfev;
    int nit;
    int status;
    std::string message;
};

MinimizeResult minimize(
    const std::function<double(const Eigen::VectorXd &, Eigen::VectorXd &)> &func_grad,
    const Eigen::VectorXd &x0,
    const std::vector<std::pair<double, double>> &bounds,
    int maxiter = 15000,
    double ftol = 1e-8,
    double gtol = 1e-5);

#endif
