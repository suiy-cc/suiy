/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef LASTUSERCTX
#define LASTUSERCTX

#include <mutex>
#include <map>
#include "ama_types.h"

typedef enum {
    LUC_IVI_AUDIO,
    LUC_SRCL_AUDIO,
    LUC_SRCR_AUDIO,
    LUC_IVI_FOREGROUND,
    LUC_IVI_BACKGROUND,
    LUC_SRCL_FOREGROUND,
    LUC_SRCL_BACKGROUND,
    LUC_SRCR_FOREGROUND,
    LUC_SRCR_BACKGROUND,
    LUC_KEY_MAX
}LUC_KEY;



class LastUserCtx
{
public :
    LastUserCtx();
    ~LastUserCtx();
    void init();

    void recoverForeground( );
    void recoverBackground( );
    void recoverAudio();
    void setAudio();
    void setForeground(AMPID ampid);
    void setBackground(AMPID ampid);
    void removeBackground(AMPID ampid);


    static LastUserCtx *GetInstance();
private:
    std::string ivi_background="";
    std::string scrL_background="";
    std::string scrR_background="";
    std::string lucKeyArr [LUC_KEY_MAX]={ "IVI_AUDIO",
                                          "SRCL_AUDIO",
                                          "SRCR_AUDIO",
                                          "IVI_FOREGROUND",
                                          "IVI_BACKGROUND",
                                          "SRCL_FOREGROUND",
                                          "SRCL_BACKGROUND",
                                          "SRCR_FOREGROUND",
                                          "SRCR_BACKGROUND"};

    std::string readLUC(std::string key);

    int writeLUC(std::string key ,std::string value);
};



#endif // LASTUSERCTX

