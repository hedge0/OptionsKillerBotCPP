#ifndef DATA_H
#define DATA_H

#include <map>

struct QuoteData
{
    double bid;
    double ask;
    double mid;
    double open_interest;
    double bid_IV;
    double ask_IV;
    double mid_IV;
};

extern std::map<double, QuoteData> quote_data;

void initialize_quote_data();

#endif
