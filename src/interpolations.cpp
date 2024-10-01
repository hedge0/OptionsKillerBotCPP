#include <Eigen/Dense>
#include "rbf.h"
#include "interpolations.h"

/**
 * @brief Function for RBF Interpolation model.
 *
 * @param k Vector of input points for interpolation (log-moneyness).
 * @param y Corresponding values (implied volatilities).
 * @param epsilon Regularization parameter for the RBF kernel. Defaults to computed value if not provided.
 * @return std::function<Eigen::VectorXd(const Eigen::VectorXd &)> Lambda function that evaluates interpolated values.
 */
std::function<Eigen::VectorXd(const Eigen::VectorXd &)> rbf_model(const Eigen::VectorXd &k, const Eigen::VectorXd &y, double epsilon)
{
    if (epsilon <= 0)
    {
        Eigen::VectorXd k_sorted = k;
        std::sort(k_sorted.data(), k_sorted.data() + k_sorted.size());
        epsilon = (k_sorted.tail(1)[0] - k_sorted.head(1)[0]) / (k_sorted.size() - 1);
    }

    RBFInterpolator rbf(k, y, epsilon);

    return [rbf](const Eigen::VectorXd &inputs) mutable -> Eigen::VectorXd
    {
        return rbf.interpolate(inputs);
    };
}
