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
#include "nlohmann/json.hpp" // For JSON parsing

// Global variable for risk-free rate
double risk_free_rate = 0.0;

// Callback function for libcurl to store the HTTP response data
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *s)
{
    size_t newLength = size * nmemb;
    try
    {
        s->append((char *)contents, newLength);
    }
    catch (std::bad_alloc &e)
    {
        // Handle memory allocation problem
        return 0;
    }
    return newLength;
}

// Function to get risk-free rate from FRED API
void fetch_risk_free_rate(const std::string &api_key)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    std::string url = "https://api.stlouisfed.org/fred/series/observations?series_id=SOFR&api_key=" + api_key + "&file_type=json";

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check if the request was successful
        if (res != CURLE_OK)
        {
            std::cerr << "CURL Error: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            // Parse the JSON response using nlohmann/json
            auto json_data = nlohmann::json::parse(readBuffer);
            try
            {
                // Get the last SOFR value from the JSON response
                auto observations = json_data["observations"];
                if (!observations.empty())
                {
                    double sofr_value = std::stod(observations.back()["value"].get<std::string>());
                    risk_free_rate = sofr_value / 100; // Convert percentage to decimal
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << "JSON Parsing Error: " << e.what() << std::endl;
            }
        }

        // Clean up curl
        curl_easy_cleanup(curl);
    }
    else
    {
        std::cerr << "Error initializing CURL" << std::endl;
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

    // Fetch risk-free rate from FRED API
    fetch_risk_free_rate(fred_api_key);

    std::cout << "Risk-Free Rate (SOFR): " << risk_free_rate << std::endl;

    initialize_quote_data();

    double S = 566.345;
    double T = 0.015708354371353372;
    double r = risk_free_rate; // Use fetched risk-free rate
    double q = 0.0035192;
    std::string option_type = "calls";
    double strike_filter_value = 1.5;

    std::vector<double> strikes;
    for (const auto &pair : quote_data)
    {
        strikes.push_back(pair.first);
    }

    std::sort(strikes.begin(), strikes.end());

    std::vector<double> filtered_strikes = filter_strikes(strikes, S, strike_filter_value);

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

        data.bid_IV = calculate_implied_volatility_baw(data.bid, S, K, r, T, q, option_type);
        data.ask_IV = calculate_implied_volatility_baw(data.ask, S, K, r, T, q, option_type);
        data.mid_IV = calculate_implied_volatility_baw(data.mid, S, K, r, T, q, option_type);
    }

    filtered_data = filter_by_mid_iv(filtered_data);

    for (const auto &pair : filtered_data)
    {
        std::cout << "Strike: " << pair.first
                  << ", Bid: " << pair.second.bid
                  << ", Ask: " << pair.second.ask
                  << ", Mid: " << pair.second.mid
                  << ", Open Interest: " << pair.second.open_interest
                  << ", Bid IV: " << pair.second.bid_IV
                  << ", Ask IV: " << pair.second.ask_IV
                  << ", Mid IV: " << pair.second.mid_IV
                  << std::endl;
    }

    return 0;
}
