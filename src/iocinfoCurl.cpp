#include "iocinfoCurl.hpp"

void postJson(const nlohmann::json j, const std::string url)
{
  CURL *curl = nullptr;
  CURLcode res = CURLE_OK;

  std::string jsonData = j.dump().c_str();

  curl_global_init(CURL_GLOBAL_ALL);

  curl = curl_easy_init();
  if (curl)
  {
    /* First set the URL that is about to receive our POST. This URL can
       just as well be an https:// URL if that is what should receive the
       data. */
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charset: utf-8");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, response_callback);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L); /* complete within 20 seconds */
    res = curl_easy_perform(curl);
    // TODO: add error handling
    if (res != CURLE_OK)
    {
      fprintf(stderr, "curl_easy_perform() failed:\n URL: %s\n %s\n",
              url.c_str(),
              curl_easy_strerror(res));
    }
    curl_slist_free_all(headers); /* free the list */
    curl_easy_cleanup(curl);      /* cleanup, adresses the file descriptor bug */
  }
}

// /**
//  * @brief callback for response handling
//  */
// static size_t response_callback(void *data, size_t size, size_t nmemb, void *userp)
// {
//   size_t realsize = size * nmemb;
//   return realsize;
// }
