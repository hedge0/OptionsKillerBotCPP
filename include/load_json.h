#ifndef LOAD_JSON_H
#define LOAD_JSON_H

#include <map>
#include <string>
#include <vector>

extern std::vector<std::map<std::string, std::string>> stocks_data;

void load_json_file(const std::string &file_path);

#endif
