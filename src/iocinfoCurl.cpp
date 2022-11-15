#include "iocinfoCurl.hpp"
#include <iostream>

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
    res = curl_easy_perform(curl);
    // TODO: add error handling
    if (res != CURLE_OK)
    {
      std::cout << res << std::endl;
    }
    curl_slist_free_all(headers); /* free the list */
  }
}

/**
 * @brief callback for response handling
 */
static size_t response_callback(void *data, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  return realsize;
}

// static void writeJson(const nlohmann::json payload, const std::string fname)
// {
//   std::ofstream file(fname);
//   file << payload.dump() << std::endl;
// }



// /**
//  * @brief post payload to the server
//  * @param[in] payload json payload
//  * @param[in] url URL to post to
//  */
// static void postJson(const nlohmann::json payload, const char *url)
// {
//   std::cout << "=======> posting data to URL: " << url << std::endl;
//   CURL *curl = nullptr;
//   CURLcode res = CURLE_OK;

//   std::string data = payload.dump();

//   curl_global_init(CURL_GLOBAL_ALL);

//   curl = curl_easy_init();
//   if (curl)
//   {
//     /* First set the URL that is about to receive our POST. This URL can
//        just as well be an https:// URL if that is what should receive the
//        data. */
//     curl_easy_setopt(curl, CURLOPT_URL, url);
//     struct curl_slist *headers = nullptr;
//     headers = curl_slist_append(headers, "Accept: application/json");
//     headers = curl_slist_append(headers, "Content-Type: application/json");
//     headers = curl_slist_append(headers, "charset: utf-8");
//     curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
//     curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, response_callback);
//     curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
//     res = curl_easy_perform(curl);
//     // TODO: add error handling
//     if (res != CURLE_OK)
//     {
//       // fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
//       std::cerr << "EE: curl_easy_perform() failed:" << curl_easy_strerror(res) << std::endl;
//       std::cerr << "EE: url" << url << std::endl;
//     }

//     /* always cleanup */
//     curl_easy_cleanup(curl);
//     delete (headers);
//   }
// }