#ifndef __IOCINFO_HPP
#define __IOCINFO_HPP

#include <epicsThread.h>

#include <atomic>
#include <memory>
#include <string>

#include "iocinfoData.hpp"

class iocInfo : public epicsThreadRunable {
 public:
  iocInfo(int arg, const char *name);
  virtual ~iocInfo();
  virtual void run();
  
  void inline setUrl(const std::string URL) { url = URL; };
  void inline setUrl(const char *URL) { url = URL; };
  void inline setPostDelay(const size_t delay) { postDelay = delay;};

  void setVerbose(bool verbose);
  // bool running; // old school
  std::shared_ptr<IocInfoData::Data> data;
  std::atomic<bool> running;
  std::string url;
  epicsThread thread;
  bool verbose = false;

 private:
  size_t postDelay = 60;
};

#endif  // __IOCINFO_HPP