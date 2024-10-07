#ifndef FILTERS_H
#define FILTERS_H

#include <config.hpp>
#include <vector>
#include <map>
#include "data.hpp"

class filters {
  public:
	/**
	* @brief Calculate the standard deviation of a vector of strike prices.
	*
	* @param strikes A vector of strike prices.
	* @return double The calculated standard deviation.
	*/
	OPTS_BOT_ALWAYS_INLINE static double calculate_standard_deviation(const std::vector<double>& strikes) {
		size_t n	= strikes.size();
		double mean = std::accumulate(strikes.begin(), strikes.end(), 0.0) / n;

		double variance_sum = std::accumulate(strikes.begin(), strikes.end(), 0.0, [mean](double acc, double strike) {
			return acc + (strike - mean) * (strike - mean);
		});

		return std::sqrt(variance_sum / n);
	}

	/**
	* @brief Filter strike prices within a specified range based on standard deviations.
	*
	* @param strikes A vector of strike prices.
	* @param S The underlying asset's current price.
	* @param num_stdev The number of standard deviations for filtering (default is 1.25).
	* @param two_sigma_move A boolean indicating whether to use a 2-sigma move for upper bound (default is false).
	* @return std::vector<double> A vector of filtered strike prices.
	*/
	OPTS_BOT_ALWAYS_INLINE static std::vector<double> filter_strikes(const std::vector<double>& strikes, double S, double num_stdev = 1.25, bool two_sigma_move = false) {
		double stdev	   = calculate_standard_deviation(strikes);
		double lower_bound = S - num_stdev * stdev;
		double upper_bound = S + num_stdev * stdev;

		if (two_sigma_move) {
			upper_bound = S + 2 * stdev;
		}

		std::vector<double> filtered_strikes;
		for (double strike: strikes) {
			if (strike >= lower_bound && strike <= upper_bound) {
				filtered_strikes.push_back(strike);
			}
		}

		return filtered_strikes;
	}

	/**
	* @brief Filter the map of QuoteData, removing entries where the bid price is 0.0.
	*
	* @param data A map of strike prices to QuoteData objects.
	* @return std::map<double, QuoteData> A filtered map containing only entries where the bid price is not 0.0.
	*/
	OPTS_BOT_ALWAYS_INLINE static std::map<double, QuoteData> filter_by_bid_price(const std::map<double, QuoteData>& data) {
		std::map<double, QuoteData> filtered_data;

		for (const auto& pair: data) {
			if (pair.second.bid != 0.0) {
				filtered_data[pair.first] = pair.second;
			}
		}

		return filtered_data;
	}

	/**
	* @brief Filter the map of QuoteData, removing entries where the mid_IV is <= 0.005.
	*
	* @param data A map of strike prices to QuoteData objects.
	* @return std::map<double, QuoteData> A filtered map containing only entries where mid_IV is greater than 0.005.
	*/
	OPTS_BOT_ALWAYS_INLINE static std::map<double, QuoteData> filter_by_mid_iv(const std::map<double, QuoteData>& data) {
		std::map<double, QuoteData> filtered_data;

		for (const auto& pair: data) {
			if (pair.second.mid_IV > 0.005) {
				filtered_data[pair.first] = pair.second;
			}
		}

		return filtered_data;
	}
};

#endif
