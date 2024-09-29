#ifndef LOAD_ENV_H
#define LOAD_ENV_H

#include <string>

extern const char *schwab_api_key;
extern const char *schwab_secret;
extern const char *callback_url;
extern const char *account_hash;
extern const char *fred_api_key;

void load_env_file(const std::string &file_path);

#endif
