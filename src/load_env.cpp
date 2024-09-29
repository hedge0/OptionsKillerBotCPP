#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include "load_env.h"

#ifdef __MINGW32__
extern "C" int putenv(char *);
#endif

/**
 * @brief Loads environment variables from a .env file.
 *
 * This function reads a .env file and sets environment variables using either `putenv` on Windows systems
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
            // For MinGW and MSVC, use putenv (requires dynamically allocated memory for MinGW)
            char *env_var_cstr = new char[env_var.size() + 1];
            std::strcpy(env_var_cstr, env_var.c_str());
            putenv(env_var_cstr);
#else
            // For Unix/Linux/Mac systems, use setenv
            setenv(key.c_str(), value.c_str(), 1);
#endif
        }
    }

    file.close();
}