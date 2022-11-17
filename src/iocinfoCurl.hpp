#ifndef __IOCINFO_CURL_HPP
#define __IOCINFO_CURL_HPP

#include <string>
#include <curl/curl.h>
#include "nlohmann/json.hpp"

/**
 * @brief post json data to server
 * @param[in] j json data
 * @param[in] url the URL to post the data
 */
void postJson(const nlohmann::json j, const std::string url);

/**
 * @brief callback for response handling
 */
static inline size_t response_callback(void *data, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  return realsize;
}

#endif // __IOCINFO_CURL_HPP