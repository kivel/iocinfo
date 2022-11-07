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

#include <epicsThread.h>
#include "cantProceed.h"

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
 * @param[in] payload json payload
 * @param[in] url URL to post to
 */
static void postJson(const nlohmann::json payload, const char *url)
{
  std::cout << "=======> posting data to URL: " << url << std::endl;
  CURL *curl = nullptr;
  CURLcode res = CURLE_OK;

  std::string data = payload.dump();

  curl_global_init(CURL_GLOBAL_ALL);

  curl = curl_easy_init();
  if (curl)
  {
    /* First set the URL that is about to receive our POST. This URL can
       just as well be an https:// URL if that is what should receive the
       data. */
    curl_easy_setopt(curl, CURLOPT_URL, url);
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charset: utf-8");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, response_callback);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    res = curl_easy_perform(curl);
    // TODO: add error handling
    if (res != CURLE_OK)
    {
      // fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
      std::cerr << "EE: curl_easy_perform() failed:" << curl_easy_strerror(res) << std::endl;
      std::cerr << "EE: url" << url << std::endl;
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
// std::unique_ptr<std::thread> postThread; // Thread periodically posting data to elasticsearch
// epicsThreadId epicsPostThread;
std::atomic<bool> stopPost; // Setting this flag will stop the posting of data to elasticsearch
// IocInfoData::Data *data;

typedef struct iocinfoPrivData
{
  const char *url;               // url to post data to elasticsearch
  IocInfoData::Data *data;       // json data
  epicsThreadId epicsPostThread; // thread

} iocinfoPrivData;

/**
 * @brief worker thread function
 * @param[in] data pointer to data structure
 * @param[in] url url to post to
 * @todo add variable for sleep_for
 */
// void worker(IocInfoData::Data *data, std::string url)
void worker(void *input)
{
  struct iocinfoPrivData *priv = (iocinfoPrivData *)(input);

  std::cout << "== worker started" << std::endl;
  postJson(*priv->data->payload, priv->url);
  std::cerr << "url addr is " << &priv->url << std::endl;
  auto start = std::chrono::steady_clock::now();

  while (!stopPost)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    auto now = std::chrono::steady_clock::now();
    // only post data every n seconds
    if ((now - start) > std::chrono::seconds(10))
    {
      postJson(*priv->data->payload, priv->url);
      std::cerr << "url addr is " << &priv->url << std::endl;
      start = now;
    }
  }
  // delete(priv);
}

/**
 * @brief start post thread function
 * @todo remove url and obtain from global scope, which is set through dedicated function
 */
void iocinfoStart(const char *url)
{
  struct iocinfoPrivData *priv;
  priv = new iocinfoPrivData;

  // priv->url = "svd-ciddock01:1516/iocinfo/";
  priv->url = url;
  priv->data = new IocInfoData::Data;

  stopPost = false;

  priv->epicsPostThread = epicsThreadMustCreate("jsonPostWorker",
                                                epicsThreadPriorityLow,
                                                epicsThreadGetStackSize(epicsThreadStackSmall),
                                                &worker, priv);
}

/**
 * @brief stop post thread function
 */
void iocinfoStop()
{
  // stopPost = true;
  // if (postThread.get())
  // {
  //   try
  //   {
  //     postThread->join();
  //   }
  //   catch (const std::system_error &e)
  //   {
  //     std::cout << "failed to join thread" << std::endl;
  //     std::cout << e.what() << std::endl;
  //     // Do nothing since we are destroying everything anyway.
  //   }
  // }
  // delete (data);
}

/**
 * @brief foo
 */
void iocinfo(const char *url)
{
  IocInfoData::Data *data;
  data = new IocInfoData::Data;

  // writeJson(*data->payload, "/tmp/iocinfo.json");
  postJson(*data->payload, url);

  delete (data);
}

IOCSH_FUNC_WRAP_REGISTRAR(myRegistrar,
                          IOCSH_FUNC_WRAP(iocinfo, "url(string)");
                          IOCSH_FUNC_WRAP(iocinfoStart, "url(string)");
                          IOCSH_FUNC_WRAP(iocinfoStop);
                          /* more functions may be registered here */
)