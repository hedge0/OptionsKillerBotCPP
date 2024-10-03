#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include "load_env.h"

/**
 * @brief Global variable to store the Schwab API key.
 */
std::string schwab_api_key;

/**
 * @brief Global variable to store the Schwab secret.
 */
std::string schwab_secret;

/**
 * @brief Global variable to store the callback URL for Schwab authentication.
 */
std::string callback_url;

/**
 * @brief Global variable to store the account hash for Schwab.
 */
std::string account_hash;

/**
 * @brief Global variable to store the FRED API key.
 */
std::string fred_api_key;

/**
 * @brief Global variable to store the DRY_RUN flag.
 */
bool dry_run = true;

/**
 * @brief Global variable to store the TIME_TO_REST value.
 */
int time_to_rest = 100; // Default value in milliseconds

/**
 * @brief Loads environment variables from a .env file.
 *
 * This function reads a .env file and sets global variables based on the key-value pairs.
 * It supports comment lines (starting with '#') and trims whitespace around keys and values.
 *
 * @param file_path The path to the .env file to be loaded.
 */
void load_env_file(const std::string &file_path)
{
    std::ifstream file(file_path);
    if (!file.is_open())
    {
        std::cerr << "Could not open .env file: " << file_path << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream ss(line);
        std::string key, value;

        if (std::getline(ss, key, '=') && std::getline(ss, value))
        {
            key.erase(0, key.find_first_not_of(" \t\n\r\f\v"));
            key.erase(key.find_last_not_of(" \t\n\r\f\v") + 1);
            value.erase(0, value.find_first_not_of(" \t\n\r\f\v"));
            value.erase(value.find_last_not_of(" \t\n\r\f\v") + 1);

            if (key == "SCHWAB_API_KEY")
            {
                schwab_api_key = value;
            }
            else if (key == "SCHWAB_SECRET")
            {
                schwab_secret = value;
            }
            else if (key == "SCHWAB_CALLBACK_URL")
            {
                callback_url = value;
            }
            else if (key == "SCHWAB_ACCOUNT_HASH")
            {
                account_hash = value;
            }
            else if (key == "FRED_API_KEY")
            {
                fred_api_key = value;
            }
            else if (key == "DRY_RUN")
            {
                dry_run = (value == "true" || value == "TRUE" || value == "1");
            }
            else if (key == "TIME_TO_REST")
            {
                try
                {
                    time_to_rest = std::stoi(value);
                }
                catch (const std::exception &)
                {
                    std::cerr << "Invalid TIME_TO_REST value: " << value << ". Using default value." << std::endl;
                }
            }
        }
    }

    file.close();
}
