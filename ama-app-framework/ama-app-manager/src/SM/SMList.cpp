/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "SMList.h"

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

using namespace tinyxml2;

bool appNoteOperate::ReadXmlFile(std::string& szFileName, std::list<appNoteStruct> *listXml)
{
	logVerbose(SM,"appNoteOperate::ReadXmlFile::",szFileName.c_str(),"-->>IN");

	struct appNoteStruct s_xml;
	FILE * fp=fopen(szFileName.c_str(),"r");
	if(NULL==fp)
	{
		logInfo(SM,"appNoteOperate::ReadXmlFile() OPEN",szFileName.c_str(),"FAILED!  return false");
		return false;//
	}

	try
	{
		std::string fullPath = szFileName;

		XMLDocument *myDocument =new XMLDocument();
		XMLError xmlerror = myDocument->LoadFile(fullPath.c_str());
		if (xmlerror != XML_SUCCESS)
		{
			logError(SM,"appNoteOperate::ReadXmlFile--XMLDocument::LoadFile:", szFileName.c_str(), ", error:",xmlerror,"  return false");
			delete(myDocument);
			myDocument=NULL;
			fclose(fp);
		    fp = NULL;
			return false;
		}
		else
			logInfo(SM,"appNoteOperate::ReadXmlFile--XMLDocument::LoadFile:",szFileName.c_str()," end");

		XMLElement *RootElement = myDocument->RootElement();
		if (RootElement == NULL)
		{
			logError(SM,"appNoteOperate::ReadXmlFile--get RootElement null  return false");
			delete(myDocument);
			myDocument=NULL;
			fclose(fp);
		    fp = NULL;
			return false;
		}

		XMLElement* pProviderElement = RootElement->FirstChildElement("provider");
		if (pProviderElement == NULL)
		{
			for ( ; RootElement != NULL; RootElement = RootElement->NextSiblingElement())
			{
				XMLElement * body = RootElement->FirstChildElement();

				for ( ; body != NULL; body = body->NextSiblingElement())
				{
					XMLElement *FirstElement  =  body->FirstChildElement();
					XMLElement *SecondElement = FirstElement->NextSiblingElement();
					XMLElement *ThirdElement  =  SecondElement->NextSiblingElement();

					s_xml.value1 = FirstElement->FirstChild()->Value();
					s_xml.value2 = SecondElement->FirstChild()->Value();
					s_xml.value3 = "unknown";//ThirdElement->FirstChild()->Value();

					logVerbose(SM,"type1: value1-->>",s_xml.value1.c_str());
					logVerbose(SM,"type1: value2-->>",s_xml.value2.c_str());
					logVerbose(SM,"type1: value3-->>",s_xml.value3.c_str());

					listXml->push_back(s_xml);
				}
			}
		}
		else
		{
			//logVerbose(SM,"type 2 xml");

			for ( ; RootElement != NULL; RootElement = RootElement->NextSiblingElement("body"))
			{
				XMLElement* pProviderElement = RootElement->FirstChildElement("provider");
				if (pProviderElement == NULL)
				{
					logWarn(SM,"appNoteOperate::ReadXmlFile--no provider");
					continue;
				}

				s_xml.value1 = pProviderElement->GetText();

				for (XMLElement* pKey = RootElement->FirstChildElement("key"); pKey != NULL; pKey = pKey->NextSiblingElement("key"))
				{
					if (pKey->Value() != "key")

					s_xml.value2 = pKey->GetText();
					const char* pAttr =  pKey->Attribute("d_value");
					if (pAttr == NULL)
					{
						logWarn(SM,"appNoteOperate::ReadXmlFile--Attribute d_value null, key:", pKey->GetText());
						continue;
					}
					s_xml.value3 = "unknown";//pAttr;

					logVerbose(SM,"type2: value1-->>",s_xml.value1.c_str());
					logVerbose(SM,"type2: value2-->>",s_xml.value2.c_str());
					logVerbose(SM,"type2: value3-->>",s_xml.value3.c_str());

					listXml->push_back(s_xml);
				}
			}
		}

		delete(myDocument);
		myDocument=NULL;
	}
	catch(std::string& e)
	{
		//delete(myDocument);
		//myDocument=NULL;
		fclose(fp);
		fp = NULL;
		logError(SM,"appNoteOperate::ReadXmlFile() READ",szFileName.c_str(),"FAILED!");
		return false;
	}
	//delete(myDocument);
	//myDocument=NULL;
	fclose(fp);
	fp = NULL;
	logVerbose(SM,"appNoteOperate::ReadXmlFile::",szFileName.c_str(),"-->>OUT");
	return true;
}
