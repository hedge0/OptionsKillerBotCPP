#ifndef MODELS_H
#define MODELS_H

#include <string>

double calculate_implied_volatility_baw(double option_price, double S, double K, double r, double T, double q = 0.0, const std::string &option_type = "calls", int max_iterations = 100, double tolerance = 1e-8);

#endif