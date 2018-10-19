/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef BPMCTRL_H
#define BPMCTRL_H


#include <mutex>

#include "AMGRLogContext.h"
#include "ama_types.h"
#include "BPMTask.h"
#include "BPManager.h"

using std::shared_ptr;
using std::string;
using std::map;


class BPMCtrl
{
public:

    BPMCtrl();
    ~BPMCtrl();
    static BPMCtrl* GetInstance();
    void Init(void);
    void doTask(shared_ptr<Task> task);
};

#endif //BPMCTRL_H
