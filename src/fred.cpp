#include <iostream>
#include <curl/curl.h>
#include "nlohmann/json.hpp"
#include "fred.h"

/**
 * @brief Global variable to store the risk-free rate fetched from the FRED API.
 */
double risk_free_rate = 0.0;

/**
 * @brief Callback function used by libcurl to store HTTP response data into a string.
 *
 * @param contents Pointer to the contents of the data received.
 * @param size Size of each data chunk.
 * @param nmemb Number of data chunks.
 * @param s Pointer to the string where data will be appended.
 * @return Size of the appended data in bytes.
 */
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *s)
{
    size_t newLength = size * nmemb;
    try
    {
        s->append((char *)contents, newLength);
    }
    catch (std::bad_alloc &e)
    {
        std::cerr << "Memory allocation failed: " << e.what() << std::endl;
        return 0;
    }
    return newLength;
}

/**
 * @brief Fetches the risk-free rate (SOFR) from the FRED API and updates the global risk-free rate variable.
 *
 * @param api_key The FRED API key used to authenticate the request.
 */
void fetch_risk_free_rate(const std::string &api_key)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    std::string url = "https://api.stlouisfed.org/fred/series/observations?series_id=SOFR&api_key=" + api_key + "&file_type=json";

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            std::cerr << "CURL Error: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            try
            {
                auto json_data = nlohmann::json::parse(readBuffer);
                auto observations = json_data["observations"];
                if (!observations.empty())
                {
                    double sofr_value = std::stod(observations.back()["value"].get<std::string>());
                    risk_free_rate = sofr_value / 100;
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << "JSON Parsing Error: " << e.what() << std::endl;
            }
        }

        curl_easy_cleanup(curl);
    }
    else
    {
        std::cerr << "Error initializing CURL" << std::endl;
    }
}
