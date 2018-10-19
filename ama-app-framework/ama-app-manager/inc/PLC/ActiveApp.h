/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef ACTIVEAPP_H
#define ACTIVEAPP_H

#include <map>

#include "ama_types.h"
#include "PLCTask.h"
#include "AppPKGInfo.h"
#include "UIProcess.h"

using std::shared_ptr;

class ActiveApp
: public UIProcess
{
public:
    /// @name constructors & destructors
    /// @{
    ActiveApp():Process(INVALID_AMPID, std::make_shared<InitialState>()){};
    ActiveApp(AMPID app):Process(app, std::make_shared<InitialState>()), UIProcess(app){};
    virtual ~ActiveApp(){};
    /// @}

    /// @name notifies
    /// @{
    void InitializeNotify(bool isVisible);
    void SwitchTitlebarVisibilityNotify(bool isShow, int style);
    /// @}

    /// @name getters & setters
    /// @{
    shared_ptr<UIProcess> GetTitlebar() const;
    /// @}
};

#endif // ACTIVEAPP_H
