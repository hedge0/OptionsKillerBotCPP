#ifndef INTERPOLATIONS_H
#define INTERPOLATIONS_H

#include <Eigen/Dense>
#include <functional>

std::function<Eigen::VectorXd(const Eigen::VectorXd &)> rbf_model(
    const Eigen::VectorXd &k,
    const Eigen::VectorXd &y,
    double epsilon);

Eigen::VectorXd rfv_model(
    const Eigen::VectorXd &k,
    const Eigen::VectorXd &params);

Eigen::VectorXd fit_model(
    const Eigen::VectorXd &x,
    const Eigen::VectorXd &y_mid,
    const Eigen::VectorXd &y_bid,
    const Eigen::VectorXd &y_ask);

#endif
