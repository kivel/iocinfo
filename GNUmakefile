DRIVER_MAKEFILE ?= /ioc/tools/driver.makefile
ifeq ($(wildcard ${DRIVER_MAKEFILE}),)
$(info If you are not using the PSI build environment, GNUmakefile can be removed.)
$(error Cannot find driver.makefile)
else
include ${DRIVER_MAKEFILE}
endif

MODULE=iocinfo

#To avoid clash with SynApps module don't use in EPICS 3
EXCLUDE_VERSIONS=3
BUILDCLASSES=Linux

# useful for looping over maps and make_unique
# USR_CXXFLAGS+=-std=c++17
USR_CXXFLAGS+=-std=c++11 -g
USR_LDFLAGS+=-lcurl

SOURCES += src/iocinfo.cpp
SOURCES += src/iocinfoData.cpp

DBDS += dbd/iocinfo.dbd
