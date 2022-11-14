
#include <epicsThread.h>
#include <iocshDeclWrapper.h>

#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "iocinfoCurl.hpp"
#include "iocinfoData.h"

class iocInfo : public epicsThreadRunable {
 public:
  iocInfo(int arg, const char *name);
  virtual ~iocInfo();
  virtual void run();
  void setUrl(const std::string url);
  void setUrl(const char *url);
  void setVerbose(bool verbose);
  // bool running; // old school
  std::atomic<bool> running;
  std::string url;
  epicsThread thread;
  bool verbose = false;

 private:
  // IocInfoData::Data *data; // use shared_ptr
  std::shared_ptr<IocInfoData::Data> data;
};

iocInfo::iocInfo(int arg, const char *name)
    : running(true),
      url("localhost:1516/iocinfo/"),
      thread(*this, name, epicsThreadGetStackSize(epicsThreadStackSmall), 50) {
  // data = new IocInfoData::Data;
  std::cout << "===>>> iocInfo " << std::endl;
  data = std::make_shared<IocInfoData::Data>();
  // std::cout << "   >>> initial post to URL: " << url << std::endl;
  // postJson(*data->payload, url.c_str());
  std::cout << "<<<" << std::endl;
  // thread.start();  // << called by `iocinfo` instead for better controll
}

iocInfo::~iocInfo() {
  std::cout << "iocinfo destructor" << std::endl;
  // delete (data); // use of shared_ptr makes this obsolete
  running = false;
  thread.exitWait();
}

// TODO: move to header, inline
void iocInfo::setUrl(const std::string URL) { url = URL; }

// TODO: move to header, inline
void iocInfo::setUrl(const char *URL) { url = URL; }

void iocInfo::setVerbose(bool v) {
  if (v) {
    std::cout << "iocInfo verbose is set to true" << std::endl;
  }
  verbose = v;
}

void iocInfo::run() {
  auto start = std::chrono::steady_clock::now();

  while (running) {
    auto now = std::chrono::steady_clock::now();
    // TODO: make delay adjustable
    if (now - start < std::chrono::seconds(10)) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      continue;
    }
    start = now;
    if (verbose) std::cout << "posting data to: " << url << std::endl;
    postJson(*data->payload, url.c_str());
  }
}

/**
 * create object
 */
iocInfo *et2;

/**
 * @brief iocsh function to start iocinfo
 * @param[in] url URL to send data to
 * @details Will start iocinfo thread and send data to url
 */
void iocinfo(const char *url) {
  std::cout << "========================================" << std::endl;
  std::cout << "starting `iocinfo`, data will be send to --> " << url
            << std::endl;
  et2 = new iocInfo(0, "iocInfo");
  et2->setUrl(url);
  et2->thread.start();
}

// TODO: move to header, inline
void iocInfoVerbose(int verbose) { et2->setVerbose((bool)verbose); }

IOCSH_FUNC_WRAP_REGISTRAR(myRegistrar, IOCSH_FUNC_WRAP(iocinfo, "url(string)");
                          IOCSH_FUNC_WRAP(iocInfoVerbose, "on|off (1|0) (int)");
                          // IOCSH_FUNC_WRAP(startPostThread, "url(string)");
                          // IOCSH_FUNC_WRAP_QUIET(stopPostThread);
                          /* more functions may be registered here */
)