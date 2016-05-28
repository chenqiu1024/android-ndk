//
//  Thread.h
//  TestCyTrackableStateNode
//
//  Created by FutureBoy on 8/23/15.
//  Copyright (c) 2015 Cyllenge. All rights reserved.
//

#ifndef TestCyTrackableStateNode_Thread_h
#define TestCyTrackableStateNode_Thread_h

#include "TargetConditionals.h"

#if  defined(TARGET_OS_IOS)
#define getCurrentThreadID getCurrentThreadID_Apple
#elif TARGET_PLATFORM_OSX
#define getCurrentThreadID getCurrentThreadID_Apple
#elif  TARGET_PLATFORM_LINUX
#define getCurrentThreadID getCurrentThreadID_Linux
#endif

#ifdef __cplusplus
extern "C" {
#endif

    long getCurrentThreadID_Apple();
    long getCurrentThreadID_Linux();

#ifdef __cplusplus
}
#endif

#endif
