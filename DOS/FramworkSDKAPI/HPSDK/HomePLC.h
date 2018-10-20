/// @file HPAppPLC.h
/// @brief contains High-priorty AppPLC APIs
///
/// Created by zs on 2016/8/16.
/// this file contains the declaration of High-priorty AppPLC APIs
///

#ifndef HOMEPLC_H
#define HOMEPLC_H

#include "AppPLC.h"
#include "AppPKG.h"

#include <vector>

class HomePLC
: public AppPLC
{
public:
    HomePLC();
    virtual ~HomePLC();

protected:
    friend class HomePLCImplementation;
    virtual void OnInstalledPKGsChanged()=0;
private:
    virtual void *GetImplementation();

};

// app operation
void StartApp(unsigned int app, int startArg);
void StopApp(unsigned int app);
void HideApp(unsigned int app);

// package operation
std::vector<AppPKG> GetAppList();
AppPKG GetAppPKGInfo(unsigned int);

#endif // HOMEPLC_H