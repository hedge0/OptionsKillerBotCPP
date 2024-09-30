#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include "nlohmann/json.hpp"
#include "load_json.h"

/**
 * @brief Global variable to store JSON data from stocks.json.
 */
std::vector<std::map<std::string, std::string>> stocks_data;

/**
 * @brief Loads a JSON file into memory and stores its data in a global variable.
 *
 * This function reads a JSON file, parses its content, and stores the resulting
 * data into a global variable `stocks_data`. Each JSON object is mapped to a
 * dictionary containing "ticker", "date", and "option_type" keys.
 *
 * @param file_path The path to the JSON file to be loaded.
 */
void load_json_file(const std::string &file_path)
{
    std::ifstream file(file_path);
    if (!file.is_open())
    {
        std::cerr << "Could not open JSON file: " << file_path << std::endl;
        return;
    }

    try
    {
        nlohmann::json json_data;
        file >> json_data;

        for (const auto &item : json_data)
        {
            std::map<std::string, std::string> stock_entry;
            stock_entry["ticker"] = item.at("ticker");
            stock_entry["date"] = std::to_string(item.at("date").get<int>());
            stock_entry["option_type"] = item.at("option_type");

            stocks_data.push_back(stock_entry);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error parsing JSON file: " << e.what() << std::endl;
    }

    file.close();
}
