#include <curl/curl.h>
#include <iocshDeclWrapper.h>

#include <fstream>
#include <iostream>
#include <string>
#include <memory>
#include <atomic>
#include <thread>
#include <chrono>
#include <system_error>

#include "iocinfoData.h"

/**
 * @brief callback for response handling
 */
static size_t response_callback(void *data, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  return realsize;
}

/**
 * @brief post payload to the server
 * @param[in] j json payload
 * @param[in] url URL to post to
 */
static void postJson(const nlohmann::json j, const std::string url)
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
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    }

    /* always cleanup */
    curl_easy_cleanup(curl);
    delete (headers);
  }
}

/**
 * @brief write payload to file
 * @param[in] payload json payload
 * @param[in] filename filename to write to
 */
static void writeJson(const nlohmann::json payload, const std::string fname)
{
  std::ofstream file(fname);
  file << payload.dump() << std::endl;
}

/**
 * @brief global variables
 */
std::unique_ptr<std::thread> postThread; //!< Thread periodically reading back setpoint values.
std::atomic<bool> stopPost;              //!< Setting this flag will stop the parameter thread loop.
IocInfoData::Data *data;

/**
 * @brief worker thread function
 * @param[in] data pointer to data structure
 * @param[in] url url to post to
 * @todo add variable for sleep_for
 */
void worker(IocInfoData::Data *data, std::string url)
{
  std::cout << "== worker started" << std::endl;
  while (!stopPost)
  {
    postJson(*data->payload, url);
    std::this_thread::sleep_for(std::chrono::seconds(10));
  }
}

/**
 * @brief start post thread function
 * @todo remove url and obtain from global scope, which is set through dedicated function
 */
void startPostThread(const char *url)
{
  stopPost = false;
  // WTF! This doesn't work ???
  // std::shared_ptr<IocInfoData::Data> data = std::make_shared<IocInfoData::Data>();
  data = new IocInfoData::Data;

  writeJson(*data->payload, "/tmp/iocinfo.json");

  // need to use `new` here, as `std::make_unique` is only available in C++17
  postThread.reset(new std::thread(worker, data, (std::string)url));

  // TODO: should be cleaned up, shouldn't it?
}

/**
 * @brief stop post thread function
 */
void stopPostThread()
{
  stopPost = true;
  if (postThread.get())
  {
    try
    {
      postThread->join();
    }
    catch (const std::system_error &e)
    {
      // Do nothing since we are destroying everything anyway.
    }
  }
  delete (data);
}

/**
 * @brief foo
 */
void iocinfo(const char *url)
{
  startPostThread(url);
  // IocInfoData::Data *data;
  // data = new IocInfoData::Data;

  // writeJson(*data->payload, "/tmp/iocinfo.json");
  // postJson(*data->payload, (std::string)url);

  // delete (data);
}

IOCSH_FUNC_WRAP_REGISTRAR(myRegistrar,
                          IOCSH_FUNC_WRAP(iocinfo, "url(string)");
                          IOCSH_FUNC_WRAP(startPostThread, "url(string)");
                          IOCSH_FUNC_WRAP(stopPostThread);
                          /* more functions may be registered here */
)