#ifndef MINIMIZE_H
#define MINIMIZE_H

#include <Eigen/Dense>
#include <functional>
#include <utility>

// Function to compute the objective function
double objective_function(const Eigen::VectorXd &params, const Eigen::VectorXd &k, const Eigen::VectorXd &y_mid);

// L-BFGS-B minimization function
std::pair<Eigen::VectorXd, double> minimize_lbfgsb(
    const std::function<double(const Eigen::VectorXd &)> &func,
    Eigen::VectorXd &x0,
    Eigen::VectorXd &lb,
    Eigen::VectorXd &ub,
    int max_iter = 15000,
    double pgtol = 1e-5,
    double factr = 1e7);

#endif // MINIMIZE_H
