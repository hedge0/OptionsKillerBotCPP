#ifndef RBF_H
#define RBF_H

#include <Eigen/Dense>
#include <vector>

Eigen::MatrixXd compute_rbf_matrix(const Eigen::VectorXd &k, double epsilon);

Eigen::VectorXd interpolate_rbf(const Eigen::VectorXd &new_points,
                                const Eigen::VectorXd &k,
                                const Eigen::VectorXd &w,
                                double epsilon);

Eigen::VectorXd rbf_interpolation(const Eigen::VectorXd &k, const Eigen::VectorXd &y,
                                  const Eigen::VectorXd &new_points, double epsilon, double smoothing);

#endif
