#ifndef RBF_H
#define RBF_H

#include <Eigen/Dense>

class RBFInterpolator
{
public:
    RBFInterpolator(const Eigen::VectorXd &k, const Eigen::VectorXd &y, double epsilon);
    Eigen::VectorXd interpolate(const Eigen::VectorXd &x);

private:
    Eigen::VectorXd k_;
    Eigen::VectorXd y_;
    Eigen::VectorXd weights_;
    Eigen::MatrixXd A_;
    double epsilon_;
    double smoothing_;
};

#endif
