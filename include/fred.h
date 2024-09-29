#ifndef FRED_H
#define FRED_H

#include <string>

extern double risk_free_rate;

void fetch_risk_free_rate(const std::string &api_key);

#endif
