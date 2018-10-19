/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "ScreenTemperature.h"

ScreenTemperature::ScreenTemperature(const char* temperatureDevPath)
{
    logVerboseF(LCDM,"ScreenTemperature::ScreenTemperature()--->IN temperatureDevPath: %s",temperatureDevPath);
    if (NULL==temperatureDevPath)
    {
        logErrorF(LCDM,"ScreenTemperature::ScreenTemperature() temperatureDevPath IS NULL!");
        logVerboseF(LCDM,"ScreenTemperature::ScreenTemperature()--->OUT ");
        return ;
    }
    else
    {
        memcpy(m_temperatureDevPath,temperatureDevPath,strlen(temperatureDevPath));
        //strcpy(m_temperatureDevPath,temperatureDevPath);
        init();
    }
    logVerboseF(LCDM,"ScreenTemperature::ScreenTemperature()--->OUT ");
}

ScreenTemperature::~ScreenTemperature()
{
    logVerbose(LCDM,"ScreenTemperature::GetTemperatureDevPath()--->IN ");
    deInit();
    logVerbose(LCDM,"ScreenTemperature::GetTemperatureDevPath()--->OUT ");
}
void ScreenTemperature::deInit()
{
    logVerbose(LCDM,"ScreenTemperature::deInit()--->IN ");
    if (temperatureFile!=0)
        close(temperatureFile);
    logVerbose(LCDM,"ScreenTemperature::deInit()--->OUT ");
}
char *ScreenTemperature::GetTemperatureDevPath()
{
    logVerbose(LCDM,"ScreenTemperature::GetTemperatureDevPath()--->IN ");
    return m_temperatureDevPath;
    logVerbose(LCDM,"ScreenTemperature::GetTemperatureDevPath()--->OUT ");
}

void ScreenTemperature::SetTemperatureDevPath(char *temperatureDevPath)
{
    logVerbose(LCDM,"ScreenTemperature::SetTemperatureDevPath()--->IN ");
    if (NULL!=temperatureDevPath)
    {
        memcpy(m_temperatureDevPath,temperatureDevPath,strlen(temperatureDevPath));
    }
    else
    {
        logErrorF(LCDM,"ScreenTemperature::SetTemperatureDevPath temperatureDevPath  IS NULL!");
    }
    logVerbose(LCDM,"ScreenTemperature::SetTemperatureDevPath()--->OUT ");
}
void ScreenTemperature::init()
{
    logVerbose(LCDM,"ScreenTemperature::Init()--->IN ");


    logVerbose(LCDM,"ScreenTemperature::Init()--->OUT ");
}
int ScreenTemperature::GetTemperature()
{
   // logVerboseF(LCDM,"ScreenTemperature::GetTemperature()---> IN %s",m_temperatureDevPath);
    if (NULL==m_temperatureDevPath)
    {
        logErrorF(LCDM,"ScreenTemperature::GetTemperature invalid dev path ! %s",m_temperatureDevPath);
        logVerbose(LCDM,"ScreenTemperature::GetTemperature()--->OUT ");
        return -1;
    }
    try {
        temperatureFile = open(m_temperatureDevPath, O_RDONLY);
    }
    catch (std::string & e) {
        logError(LCDM, "open dev  ERROR: ", e);
    }
    if(temperatureFile<=0)
    {
        // logErrorF(LCDM,"open  %s  device error!",m_temperatureDevPath );
        // logErrorF(LCDM,"Message : %s", strerror(errno));
        close(temperatureFile);

    }
    else
    {
        //logInfoF(LCDM,"open  %s device success!",m_temperatureDevPath);

    }
    char buff [TMPTURE_FILE_BUFFER]={0};

    int remaimData= read(temperatureFile,&buff,TMPTURE_FILE_BUFFER);
    int iTemperature=0;
    //convert to int
    try {
        iTemperature=atol(buff);
    }
    catch (std::string & e) {
        logError(LCDM, "convert Temperature to int ERROR: ", e);
    }
    close(temperatureFile);
    // logVerboseF(LCDM,"ScreenTemperature::GetTemperature()--->OUT iTemperature:%d",iTemperature);
    return iTemperature;
}
