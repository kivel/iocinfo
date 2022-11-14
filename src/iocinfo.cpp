#include <chrono>
#include <iostream>
#include <thread>

// dependency
#include <iocshDeclWrapper.h>

// this module
#include "iocinfo.hpp"
#include "iocinfoCurl.hpp"

iocInfo::iocInfo(int arg, const char *name)
    : running(true),
      url("localhost:1516/iocinfo/"),
      thread(*this, name, epicsThreadGetStackSize(epicsThreadStackSmall), 50) {
  std::cout << "===>>> iocInfo construtor" << std::endl;
  data = std::make_shared<IocInfoData::Data>();
}

iocInfo::~iocInfo() {
  std::cout << "<<<=== iocInfo destructor" << std::endl;
  running = false;
  thread.exitWait();
}

void iocInfo::setVerbose(bool v) {
  if (v) {
    std::cout << "iocInfo verbose is set to true" << std::endl;
  }
  verbose = v;
}

void iocInfo::run() {
  auto start = std::chrono::steady_clock::now();
  bool postData = true;

  while (running) {
    if (postData) {
      if (verbose) std::cout << "posting data to: " << url << std::endl;
      postJson(*data->payload, url.c_str());
      postData = false;
    }
    auto now = std::chrono::steady_clock::now();
    // TODO: make delay adjustable
    if (now - start < std::chrono::seconds(10)) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    } else {
      start = now;
      postData = true;
    }
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

/**
 * @brief iocsh function to enable/disable verbose output
 * @param[in] verbose 0: off, 1: on
 */
void iocInfoVerbose(int verbose) { et2->setVerbose((bool)verbose); }

IOCSH_FUNC_WRAP_REGISTRAR(myRegistrar, IOCSH_FUNC_WRAP(iocinfo, "url(string)");
                          IOCSH_FUNC_WRAP(iocInfoVerbose, "on|off (1|0) (int)");
                          // IOCSH_FUNC_WRAP(startPostThread, "url(string)");
                          // IOCSH_FUNC_WRAP_QUIET(stopPostThread);
                          /* more functions may be registered here */
)