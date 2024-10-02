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

// Function to check if NYSE is open
bool is_nyse_open()
{
    // Get the current time
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    std::tm local_time;
    localtime_s(&local_time, &now_c);

    // NYSE operates Monday - Friday from 9:30 AM to 4:00 PM EST
    int current_hour = local_time.tm_hour;
    int current_minute = local_time.tm_min;
    int current_day_of_week = local_time.tm_wday;

    // If today is Saturday (6) or Sunday (0), NYSE is closed
    if (current_day_of_week == 0 || current_day_of_week == 6)
    {
        return false;
    }

    // NYSE opens at 9:30 AM and closes at 4:00 PM
    int open_hour = 9;
    int open_minute = 30;
    int close_hour = 16;
    int close_minute = 0;

    // Check if current time is within the NYSE open hours
    if ((current_hour > open_hour || (current_hour == open_hour && current_minute >= open_minute)) &&
        (current_hour < close_hour || (current_hour == close_hour && current_minute < close_minute)))
    {
        return true;
    }

    return false;
}
