/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef APP
#define APP

#include <string>

#include "cereal/access.hpp"
#include "cereal/cereal.hpp"

#include "log.h"

class App
{
public :
    App();
    App(const App& app);
    App(App&& app);
    ~App();

    void operator=(App&& app);
    void operator=(const App& app);

    unsigned int getAMPID() const;
    void setAMPID(unsigned int amaPid);

    std::string getItemID() const;
    void setItemID(const std::string& itemID);

    std::string getAppPath() const;
    void setAppPath(const std::string& appPath);

    std::string getAppName() const;
    void setAppName(const std::string& appName);

    std::string getBigIcon() const;
    void setBigIcon(const std::string& bIcon);

    std::string getSmallIcon() const;
    void setSmallIcon(const std::string& sIcon);

    std::string getSystemdFile() const;
    void setSystemdFile(const std::string& systemdFile);

    // not used for now
//    std::string getPkgID() ;
//    void setPkgID(const std::string& pkgID);

    std::string getAppType() const;
    void setAppType(const std::string& appType);

    bool getIsAutoStart() const;
    void setIsAutoStart(bool isAutoStart);

    bool getDisableStartupAnimation() const;
    void setDisableStartupAnimation(bool disable);

    std::string getTarget()const;
    void setTarget(const std::string& target);

protected:
    // serialization support
    friend class cereal::access;
    template <class Archive>
    void save( Archive & sereal ) const
    {
        sereal(cereal::make_nvp("AMPID", ToHEX(m_amaPid))
               , cereal::make_nvp("itemID", m_itemID)
               , cereal::make_nvp("path", m_appPath)
               , cereal::make_nvp("label", m_appName)
               , cereal::make_nvp("bigIcon", m_bIcon)
               , cereal::make_nvp("smallIcon", m_sIcon)
               , cereal::make_nvp("systemdFile", m_systemdFile)
               , cereal::make_nvp("itemType", m_appType)
               , cereal::make_nvp("isAutoStart", m_isAutoStart)
               , cereal::make_nvp("isThereLoadingScreen", m_isStartupAnimationDisabled)
               , cereal::make_nvp("target", m_target)
              );
    }
    template <class Archive>
    void load( Archive & sereal )
    {
        std::string ampid;
        sereal(cereal::make_nvp("AMPID", ampid)
               , cereal::make_nvp("itemID", m_itemID)
               , cereal::make_nvp("path", m_appPath)
               , cereal::make_nvp("label", m_appName)
               , cereal::make_nvp("bigIcon", m_bIcon)
               , cereal::make_nvp("smallIcon", m_sIcon)
               , cereal::make_nvp("systemdFile", m_systemdFile)
               , cereal::make_nvp("itemType", m_appType)
               , cereal::make_nvp("isAutoStart", m_isAutoStart)
               , cereal::make_nvp("isThereLoadingScreen", m_isStartupAnimationDisabled)
               , cereal::make_nvp("target", m_target)
              );

        m_amaPid = stoul(ampid, 0, 0);
    }

private:
    unsigned int m_amaPid;
    std::string m_itemID;
    std::string m_appPath;
    std::string m_appName;
    //std::string m_pkgID;
    std::string m_bIcon;
    std::string m_sIcon;
    std::string m_systemdFile;
    std::string m_appType;
    bool m_isAutoStart;
    bool m_isStartupAnimationDisabled;
    std::string m_target;
};


#endif // APP
