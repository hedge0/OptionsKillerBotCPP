#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <curl/curl.h>
#include "data.h"
#include "filters.h"
#include "models.h"
#include "load_env.h"
#include "nlohmann/json.hpp"
#include "fred.h"
#include <chrono>
#include <thread> // For sleep

int main()
{
    load_env_file(".env");

    if (!schwab_api_key || !schwab_secret || !callback_url || !account_hash || !fred_api_key)
    {
        std::cerr << "Error: One or more environment variables are missing." << std::endl;
        return 1;
    }

    fetch_risk_free_rate(fred_api_key);

    for (int i = 0; i < 10; ++i)
    {
        initialize_quote_data();
        std::cout << "Iteration " << (i + 1) << std::endl;

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

        auto start = std::chrono::high_resolution_clock::now();

        for (auto &pair : filtered_data)
        {
            double K = pair.first;
            QuoteData &data = pair.second;

            data.mid_IV = calculate_implied_volatility_baw(data.mid, S, K, risk_free_rate, T, q, option_type);
            data.bid_IV = calculate_implied_volatility_baw(data.bid, S, K, risk_free_rate, T, q, option_type, data.mid_IV);
            data.ask_IV = calculate_implied_volatility_baw(data.ask, S, K, risk_free_rate, T, q, option_type, data.mid_IV);
        }

        // End timing
        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the duration
        std::chrono::duration<double> duration = end - start;

        // Print the duration in seconds
        std::cout << "PerformTest took " << duration.count() << " seconds." << std::endl;

        filtered_data = filter_by_mid_iv(filtered_data);

        // Wait for 3 seconds before the next iteration
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
