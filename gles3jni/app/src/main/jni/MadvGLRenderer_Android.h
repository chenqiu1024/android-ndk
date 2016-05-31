//
// Created by QiuDong on 16/5/31.
//

#ifndef GLES3JNI_MADVGLRENDERER_ANDROID_H
#define GLES3JNI_MADVGLRENDERER_ANDROID_H

#include "MadvGLRenderer.h"

class MadvGLRenderer_Android : public MadvGLRenderer {
public:

    virtual ~MadvGLRenderer_Android();

    MadvGLRenderer_Android(const char* lutPath);

//    static const char* renderThumbnail(const char* thumbnailPath, CGSize destSize);

    void prepareLUT(const char* lutPath);

protected:

    void prepareTextureWithRenderSource(void* renderSource);
};


#endif //GLES3JNI_MADVGLRENDERER_ANDROID_H
