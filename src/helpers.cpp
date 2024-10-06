#include "helpers.h"
#include <fstream>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <iostream>
#include <cmath>

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

/**
 * @brief Check if the New York Stock Exchange (NYSE) is currently open.
 *
 * This function checks if the current time falls within the trading hours
 * of the NYSE, which operates Monday through Friday from 9:30 AM to 3:45 PM EST.
 * It takes into account the current day of the week to exclude weekends
 * (Saturday and Sunday), and it considers both the opening and closing times.
 *
 * @return True if the NYSE is currently open, false otherwise.
 */
bool is_nyse_open()
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    std::tm local_time;
#ifdef _WIN32
    localtime_s(&local_time, &now_c);
#else
    localtime_r(&now_c, &local_time);
#endif

    int current_hour = local_time.tm_hour;
    int current_minute = local_time.tm_min;
    int current_day_of_week = local_time.tm_wday;

    if (current_day_of_week == 0 || current_day_of_week == 6)
    {
        return false;
    }

    int open_hour = 9;
    int open_minute = 30;
    int close_hour = 15;
    int close_minute = 45;

    if ((current_hour > open_hour || (current_hour == open_hour && current_minute >= open_minute)) &&
        (current_hour < close_hour || (current_hour == close_hour && current_minute < close_minute)))
    {
        return true;
    }

    return false;
}

/**
 * @brief Perform 1D linear interpolation.
 *
 * This function interpolates the values of a function \( y = f(xp) \) at the points \( x \),
 * given discrete data points \((xp, fp)\) using linear interpolation.
 * It returns a vector \( y \) such that \( y[i] = f(x[i]) \).
 *
 * @param x Points at which to interpolate.
 * @param xp Known data points (must be sorted in ascending order).
 * @param fp Values at the known data points.
 * @return Interpolated values at points x.
 */
Eigen::VectorXd interp1d(const Eigen::VectorXd &x, const Eigen::VectorXd &xp, const Eigen::VectorXd &fp)
{
    Eigen::VectorXd y(x.size());

    for (Eigen::Index i = 0; i < x.size(); ++i)
    {
        double xi = x[i];

        if (xi <= xp[0])
        {
            y[i] = fp[0];
        }
        else if (xi >= xp[xp.size() - 1])
        {
            y[i] = fp[fp.size() - 1];
        }
        else
        {
            Eigen::Index j = std::upper_bound(xp.data(), xp.data() + xp.size(), xi) - xp.data() - 1;

            double x0 = xp[j];
            double x1 = xp[j + 1];
            double y0 = fp[j];
            double y1 = fp[j + 1];

            double t = (xi - x0) / (x1 - x0);
            y[i] = y0 + t * (y1 - y0);
        }
    }

    return y;
}

/**
 * @brief Calculate the Root Mean Squared Error (RMSE) between two vectors.
 *
 * This function computes the RMSE between the true values \( y_{\text{true}} \) and the predicted values \( y_{\text{pred}} \).
 * RMSE is a measure of the differences between values predicted by a model and the values actually observed.
 *
 * @param y_true Vector of true values.
 * @param y_pred Vector of predicted values.
 * @return The RMSE value.
 */
double calculate_rmse(const Eigen::VectorXd &y_true, const Eigen::VectorXd &y_pred)
{
    if (y_true.size() != y_pred.size())
    {
        std::cerr << "Error: Vectors y_true and y_pred must have the same size." << std::endl;
        return -1.0;
    }

    Eigen::VectorXd diff = y_true - y_pred;
    double mse = diff.squaredNorm() / static_cast<double>(y_true.size());
    double rmse = std::sqrt(mse);
    return rmse;
}
