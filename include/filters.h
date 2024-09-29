#ifndef FILTERS_H
#define FILTERS_H

#include <vector>
#include <map>
#include "data.h"

double calculate_standard_deviation(const std::vector<double> &strikes);
std::vector<double> filter_strikes(const std::vector<double> &strikes, double S, double num_stdev = 1.25, bool two_sigma_move = false);
std::map<double, QuoteData> filter_by_bid_price(const std::map<double, QuoteData> &data);
std::map<double, QuoteData> filter_by_mid_iv(const std::map<double, QuoteData> &data);

#endif
