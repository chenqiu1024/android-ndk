//
// Created by QiuDong on 16/5/28.
//

#ifndef GLES3JNI_GLES2EXT_H
#define GLES3JNI_GLES2EXT_H

#include "TargetConditionals.h"

#ifdef TARGET_PLATFORM_LINUX
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2ext.h>
#elif TARGET_OS_IOS
#include <OpenGLES/ES2/glext.h>
#endif

#endif //GLES3JNI_GLES2EXT_H
