#include "rbf.h"
#include <Eigen/Dense>
#include <cmath>

/**
 * @brief Constructor for RBFInterpolator with multiquadric kernel and hardcoded smoothing.
 *
 * @param k Vector of input points for interpolation (log-moneyness).
 * @param y Corresponding values (implied volatilities).
 * @param epsilon Regularization parameter for the RBF kernel.
 */
RBFInterpolator::RBFInterpolator(const Eigen::VectorXd &k, const Eigen::VectorXd &y, double epsilon)
    : k_(k), y_(y), epsilon_(epsilon), smoothing_(1e-12)
{
    Eigen::Index n = k.size(); // Use Eigen::Index to avoid warnings
    A_ = Eigen::MatrixXd(n, n);

    for (Eigen::Index i = 0; i < n; ++i)
    {
        for (Eigen::Index j = 0; j < n; ++j)
        {
            double r = (k(i) - k(j));
            A_(i, j) = std::sqrt(1 + (epsilon_ * epsilon_ * r * r));
        }
        A_(i, i) += smoothing_;
    }

    weights_ = A_.ldlt().solve(y);
}

/**
 * @brief Function to interpolate the values for all inputs.
 *
 * @param x Vector of points where interpolation is evaluated.
 * @return Eigen::VectorXd Vector of interpolated values.
 */
Eigen::VectorXd RBFInterpolator::interpolate(const Eigen::VectorXd &x)
{
    Eigen::Index n = k_.size(); // Use Eigen::Index to avoid warnings
    Eigen::Index m = x.size();  // Use Eigen::Index to avoid warnings
    Eigen::VectorXd result = Eigen::VectorXd::Zero(m);

    for (Eigen::Index i = 0; i < m; ++i)
    {
        for (Eigen::Index j = 0; j < n; ++j)
        {
            double r = (x(i) - k_(j));
            result(i) += weights_(j) * std::sqrt(1 + (epsilon_ * epsilon_ * r * r));
        }
    }

    return result;
}
