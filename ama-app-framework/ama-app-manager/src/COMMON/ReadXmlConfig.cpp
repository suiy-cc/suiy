/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include <unordered_map>
#include <mutex>
#include <fstream>

#include "ReadXmlConfig.h"
#include "AMGRLogContext.h"
#include "cereal/access.hpp"
#include "cereal/cereal.hpp"
#include "cereal/archives/xml.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/map.hpp"
#include "cereal/types/unordered_map.hpp"
#include "log.h"

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

static const std::string HS7_FRAMEWORK_CONIFG_PATH = "/etc/ama.d/frameworkConfig.xml";

typedef std::string Key;
typedef std::string SubKey;
typedef std::string Value;

class FrameworkConfig{
public:
    FrameworkConfig();
    FrameworkConfig(const std::string& path);
    FrameworkConfig(const std::unordered_map<Key, Value>& strConfig,
                    const std::unordered_map<Key, std::vector<Value>>& vecConfig,
                    const std::unordered_map<Key, std::map<SubKey, Value>>& mapConfig,
                    const std::unordered_map<Key, std::multimap<SubKey, Value>>& mmapConfig,
                    const bool isLoaded,
                    const std::string& path);
    virtual ~FrameworkConfig();

    RXC_Error Load();
    void Save();
    bool IsLoaded()const;
    void SetPath(const std::string& path);
    void SetPath(std::string&& path);

    bool IsExisting(const Key& key)const;
    const Value& GetValue(const Key& key)const;
    const std::vector<Value>& GetVec(const Key& key)const;
    const std::map<SubKey, Value>& GetMap(const Key& key)const;
    const std::multimap<SubKey, Value>& GetMMap(const Key& key)const;

protected:
    // serialization support
    friend class cereal::access;
    template <class Archive>
    void save( Archive & sereal ) const
    {
        sereal(cereal::make_nvp("string", strConfig)
               , cereal::make_nvp("vector", vecConfig)
               , cereal::make_nvp("map", mapConfig)
               , cereal::make_nvp("multi-map", mmapConfig)
              );
    }
    template <class Archive>
    void load( Archive & sereal )
    {
        sereal(cereal::make_nvp("string", strConfig)
               , cereal::make_nvp("vector", vecConfig)
               , cereal::make_nvp("map", mapConfig)
               , cereal::make_nvp("multi-map", mmapConfig)
              );

        isLoaded = true;
    }

private:
    std::unordered_map<Key, Value> strConfig;
    std::unordered_map<Key, std::vector<Value>> vecConfig;
    std::unordered_map<Key, std::map<SubKey, Value>> mapConfig;
    std::unordered_map<Key, std::multimap<SubKey, Value>> mmapConfig;
    bool isLoaded;
    std::string path;
};

FrameworkConfig::FrameworkConfig()
: isLoaded(false)
{

}

FrameworkConfig::FrameworkConfig(const std::string &path)
: path(path)
, isLoaded(false)
{

}

FrameworkConfig::FrameworkConfig(const std::unordered_map<Key, Value>& strConfig,
                                 const std::unordered_map<Key, std::vector<Value>>& vecConfig,
                                 const std::unordered_map<Key, std::map<SubKey, Value>>& mapConfig,
                                 const std::unordered_map<Key, std::multimap<SubKey, Value>>& mmapConfig,
                                 const bool isLoaded,
                                 const std::string& path)
: strConfig(strConfig)
, vecConfig(vecConfig)
, mapConfig(mapConfig)
, mmapConfig(mmapConfig)
, isLoaded(isLoaded)
, path(path)
{

}

FrameworkConfig::~FrameworkConfig()
{

}

static FrameworkConfig configGen = {
    {
        {"IsRearAlwaysON", "0"},
        {"RearTimerToCloseLCD", "10"},
        {"KeyApp", "com.hs7.radio.scr1"},
    },
    {
        {
            "AlwaysOnApp",
            {
                "com.hs7.home.scr1",
                "com.hs7.home.scr2",
                "com.hs7.home.scr3",
                "com.hs7.titlebar.scr1",
                "com.hs7.titlebar.scr2",
                "com.hs7.titlebar.scr3",
                "com.hs7.bt.scr1",
                "com.hs7.media-carplay.scr1",
                "com.hs7.media-carlife.scr1",
            }
        },
        {
            "AwakeAppList",
            {
                "com.hs7.bt.scr1",
                "com.hs7.aircondition.scr1",
                "com.hs7.tbox.scr1",
                "com.hs7.camera.scr1",
                "com.hs7.avm.scr1",
                "com.hs7.media-carplay.scr1",
                "com.hs7.vr.scr1",
                "com.hs7.vehsetting.service",
            }
        },
        {
            "ScreenNumber", {"0", "1"}
        },
        {
            "DisabledApps", {"com.hs7.navi.scr1"}
        },
    },
    {
        {
            "LogID",
            {
                {"home", "0x05"},
                {"titlebar", "0x0F"},
                {"popup", "0x0C"},
                {"ime", "0x00"},
                {"camera", "0x27"},
                {"radar", "0x05"},
                {"tbox", "0x05"},
                {"animation-player", "0x05"},
                {"media-service", "0x05"},
                {"boot-animation", "0x05"},
                {"background", "0x05"},
                {"gesture-recognizer", "0x05"},
                {"avm", "0x05"},
            },
        },
    },
    {
        {
            "EOLName2ItemID",
            {
                {"MPI", "com.hs7.media-carlife.scr1"},
                {"MPI", "com.hs7.media-carplay.scr1"},
                {"MPI", "com.hs7.media-ipod.scr1"},
                {"MPI", "com.hs7.welink.scr1"},

                {"RI_AVS", "com.hs7.avm.scr1"},

                {"AP", "com.hs7.park-in.scr1"},
                {"AP", "com.hs7.park-out.scr1"},

                {"TBOX", "com.hs7.tbox.scr1"},

                {"BROW", "com.hs7.browser.scr1"},
            }
        },
    },
    false,
    "/tmp/configGen.xml",
};

RXC_Error FrameworkConfig::Load()
{
    //configGen.Save();
    try
    {
        std::ifstream iFile(path.c_str());
        if(iFile.is_open())
        {
            {
                cereal::XMLInputArchive deserialize(iFile);
                deserialize(cereal::make_nvp("FrameworkConfig", *this));
            }
            iFile.close();
        }
        else
            return Error_Read_XML_File_Failed;
    }
    catch(cereal::Exception& ce)
    {
        logError(PLC, LOG_HEAD, "exception: ", (&ce)->what());
        return Error_Load_XML_File_Failed;
    }
    catch(std::exception& e)
    {
        logError(PLC, LOG_HEAD, "exception: ", (&e)->what());
        return Error_Read_XML_File_Failed;
    }
    catch(...)
    {
        logError(PLC, LOG_HEAD, "unexpected exception!");
        return Error_Read_XML_File_Failed;
    }
    return Error_None;
}

void FrameworkConfig::Save()
{
    try
    {
        std::ofstream oFile(path.c_str());
        if(oFile.is_open())
        {
            {
                cereal::XMLOutputArchive serialize(oFile);
                serialize(cereal::make_nvp("FrameworkConfig", *this));
            }
            oFile.close();
        }
    }
    catch(cereal::Exception& ce)
    {
        logError(PLC, LOG_HEAD, "exception: ", (&ce)->what());
    }
    catch(std::exception& e)
    {
        logError(PLC, LOG_HEAD, "exception: ", (&e)->what());
    }
    catch(...)
    {
        logError(PLC, LOG_HEAD, "unexpected exception!");
    }
}

bool FrameworkConfig::IsLoaded() const
{
    return isLoaded;
}

void FrameworkConfig::SetPath(const std::string &path)
{
    this->path = path;
}

void FrameworkConfig::SetPath(std::string &&path)
{
    this->path = std::move(path);
}

bool FrameworkConfig::IsExisting(const Key &key) const
{
    return strConfig.find(key)!=strConfig.end()
        || vecConfig.find(key)!=vecConfig.end()
        || mapConfig.find(key)!=mapConfig.end()
        || mmapConfig.find(key)!=mmapConfig.end()
        ;
}

const Value& FrameworkConfig::GetValue(const Key &key) const
{
    return strConfig.at(key);
}

const std::vector<Value> &FrameworkConfig::GetVec(const Key &key) const
{
    return vecConfig.at(key);
}
const std::map<SubKey, Value> &FrameworkConfig::GetMap(const Key &key) const
{
    return mapConfig.at(key);
}
const std::multimap<SubKey, Value> &FrameworkConfig::GetMMap(const Key &key) const
{
    return mmapConfig.at(key);
}

static FrameworkConfig frameworkConfig(HS7_FRAMEWORK_CONIFG_PATH);
static std::mutex mutex4Config;

RXC_Error GetConfigFromMMap(const std::string& configKey, XMLConfigMMap& dataMMap)
{
    std::unique_lock<std::mutex> lock(mutex4Config);

    // initialize
    if (!frameworkConfig.IsLoaded())
    {
        auto err = frameworkConfig.Load();
        if (Error_None!=err)
            return err;
    }

    //can't find the ConfigKey
    if(!frameworkConfig.IsExisting(configKey))
        return Error_No_This_Config_Key;

    dataMMap = frameworkConfig.GetMMap(configKey);

    return Error_None;
}

RXC_Error GetConfigFromMap(const std::string& configKey, XMLConfigMap& dataMap)
{
    std::unique_lock<std::mutex> lock(mutex4Config);

    // initialize
    if (!frameworkConfig.IsLoaded())
    {
        auto err = frameworkConfig.Load();
        if (Error_None!=err)
            return err;
    }

    //can't find the ConfigKey
    if(!frameworkConfig.IsExisting(configKey))
        return Error_No_This_Config_Key;

    dataMap = frameworkConfig.GetMap(configKey);

    return Error_None;
}

RXC_Error GetConfigFromVector(const std::string& configKey, std::vector<std::string>& dataVector)
{
    std::unique_lock<std::mutex> lock(mutex4Config);

    // initialize
    if (!frameworkConfig.IsLoaded())
    {
        auto err = frameworkConfig.Load();
        if (Error_None!=err)
            return err;
    }

    //can't find the ConfigKey
    if(!frameworkConfig.IsExisting(configKey))
        return Error_No_This_Config_Key;

    dataVector = frameworkConfig.GetVec(configKey);

    return Error_None;
}

RXC_Error GetConfigFromString(const std::string& configKey, std::string& dataString )
{
    std::unique_lock<std::mutex> lock(mutex4Config);

    // initialize
    if (!frameworkConfig.IsLoaded())
    {
        auto err = frameworkConfig.Load();
        if (Error_None!=err)
            return err;
    }

    //can't find the ConfigKey
    if(!frameworkConfig.IsExisting(configKey))
        return Error_No_This_Config_Key;

    dataString = frameworkConfig.GetValue(configKey);

    return Error_None;
}
