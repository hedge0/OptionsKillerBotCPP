#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include "load_env.h"

/**
 * @brief Global variable to store the Schwab API key.
 */
const char *schwab_api_key = nullptr;

/**
 * @brief Global variable to store the Schwab secret.
 */
const char *schwab_secret = nullptr;

/**
 * @brief Global variable to store the callback URL for Schwab authentication.
 */
const char *callback_url = nullptr;

/**
 * @brief Global variable to store the account hash for Schwab.
 */
const char *account_hash = nullptr;

/**
 * @brief Global variable to store the FRED API key.
 */
const char *fred_api_key = nullptr;

/**
 * @brief Loads environment variables from a .env file.
 *
 * This function reads a .env file and sets environment variables using either `_putenv` on Windows systems
 * or `setenv` on Unix-based systems. It supports comment lines (starting with '#') and trims whitespace
 * around keys and values.
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
            key.erase(key.find_last_not_of(" \t\n\r\f\v") + 1);
            value.erase(value.find_last_not_of(" \t\n\r\f\v") + 1);

            std::string env_var = key + "=" + value;

#ifdef _WIN32
            // For Windows, use _putenv
            _putenv(env_var.c_str());
#else
            // For Unix/Linux/Mac systems, use setenv
            setenv(key.c_str(), value.c_str(), 1);
#endif
            // Safely retrieve environment variables using _dupenv_s
            if (key == "SCHWAB_API_KEY")
            {
                char *buffer = nullptr;
                size_t len;
                _dupenv_s(&buffer, &len, "SCHWAB_API_KEY");
                schwab_api_key = buffer;
            }
            else if (key == "SCHWAB_SECRET")
            {
                char *buffer = nullptr;
                size_t len;
                _dupenv_s(&buffer, &len, "SCHWAB_SECRET");
                schwab_secret = buffer;
            }
            else if (key == "SCHWAB_CALLBACK_URL")
            {
                char *buffer = nullptr;
                size_t len;
                _dupenv_s(&buffer, &len, "SCHWAB_CALLBACK_URL");
                callback_url = buffer;
            }
            else if (key == "SCHWAB_ACCOUNT_HASH")
            {
                char *buffer = nullptr;
                size_t len;
                _dupenv_s(&buffer, &len, "SCHWAB_ACCOUNT_HASH");
                account_hash = buffer;
            }
            else if (key == "FRED_API_KEY")
            {
                char *buffer = nullptr;
                size_t len;
                _dupenv_s(&buffer, &len, "FRED_API_KEY");
                fred_api_key = buffer;
            }
        }
    }

    file.close();
}
