#include <Eigen/Dense>
#include "rbf.h"
#include "interpolations.h"
#include <functional>
#include <algorithm>

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

/**
 * @brief RFV Model function for implied volatility fitting.
 *
 * @param k Log-moneyness of the option.
 * @param params Parameters [a, b, c, d, e] for the RFV model.
 * @return double The calculated RFV model value for the given log-moneyness.
 */
double rfv_model(double k, const Eigen::VectorXd &params)
{
    double a = params[0];
    double b = params[1];
    double c = params[2];
    double d = params[3];
    double e = params[4];

    return (a + b * k + c * k * k) / (1 + d * k + e * k * k);
}

/**
 * @brief Objective function to minimize during model fitting (WLS method).
 *
 * @param params Vector of model parameters.
 * @param k Vector of log-moneyness.
 * @param y_mid Vector of mid prices of the options.
 * @param y_bid Vector of bid prices of the options.
 * @param y_ask Vector of ask prices of the options.
 * @return double The calculated objective value (WLS).
 */
double objective_function(const Eigen::VectorXd &params, const Eigen::VectorXd &k, const Eigen::VectorXd &y_mid, const Eigen::VectorXd &y_bid, const Eigen::VectorXd &y_ask)
{
    Eigen::VectorXd spread = y_ask - y_bid;
    double epsilon = 1e-8;
    Eigen::VectorXd weights = 1.0 / (spread.array() + epsilon);

    Eigen::VectorXd residuals(k.size());
    for (int i = 0; i < k.size(); ++i)
    {
        residuals[i] = rfv_model(k[i], params) - y_mid[i];
    }

    Eigen::VectorXd weighted_residuals = weights.array() * residuals.array().square();

    return weighted_residuals.sum();
}
