#ifndef __IOCINFO_CURL_HPP
#define __IOCINFO_CURL_HPP

#include <string>
#include <curl/curl.h>
#include "nlohmann/json.hpp"

void postJson(const nlohmann::json j, const std::string url);
static size_t response_callback(void *data, size_t size, size_t nmemb, void *userp);

#endif // __IOCINFO_CURL_HPP