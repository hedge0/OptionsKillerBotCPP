#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <curl/curl.h>
#include "data.h"
#include "filters.h"
#include "models.h"
#include "load_env.h"
#include "load_json.h"
#include "nlohmann/json.hpp"
#include "fred.h"
#include <Eigen/Dense>
#include "rbf.h"

int main()
{
    load_env_file(".env");
    load_json_file("stocks.json");

    if (!schwab_api_key || !schwab_secret || !callback_url || !account_hash || !fred_api_key)
    {
        std::cerr << "Error: One or more environment variables are missing." << std::endl;
        return 1;
    }

    for (const auto &stock : stocks_data)
    {
        std::cout << "Ticker: " << stock.at("ticker") << ", Date: " << stock.at("date") << ", Option Type: " << stock.at("option_type") << std::endl;
    }

    fetch_risk_free_rate(fred_api_key);

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

        // Now proceed with interpolation
        Eigen::VectorXd strike_eigen(filtered_strikes.size());
        Eigen::VectorXd mid_iv_eigen(filtered_strikes.size());

        // Populate Eigen Vectors
        for (size_t i = 0; i < filtered_strikes.size(); ++i)
        {
            strike_eigen[i] = filtered_strikes[i];
            mid_iv_eigen[i] = filtered_data[filtered_strikes[i]].mid_IV;
        }

        // Define new points for interpolation
        Eigen::VectorXd new_strikes = Eigen::VectorXd::LinSpaced(800, filtered_strikes.front(), filtered_strikes.back());
        double epsilon = 0.5;
        double smoothing = 1e-10;

        // Perform RBF interpolation
        Eigen::VectorXd interpolated_iv = rbf_interpolation(strike_eigen, mid_iv_eigen, new_strikes, epsilon, smoothing);

        // Sleep for 100000 seconds before the next iteration
        break;
        std::this_thread::sleep_for(std::chrono::seconds(100000));
    }

    return 0;
}
