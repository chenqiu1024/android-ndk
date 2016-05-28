//
// Created by QiuDong on 16/5/28.
//

#ifndef GLES3JNI_GLES2STUB_H
#define GLES3JNI_GLES2STUB_H

#include "TargetConditionals.h"

#ifdef TARGET_PLATFORM_LINUX
#include <GLES2/gl2.h>
#elif TARGET_PLATFORM_IOS
#include <OpenGLES/ES2/gl.h>
#endif

#endif //GLES3JNI_GLES2STUB_H
