#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include "data.h"
#include "filters.h"
#include "models.h"

int main()
{
    initialize_quote_data();

    double S = 566.345;
    double T = 0.015708354371353372;
    double r = 0.0483;
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
