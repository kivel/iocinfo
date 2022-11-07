#include "iocinfoData.h"

#include <iostream>

namespace IocInfoData
{
    Data::Data()
    {
        std::cout << "===>>> IocInfoData <<<" << std::endl;
        envMapToJson(bootInfo, iocEpicsEnv);
        getLoadedLibraries();
        payload = std::make_shared<nlohmann::json>(marshalPayload());
        // std::cout << marshalPayload().dump(1) << std::endl;
    }

    Data::~Data()
    {
        std::cout << "<<<=== IocInfoData destroyed <<<" << std::endl;
    }

    void Data::envMapToJson(nlohmann::json &j, const std::map<std::string, std::string> &m)
    {
        // C++11 for now as we have to support ancient compilers
        for (const auto &n : m)
        // for (const auto &n : Data::iocEpicsEnv)
        {
            // n.first is the key, n.second is the value
            if (auto env_p = std::getenv(n.second.c_str()))
            {
                j[n.first.c_str()] = env_p;
            }
        }

        /*
        // C++17
        for (const auto &[key, value] : m)
        {
            if (auto env_p = std::getenv(value.c_str()))
            {
                j[key] = env_p;
            }
        }
        */
    }

    size_t Data::myLibHandler(const char *name, const char *version, const char *path, void *arg)
    {
        // return 1 to exit caller loop, see `require::foreachLoadedLib` function for details
        if (!name)
        {
            return 1;
        }
        // cast arg void pointer into json object
        nlohmann::json *libraries = (static_cast<nlohmann::json *>(arg));

        nlohmann::json lib;
        lib["name"] = name;
        lib["version"] = version;
        lib["path"] = path;

        libraries->push_back(lib);
        return 0;
    }

    nlohmann::json Data::marshalPayload()
    {
        nlohmann::json payload;
        for (auto it : loadedLibraries)
        {
            nlohmann::json tmp = bootInfo;
            tmp["lib"] = it;
            payload.push_back(tmp);
        }
        return payload;
    }
}
