#ifndef LOAD_JSON_H
#define LOAD_JSON_H

#include <string>

struct StockNode
{
    std::string ticker;
    std::string date;
    std::string option_type;
    std::string min_overpriced;
    std::string min_underpriced;
    std::string min_oi;
    StockNode *next;
};

extern StockNode *stocks_data_head;

void load_json_file(const std::string &file_path);

#endif
