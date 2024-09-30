#include <cmath>
#include <stdexcept>
#include <iostream>

/**
 * @brief Approximation of the error function (erf) using a high-precision method.
 *
 * @param x The input value for which the error function is to be calculated.
 * @return double The calculated error function value.
 */
double approx_erf(double x)
{
    const double a1 = 0.254829592;
    const double a2 = -0.284496736;
    const double a3 = 1.421413741;
    const double a4 = -1.453152027;
    const double a5 = 1.061405429;
    const double p = 0.3275911;

    int sign = (x >= 0) ? 1 : -1;
    x = std::fabs(x);

    double t = 1.0 / (1.0 + p * x);
    double y = 1.0 - (((((a5 * t + a4) * t + a3) * t + a2) * t + a1) * t * std::exp(-x * x));

    return sign * y;
}

/**
 * @brief Approximation of the cumulative distribution function (CDF) for a standard normal distribution.
 *
 * @param x The input value for which the CDF is to be calculated.
 * @return double The CDF value.
 */
double normal_cdf(double x)
{
    return 0.5 * (1.0 + approx_erf(x / std::sqrt(2.0)));
}

/**
 * @brief Calculate the price of an American option using the Barone-Adesi Whaley model with dividends.
 *
 * @param S Current stock price.
 * @param K Strike price of the option.
 * @param T Time to expiration in years.
 * @param r Risk-free interest rate.
 * @param sigma Implied volatility.
 * @param q Continuous dividend yield (default is 0.0).
 * @param option_type Type of option ('calls' or 'puts'). Defaults to 'calls'.
 * @return double The calculated option price.
 */
double barone_adesi_whaley_american_option_price(double S, double K, double T, double r, double sigma, double q = 0.0, const std::string &option_type = "calls")
{
    double M = 2 * (r - q) / (sigma * sigma);
    double n = 2 * (r - q - 0.5 * sigma * sigma) / (sigma * sigma);
    double q2 = (-(n - 1) - std::sqrt((n - 1) * (n - 1) + 4 * M)) / 2;

    double d1 = (std::log(S / K) + (r - q + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);

    if (option_type == "calls")
    {
        double european_price = S * std::exp(-q * T) * normal_cdf(d1) - K * std::exp(-r * T) * normal_cdf(d2);
        if (q >= r)
            return european_price;
        if (q2 < 0)
            return european_price;
        double S_critical = K / (1 - 1 / q2);
        if (S >= S_critical)
            return S - K;
        else
        {
            double A2 = (S_critical - K) * std::pow(S_critical, -q2);
            return european_price + A2 * std::pow(S / S_critical, q2);
        }
    }
    else if (option_type == "puts")
    {
        double european_price = K * std::exp(-r * T) * normal_cdf(-d2) - S * std::exp(-q * T) * normal_cdf(-d1);
        if (q >= r)
            return european_price;
        if (q2 < 0)
            return european_price;
        double S_critical = K / (1 + 1 / q2);
        if (S <= S_critical)
            return K - S;
        else
        {
            double A2 = (K - S_critical) * std::pow(S_critical, -q2);
            return european_price + A2 * std::pow(S / S_critical, q2);
        }
    }
    else
    {
        throw std::invalid_argument("option_type must be 'calls' or 'puts'.");
    }
}

/**
 * @brief Calculate the delta of an American option using the Black-Scholes formula with custom normal CDF and dividend yield.
 *
 * @param S Current stock price.
 * @param K Strike price.
 * @param T Time to maturity (in years).
 * @param r Risk-free interest rate (as a decimal).
 * @param sigma Volatility of the underlying asset.
 * @param q Continuous dividend yield (default is 0.0).
 * @param option_type Option type ('calls' or 'puts').
 * @return double The delta of the option.
 */
double calculate_delta(double S, double K, double T, double r, double sigma, double q = 0.0, const std::string &option_type = "calls")
{
    double d1 = (std::log(S / K) + (r - q + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));

    if (option_type == "calls")
    {
        return normal_cdf(d1);
    }
    else if (option_type == "puts")
    {
        return normal_cdf(d1) - 1.0;
    }
    else
    {
        throw std::invalid_argument("option_type must be 'calls' or 'puts'.");
    }
}

/**
 * @brief Calculate the gamma of an American option using numerical differentiation.
 *
 * @param S Current stock price.
 * @param K Strike price.
 * @param T Time to maturity (in years).
 * @param r Risk-free interest rate (as a decimal).
 * @param sigma Volatility of the underlying asset.
 * @param q Continuous dividend yield (default is 0.0).
 * @param option_type Option type ('calls' or 'puts').
 * @return double The gamma of the option.
 */
double calculate_gamma(double S, double K, double T, double r, double sigma, double q = 0.0, const std::string &option_type = "calls")
{
    double h = 1e-4;

    double price_plus = barone_adesi_whaley_american_option_price(S + h, K, T, r, sigma, q, option_type);
    double price = barone_adesi_whaley_american_option_price(S, K, T, r, sigma, q, option_type);
    double price_minus = barone_adesi_whaley_american_option_price(S - h, K, T, r, sigma, q, option_type);

    return (price_plus - 2 * price + price_minus) / (h * h);
}

/**
 * @brief Calculate the vega of an American option using numerical differentiation.
 *
 * @param S Current stock price.
 * @param K Strike price.
 * @param T Time to maturity (in years).
 * @param r Risk-free interest rate (as a decimal).
 * @param sigma Volatility of the underlying asset.
 * @param q Continuous dividend yield (default is 0.0).
 * @param option_type Option type ('calls' or 'puts').
 * @return double The vega of the option.
 */
double calculate_vega(double S, double K, double T, double r, double sigma, double q = 0.0, const std::string &option_type = "calls")
{
    double h = 1e-4;

    double price_plus = barone_adesi_whaley_american_option_price(S, K, T, r, sigma + h, q, option_type);
    double price_minus = barone_adesi_whaley_american_option_price(S, K, T, r, sigma - h, q, option_type);

    return (price_plus - price_minus) / (2 * h);
}

/**
 * @brief Calculate the implied volatility using the Barone-Adesi Whaley model with dividends.
 *
 * @param option_price Observed option price (mid-price).
 * @param S Current stock price.
 * @param K Strike price of the option.
 * @param r Risk-free interest rate.
 * @param T Time to expiration in years.
 * @param q Continuous dividend yield (default is 0.0).
 * @param option_type Option type ('calls' or 'puts'). Defaults to 'calls'.
 * @param max_iterations Maximum number of iterations for the bisection method. Defaults to 100.
 * @param tolerance Convergence tolerance. Defaults to 1e-8.
 * @return double The implied volatility.
 */
double calculate_implied_volatility_baw(double option_price, double S, double K, double r, double T, double q = 0.0, const std::string &option_type = "calls", int max_iterations = 100, double tolerance = 1e-8)
{
    double lower_vol = 1e-5;
    double upper_vol = 10.0;

    for (int i = 0; i < max_iterations; ++i)
    {
        double mid_vol = (lower_vol + upper_vol) / 2;
        double price = barone_adesi_whaley_american_option_price(S, K, T, r, mid_vol, q, option_type);

        if (std::fabs(price - option_price) < tolerance)
        {
            return mid_vol;
        }

        if (price > option_price)
        {
            upper_vol = mid_vol;
        }
        else
        {
            lower_vol = mid_vol;
        }

        if (upper_vol - lower_vol < tolerance)
        {
            break;
        }
    }

    return (lower_vol + upper_vol) / 2;
}
