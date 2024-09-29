#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <fstream> // For writing CSV
#include <curl/curl.h>
#include "data.h"
#include "filters.h"
#include "models.h"
#include "load_env.h"
#include "nlohmann/json.hpp"
#include "fred.h"
#include <Eigen/Dense>
#include <pybind11/embed.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void write_csv(const std::string &filename, const std::vector<double> &x_vals, const std::vector<double> &y_vals)
{
    std::ofstream file(filename);
    file << "Strike,IV\n"; // CSV header
    for (size_t i = 0; i < x_vals.size(); ++i)
    {
        file << x_vals[i] << "," << y_vals[i] << "\n";
    }
    file.close();
}

int main()
{
    // Initialize the Python interpreter
    py::scoped_interpreter guard{};

    load_env_file(".env");

    if (!schwab_api_key || !schwab_secret || !callback_url || !account_hash || !fred_api_key)
    {
        std::cerr << "Error: One or more environment variables are missing." << std::endl;
        return 1;
    }

    fetch_risk_free_rate(fred_api_key);
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

    std::vector<double> filtered_strikes = filter_strikes(strikes, S, 1.25);

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

        // Calculate implied volatilities using BAW model
        data.mid_IV = calculate_implied_volatility_baw(data.mid, S, K, risk_free_rate, T, q, option_type);
        data.bid_IV = calculate_implied_volatility_baw(data.bid, S, K, risk_free_rate, T, q, option_type);
        data.ask_IV = calculate_implied_volatility_baw(data.ask, S, K, risk_free_rate, T, q, option_type);
    }

    filtered_data = filter_by_mid_iv(filtered_data);

    try
    {
        // Import necessary Python modules
        py::module numpy = py::module::import("numpy");
        py::module sklearn_preprocessing = py::module::import("sklearn.preprocessing");
        py::module scipy_interpolate = py::module::import("scipy.interpolate");

        // Prepare the data for MinMaxScaler and RBFInterpolator
        py::object strikes_np = numpy.attr("array")(filtered_strikes);
        py::object mid_iv_np = numpy.attr("array")(mid_ivs);

        // Initialize the MinMaxScaler
        py::object MinMaxScaler = sklearn_preprocessing.attr("MinMaxScaler");
        py::object scaler = MinMaxScaler();

        // Apply the scaler to normalize the strikes
        py::object strikes_normalized_np = scaler.attr("fit_transform")(strikes_np.attr("reshape")(-1, 1)).attr("flatten")();

        // Add 0.5 to the normalized values using NumPy's add function
        py::object np_add = numpy.attr("add");
        strikes_normalized_np = np_add(strikes_normalized_np, 0.5); // Add 0.5 to the array

        // Apply the log to the normalized strikes
        py::object log_strikes_normalized_np = numpy.attr("log")(strikes_normalized_np);

        // Create the RBFInterpolator object in Python
        py::object rbf_interpolator = scipy_interpolate.attr("RBFInterpolator")(
            log_strikes_normalized_np.attr("reshape")(-1, 1), mid_iv_np, py::arg("kernel") = "multiquadric", py::arg("epsilon") = 0.5, py::arg("smoothing") = 1e-10);

        // Generate new strikes for interpolation
        py::object new_strikes_normalized_np = numpy.attr("linspace")(strikes_normalized_np.attr("min")(), strikes_normalized_np.attr("max")(), 800);
        py::object log_new_strikes_normalized_np = numpy.attr("log")(new_strikes_normalized_np);

        // Perform interpolation
        py::object interpolated_iv_np = rbf_interpolator(log_new_strikes_normalized_np.attr("reshape")(-1, 1));

        // Convert the results back to C++ std::vector

        py::object new_strikes_np_fit = numpy.attr("linspace")(strikes_np.attr("min")(), strikes_np.attr("max")(), 800);
        std::vector<double> new_strikes = new_strikes_np_fit.cast<std::vector<double>>();
        std::vector<double> interpolated_iv = interpolated_iv_np.cast<std::vector<double>>();

        // Write original strikes and mid IVs to CSV
        write_csv("original_strikes_mid_iv.csv", filtered_strikes, mid_ivs);

        // Write new strikes and interpolated IVs to CSV
        write_csv("interpolated_strikes_iv.csv", new_strikes, interpolated_iv);

        std::cout << "Data written to CSV files successfully." << std::endl;
    }
    catch (py::error_already_set &e)
    {
        std::cerr << "Python error: " << e.what() << std::endl;
    }

    return 0;
}
