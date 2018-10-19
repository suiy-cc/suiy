/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "pers_config.h"

#include <regex>
#include <vector>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>

#include <tinyxml2.h>
#include "log.h"

using namespace tinyxml2;
extern LOG_DECLARE_CONTEXT(CS);

void splitString(std::string s, char splitchar, std::vector<std::string> & vec)
{
	if (vec.size() > 0) {
   		vec.clear();
    }
	int length = s.length();
	int start = 0;
	for (int i = 0; i < length; i++) {
   		if (s[i] == splitchar && i == 0) {
    		start += 1;
   		} else if (s[i] == splitchar) {
    		vec.push_back(s.substr(start, i - start));
    		start = i + 1;
   		} else if (i == length-1) {
    		vec.push_back(s.substr(start, i + 1 - start));
   		}
	}
}

bool operator< (const PersVersion & lhs, const PersVersion & rhs)
{
	if (lhs.Major < rhs.Major) {
		return true;
	} else if (lhs.Major == rhs.Major){
		if (lhs.Minor < rhs.Minor) {
			return true;
		} else if (lhs.Minor == rhs.Minor) {
			if (lhs.Revision < rhs.Revision) {
				return true;
			}
		}
	}
	return false;
}

bool PersConfig::installPersFiles()
{
    logVerbose(CS, "PersConfig::installPersFiles()-->IN");
    loadFilenames();
    loadConfigItems();

    for (auto filename : allFilePaths_) {
		PersConfigItem pci = extractFilenameInfo(filename);
		if (pci.Name.empty()) continue;
		bool isCfgUpdate = false;

        auto iter = persConfigItems_.find(pci.Name);
		persInstaller_.setPath(filename);
        if (persConfigItems_.end() != iter) {
            PersVersion pv = persConfigItems_[pci.Name];
            if (pv < pci.PersVer)
				isCfgUpdate = persInstaller_.install();
        } else {
			isCfgUpdate = persInstaller_.install();
        }
		if (isCfgUpdate)
			persConfigItems_[pci.Name] = pci.PersVer;
    }

	logInfo(CS, " Pers Config Item Start");
    for (auto val : persConfigItems_) {
		logInfo(CS, "Config Item : ", val.first, " ", val.second.Major, " ", val.second.Minor, " ", val.second.Revision);
	}
	logInfo(CS, " Pers Config Item End");
    updateConfig();
    return true;
}

PersConfig::PersConfig(std::string path, std::string filePath) : persConfigPath_(path), persFilesPath_(filePath)
{

}

void PersConfig::updateConfig()
{
	try
	{
        FILE* fp = NULL;
        fp = fopen(persConfigPath_.c_str(),"w+");   // create empty file
        fclose(fp);
        XMLDocument doc;
	    XMLDeclaration * decl = doc.NewDeclaration();
	    XMLElement * persConfigElem = doc.NewElement("pers_config");
	    for (auto val : persConfigItems_) {
	        std::string name = val.first;
	        std::string version = std::string("") + std::to_string(val.second.Major) + '.' + std::to_string(val.second.Minor) + '.'
	            + std::to_string(val.second.Revision);
	        XMLElement * itemElem = doc.NewElement("item");
	        XMLElement * nameElem = doc.NewElement("name");
	        XMLElement * currVerElem = doc.NewElement("current_version");
	        nameElem->LinkEndChild(doc.NewText(name.c_str()));
	        currVerElem->LinkEndChild(doc.NewText(version.c_str()));
	        itemElem->LinkEndChild(nameElem);
	        itemElem->LinkEndChild(currVerElem);
	        persConfigElem->LinkEndChild(itemElem);
	    }
	    doc.LinkEndChild(decl);
	    doc.LinkEndChild(persConfigElem);
	    doc.SaveFile(persConfigPath_.c_str());
	} catch (std::string & e) {
		logError(CS, "Write Version Number Error ! ", e);
	}
}

PersConfigItem PersConfig::extractFilenameInfo(std::string filename)
{
    PersVersion pv;
    PersConfigItem pci;
    std::regex r("com.neusoft.hs7.(srcX|src1|src2|src3).([A-Za-z]+)-([0-9]+).([0-9]+).([0-9]+)");
    std::smatch m;
    bool result = std::regex_search (filename, m, r);
    if (result) {
        pv.Major = std::stoi(m[3]);
        pv.Minor = std::stoi(m[4]);
        pv.Revision = std::stoi(m[5]);
		std::string prefix = "com.neusoft.hs7.";
		std::string mid = m[1];
		std::string suffix = m[2];
		pci.Name = prefix + mid + "." + suffix;
    } else {
        pv.Major = 0;
        pv.Minor = 0;
        pv.Revision = 0;
        pci.Name = "";
    }
    pci.PersVer = pv;
    return pci;
}

void PersConfig::loadFilenames()
{
    logVerbose(CS, "PersConfig::loadFilenames()-->IN");
    DIR * dir;
    struct dirent * ent;
    int i = 0;
    char childpath[512];
    memset(childpath,0,sizeof(childpath));
    dir = opendir(persFilesPath_.c_str());
    while((ent = readdir(dir)) != NULL) {
        if(ent->d_type & DT_DIR) {
            if(strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0)
                continue;
        } else {
            std::string filename = ent->d_name;
            std::regex r("com.neusoft.hs7.(srcX|src1|src2|src3).([A-Za-z]+)-([0-9]+).([0-9]+).([0-9]+)");
            std::smatch m;

            bool res = std::regex_search (filename, m, r);
			if (!res) continue;

            std::string fappname = m[2];
            bool isModify = false;
            try
            {
                int fmajor = std::stoi(m[3]);
                int fminor = std::stoi(m[4]);
                int frevision = std::stoi(m[5]);

                for (int i = 0; i < allFilePaths_.size(); ++i) {
                    bool res = std::regex_search (allFilePaths_[i], m, r);
                    if (!res)  continue;
                    std::string appname = m[2];
                    int major = std::stoi(m[3]);
                    int minor = std::stoi(m[4]);
                    int revision = std::stoi(m[5]);
                    if (appname == fappname) {
                        if (fmajor > major || (fmajor == major && fminor > minor)
                            	|| (fmajor == major && fminor == minor && frevision > revision)) {
                            allFilePaths_[i] = persFilesPath_ + '/' + filename;
                            isModify = true;
                        } else {
							isModify = true;
						}
                    }
                }
            } catch (...) {
                logError(CS, "file version error");
            }
            if (!isModify) {
                allFilePaths_.emplace_back(persFilesPath_ + '/' + ent->d_name);
            }
        }
    }
	logInfo(CS, " All File Start");
    for (auto val : allFilePaths_) {
		logInfo(CS, "File : ", val);
    }
	logInfo(CS, " All File End");
    logVerbose(CS, "PersConfig::loadFilenames()-->OUT");
}

void PersConfig::loadConfigItems()
{
    logVerbose(CS, "PersConfig::loadConfigItems()-->IN");
    PersVersion pv;
    try
    {
        XMLDocument doc;
        doc.LoadFile(persConfigPath_.c_str());
        XMLElement * rootElement = doc.RootElement();
        XMLElement * itemElem = rootElement->FirstChildElement();
        for (; itemElem != NULL; itemElem = itemElem->NextSiblingElement()) {
            XMLElement * nameElem = itemElem->FirstChildElement();
            std::string name = nameElem->GetText();
            XMLElement * currVerElem = nameElem->NextSiblingElement();
            std::string currVerStr = currVerElem->GetText();
            std::vector<std::string> v;
            splitString(currVerStr, '.', v);
            pv.Major = std::stoi(v.at(0));
            pv.Minor = std::stoi(v.at(1));
            pv.Revision = std::stoi(v.at(2));
            persConfigItems_.insert(std::make_pair(name, pv));
        }
    }
    catch (std::string & e)
    {
        logError(CS, "Read Version Number Error ! ", e);
        // pv.Major = 0;
        // pv.Minor = 0;
        // pv.Revision = 0;
        // return pv;
    }
    logVerbose(CS, "PersConfig::loadConfigItems()-->OUT");
}
