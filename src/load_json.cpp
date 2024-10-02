#include <iostream>
#include <fstream>
#include <string>
#include "nlohmann/json.hpp"
#include "load_json.h"

/**
 * @brief Global pointer to the head of the circular linked list.
 */
StockNode *stocks_data_head = nullptr;

/**
 * @brief Loads a JSON file into memory and stores its data in a circular linked list.
 *
 * This function reads a JSON file, parses its content, and stores the resulting
 * data into a circular linked list. Each JSON object is mapped to a node in the
 * linked list containing "ticker", "date", "option_type", "min_overpriced",
 * "min_underpriced", and "min_oi" keys.
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

        StockNode *tail = nullptr;

        for (const auto &item : json_data)
        {
            StockNode *new_node = new StockNode;
            new_node->ticker = item.at("ticker");
            new_node->date = std::to_string(item.at("date").get<int>());
            new_node->option_type = item.at("option_type");
            new_node->min_overpriced = std::to_string(item.at("min_overpriced").get<double>());
            new_node->min_underpriced = std::to_string(item.at("min_underpriced").get<double>());
            new_node->min_oi = std::to_string(item.at("min_oi").get<int>());
            new_node->next = nullptr;

            if (stocks_data_head == nullptr)
            {
                stocks_data_head = new_node;
                tail = new_node;
            }
            else
            {
                tail->next = new_node;
                tail = new_node;
            }
        }

        if (tail != nullptr)
        {
            tail->next = stocks_data_head;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error parsing JSON file: " << e.what() << std::endl;
    }

    file.close();
}
