#ifndef IOCINFO_DATA_H
#define IOCINFO_DATA_H

#include <string>
#include <map>
#include <iostream>
#include <memory>
#include "nlohmann/json.hpp"
#include "require.h"

namespace IocInfoData
{
    class Data
    {
    public:
        /**
         * @brief Constructor
         */
        Data();

        /**
         * @brief Destructor
         */
        ~Data();

        /**
         * @brief payload
        */
        std::shared_ptr<nlohmann::json> payload;

        /**
         * @brief print bootInfo
         */
        inline void printBootInfo() { std::cout << bootInfo.dump(1) << std::endl; };

        /**
         * @brief get loaded libraries
         */
        inline void getLoadedLibraries() { libVersionToJSON(loadedLibraries); };

        /**
         * @brief print loadedLibraries
         */
        inline void printLoadedLibraries() { std::cout << loadedLibraries.dump(1) << std::endl; }

        /**
         * @brief marshal payload
         */
        nlohmann::json marshalPayload();

    private:
        /**
         * @brief static boot information: loaded libraries, OS, EPICS version, etc.
         */
        nlohmann::json bootInfo;

        /**
         * @brief loaded libraries: name, version, path.
         */
        nlohmann::json loadedLibraries;

        /**
         * @brief traverses the iocEpicsEnv map
         * @param[in] j json object to write the data to
         * @param[in] m map to traverse. JSON_key : ENVIRONMENT_VARIABLE_NAME
         */
        void envMapToJson(nlohmann::json &j, const std::map<std::string, std::string> &m);

        /**
         * @brief fetch information of loaded libraries from require and marshal them into json
         * @param[in] loadedLibs json object to write to
         */
        inline void libVersionToJSON(nlohmann::json &loadedLibs) { foreachLoadedLib(Data::myLibHandler, &loadedLibs); };

        /**
         * @brief handler function for `foreachLoadedLib` to call
         * @details calls the require function `foreachLoadedLib`
         */
        static size_t myLibHandler(const char *name, const char *version, const char *path, void *arg);

    protected:
        std::map<std::string, std::string> iocEpicsEnv{
            {"ioc", "IOC"},
            {"hostname", "HOSTNAME"},
            {"ipAddress", "IP_ADDR"},
            {"broadcastAddress", "BC_ADDR"},
            {"macAddress", "MAC_ADDR"},
            {"procServ", "PROCSERV_PORT"},
            {"epicsVersion", "EPICS_RELEASE"},
            {"epicsHostArchitecture", "EPICS_HOST_ARCH"},
            {"engineer", "ENGINEER"},
            {"location", "LOCATION"},
            {"bootDirectory", "STARTUP"},
            {"startupScript", "ST_CMD"},
            {"platform", "PLATFORM"},
        };
    };
}
#endif // IOCINFO_DATA_H