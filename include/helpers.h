#ifndef HELPERS_H
#define HELPERS_H

#include <string>
#include "Eigen/Dense"

void write_csv(const std::string &filename, const Eigen::VectorXd &x_vals, const Eigen::VectorXd &y_vals);
bool is_nyse_open();

#endif
