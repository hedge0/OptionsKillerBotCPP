#ifndef INTERPOLATIONS_H
#define INTERPOLATIONS_H

#include <Eigen/Dense>
#include <functional>

std::function<Eigen::VectorXd(const Eigen::VectorXd &)> rbf_model(const Eigen::VectorXd &k, const Eigen::VectorXd &y, double epsilon);

#endif
