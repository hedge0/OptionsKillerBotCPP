#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include "Eigen/Dense"
#include "helpers.h"

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

/**
 * @brief Check if the New York Stock Exchange (NYSE) is currently open.
 *
 * This function checks if the current time falls within the trading hours
 * of the NYSE, which operates Monday through Friday from 9:30 AM to 3:45 PM EST.
 * It takes into account the current day of the week to exclude weekends
 * (Saturday and Sunday), and it considers both the opening and closing times.
 *
 * @return bool True if the NYSE is currently open, false otherwise.
 */
bool is_nyse_open()
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    std::tm local_time;
    localtime_s(&local_time, &now_c);

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
