#include <iostream>
#include <string>
#include "models.h"

int main()
{
    double option_price = 10.0;
    double S = 100.0;
    double K = 100.0;
    double r = 0.05;
    double T = 1.0;
    double q = 0.0;
    std::string option_type = "calls";

    double implied_vol = calculate_implied_volatility_baw(option_price, S, K, r, T, q, option_type);

    std::cout << "Implied Volatility: " << implied_vol << std::endl;

    return 0;
}
