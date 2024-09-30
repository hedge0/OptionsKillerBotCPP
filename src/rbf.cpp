#include "rbf.h"
#include <Eigen/Dense>
#include <cmath>
#include <vector>

/**
 * @brief Multiquadric Radial Basis Function (RBF) kernel.
 *
 * @param r The Euclidean distance between input points.
 * @param epsilon Shape parameter controlling the width of the RBF.
 * @return double The calculated value of the multiquadric kernel.
 */
double multiquadric_rbf(double r, double epsilon)
{
    return std::sqrt((r / epsilon) * (r / epsilon) + 1);
}

/**
 * @brief Compute the pairwise distance matrix using the multiquadric kernel.
 *
 * @param k The vector of input points.
 * @param epsilon Shape parameter for the multiquadric kernel.
 * @return Eigen::MatrixXd The computed pairwise distance matrix.
 */
Eigen::MatrixXd compute_rbf_matrix(const Eigen::VectorXd &k, double epsilon)
{
    Eigen::Index n = k.size();
    Eigen::MatrixXd A(n, n);
    for (Eigen::Index i = 0; i < n; ++i)
    {
        for (Eigen::Index j = 0; j < n; ++j)
        {
            double r = std::abs(k[i] - k[j]);
            A(i, j) = multiquadric_rbf(r, epsilon);
        }
    }
    return A;
}

/**
 * @brief Interpolate new points using RBF interpolation with multiquadric kernel.
 *
 * @param new_points The vector of points at which interpolation is performed.
 * @param k The original input points.
 * @param w The weights solved from the RBF system.
 * @param epsilon Shape parameter for the multiquadric kernel.
 * @return Eigen::VectorXd The interpolated values at the new points.
 */
Eigen::VectorXd interpolate_rbf(const Eigen::VectorXd &new_points,
                                const Eigen::VectorXd &k,
                                const Eigen::VectorXd &w,
                                double epsilon)
{
    Eigen::Index m = new_points.size();
    Eigen::Index n = k.size();
    Eigen::VectorXd interpolated_values(m);

    for (Eigen::Index i = 0; i < m; ++i)
    {
        double value = 0.0;
        for (Eigen::Index j = 0; j < n; ++j)
        {
            double r = std::abs(new_points[i] - k[j]);
            value += w[j] * multiquadric_rbf(r, epsilon);
        }
        interpolated_values[i] = value;
    }

    return interpolated_values;
}

/**
 * @brief Perform Radial Basis Function (RBF) interpolation using the multiquadric kernel.
 *
 * @param k The original input points.
 * @param y The observed values at the input points.
 * @param new_points The vector of points at which interpolation is performed.
 * @param epsilon Shape parameter for the multiquadric kernel.
 * @param smoothing Smoothing parameter to avoid overfitting.
 * @return Eigen::VectorXd The interpolated values at the new points.
 */
Eigen::VectorXd rbf_interpolation(const Eigen::VectorXd &k, const Eigen::VectorXd &y,
                                  const Eigen::VectorXd &new_points, double epsilon, double smoothing)
{
    Eigen::MatrixXd A = compute_rbf_matrix(k, epsilon);
    A += smoothing * Eigen::MatrixXd::Identity(k.size(), k.size());
    Eigen::VectorXd w = A.colPivHouseholderQr().solve(y);
    Eigen::VectorXd interpolated_values = interpolate_rbf(new_points, k, w, epsilon);

    return interpolated_values;
}
