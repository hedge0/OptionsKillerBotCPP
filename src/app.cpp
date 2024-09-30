#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <fstream>

#include <curl/curl.h>
#include <Eigen/Dense>
#include "nlohmann/json.hpp"

#include "data.h"
#include "filters.h"
#include "models.h"
#include "load_env.h"
#include "load_json.h"
#include "fred.h"
#include "rbf.h"

// Function to write CSV files
void write_csv(const std::string &filename, const Eigen::VectorXd &x_vals, const Eigen::VectorXd &y_vals)
{
    std::ofstream file(filename);
    file << "Strike,IV\n";
    for (Eigen::Index i = 0; i < x_vals.size(); ++i)
    {
        file << x_vals[i] << "," << y_vals[i] << "\n";
    }
    file.close();
}

// Function for option interpolation
void perform_option_interpolation()
{
    while (true)
    {
        initialize_quote_data();

        double S = 566.345;
        double T = 0.015708354371353372;
        double q = 0.0035192;
        std::string option_type = "calls";

        std::vector<double> strikes;
        for (const auto &pair : quote_data)
        {
            strikes.push_back(pair.first);
        }

        std::sort(strikes.begin(), strikes.end());

        std::vector<double> filtered_strikes = filter_strikes(strikes, S, 1.5);

        std::map<double, QuoteData> filtered_data;
        for (double strike : filtered_strikes)
        {
            if (quote_data.find(strike) != quote_data.end())
            {
                filtered_data[strike] = quote_data[strike];
            }
        }

        filtered_data = filter_by_bid_price(filtered_data);

        for (auto &pair : filtered_data)
        {
            double K = pair.first;
            QuoteData &data = pair.second;

            data.mid_IV = calculate_implied_volatility_baw(data.mid, S, K, risk_free_rate, T, q, option_type);
            data.bid_IV = calculate_implied_volatility_baw(data.bid, S, K, risk_free_rate, T, q, option_type);
            data.ask_IV = calculate_implied_volatility_baw(data.ask, S, K, risk_free_rate, T, q, option_type);
        }

        filtered_data = filter_by_mid_iv(filtered_data);
        filtered_strikes.clear();
        for (const auto &pair : filtered_data)
        {
            filtered_strikes.push_back(pair.first);
        }

        // Prepare Eigen vectors for original strikes and mid IVs
        Eigen::VectorXd strike_eigen(filtered_strikes.size());
        Eigen::VectorXd mid_iv_eigen(filtered_strikes.size());

        for (size_t i = 0; i < filtered_strikes.size(); ++i)
        {
            strike_eigen[i] = filtered_strikes[i];
            mid_iv_eigen[i] = filtered_data[filtered_strikes[i]].mid_IV;
        }

        Eigen::VectorXd new_strikes = Eigen::VectorXd::LinSpaced(800, strike_eigen(0), strike_eigen(strike_eigen.size() - 1));
        double epsilon = 0.5;
        double smoothing = 1e-10;

        Eigen::VectorXd interpolated_iv = rbf_interpolation(strike_eigen, mid_iv_eigen, new_strikes, epsilon, smoothing);

        // Write original strikes and mid IVs to CSV
        write_csv("original_strikes_mid_iv.csv", strike_eigen, mid_iv_eigen);

        // Write new strikes and interpolated IVs to CSV
        write_csv("interpolated_strikes_iv.csv", new_strikes, interpolated_iv);

        std::cout << "Data written to CSV files successfully." << std::endl;

        break; // TEMPORARY BREAK
        std::this_thread::sleep_for(std::chrono::seconds(100));
    }
}

int main()
{
    load_env_file(".env");

    if (!schwab_api_key || !schwab_secret || !callback_url || !account_hash || !fred_api_key)
    {
        std::cerr << "Error: One or more environment variables are missing." << std::endl;
        return 1;
    }

    load_json_file("stocks.json");

    for (const auto &stock : stocks_data)
    {
        std::cout << "Ticker: " << stock.at("ticker") << ", Date: " << stock.at("date") << ", Option Type: " << stock.at("option_type") << std::endl;
    }

    fetch_risk_free_rate(fred_api_key);

    // Call the function that runs the while loop and interpolation logic
    perform_option_interpolation();

    return 0;
}
