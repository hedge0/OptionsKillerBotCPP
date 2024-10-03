#ifndef LOAD_ENV_H
#define LOAD_ENV_H

#include <string>

extern std::string schwab_api_key;
extern std::string schwab_secret;
extern std::string callback_url;
extern std::string account_hash;
extern std::string fred_api_key;
extern bool dry_run;
extern int time_to_rest;

void load_env_file(const std::string &file_path);

#endif
