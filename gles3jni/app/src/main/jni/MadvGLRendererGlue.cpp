//
// Created by QiuDong on 16/5/30.
//
#include "MadvGLRendererGlue.h"
#include "MadvGLRenderer.h"

JNIEXPORT jlong JNICALL Java_com_android_madv_glrenderer_MadvGLRenderer_createNativeGLRenderer
        (JNIEnv* env, jobject self, jstring lutPath) {
    jboolean copied = false;
    const char* cstrLUTPath = env->GetStringUTFChars(lutPath, &copied);
    ALOGE("LUT Path : isCopy = %d, %s", copied, cstrLUTPath);

    env->ReleaseStringUTFChars(lutPath, cstrLUTPath);
    return 0;
}

JNIEXPORT void JNICALL Java_com_android_madv_glrenderer_MadvGLRenderer_releaseNativeGLRenderer
                (JNIEnv *, jobject, jlong) {
}

JNIEXPORT void JNICALL Java_com_android_madv_glrenderer_MadvGLRenderer_setSourceTextures
(JNIEnv* env, jobject self, jboolean separateSourceTexture, jint srcTextureL, jint srcTextureR, jobject srcTextureSizeL, jobject srcTextureSizeR, jint srcTextureTarget, jboolean isYUVColorSpace) {
    jclass clazz_SizeF = env->GetObjectClass(srcTextureSizeL);
    jfieldID id_width = env->GetFieldID(clazz_SizeF, "width", "F");
    jfieldID id_height = env->GetFieldID(clazz_SizeF, "height", "F");
    jfloat width = env->GetFloatField(srcTextureSizeL, id_width);
    jfloat height = env->GetFloatField(srcTextureSizeR, id_height);

    ALOGE("Java_com_android_madv_glrenderer_MadvGLRenderer_setSourceTextures : %f, %f", width,height);
}

JNIEXPORT void JNICALL Java_com_android_madv_glrenderer_MadvGLRenderer_draw
(JNIEnv *, jobject, jint, jint, jint, jint) {

}

JNIEXPORT void JNICALL Java_com_android_madv_glrenderer_MadvGLRenderer_setEulerAngles
(JNIEnv *, jobject, jfloat, jfloat) {

}

JNIEXPORT void JNICALL Java_com_android_madv_glrenderer_MadvGLRenderer_setQuaternion
(JNIEnv *, jobject, jfloat, jfloat, jfloat, jfloat) {

}

JNIEXPORT jint JNICALL Java_com_android_madv_glrenderer_MadvGLRenderer_getDisplayMode
        (JNIEnv *, jobject) {
    return 0;
}

JNIEXPORT void JNICALL Java_com_android_madv_glrenderer_MadvGLRenderer_setDisplayMode
               (JNIEnv *, jobject, jint) {

}

JNIEXPORT jboolean JNICALL Java_com_android_madv_glrenderer_MadvGLRenderer_getIsYUVColorSpace
        (JNIEnv *, jobject) {
    return 0;
}

JNIEXPORT void JNICALL Java_com_android_madv_glrenderer_MadvGLRenderer_setIsYUVColorSpace
                   (JNIEnv *, jobject, jboolean) {

}

JNIEXPORT jfloat JNICALL Java_com_android_madv_glrenderer_MadvGLRenderer_getFocalLength
        (JNIEnv *, jobject) {
    return 0;
}

JNIEXPORT void JNICALL Java_com_android_madv_glrenderer_MadvGLRenderer_setFocalLength
                 (JNIEnv *, jobject, jfloat) {

}


JNIEXPORT jfloat JNICALL Java_com_android_madv_glrenderer_MadvGLRenderer_getYawRadius
        (JNIEnv *, jobject) {
    return 0;
}

JNIEXPORT void JNICALL Java_com_android_madv_glrenderer_MadvGLRenderer_setYawRadius
                 (JNIEnv *, jobject, jfloat) {

}

JNIEXPORT jfloat JNICALL Java_com_android_madv_glrenderer_MadvGLRenderer_getPitchRadius
        (JNIEnv *, jobject) {
    return 0;
}

JNIEXPORT void JNICALL Java_com_android_madv_glrenderer_MadvGLRenderer_setPitchRadius
                 (JNIEnv *, jobject, jfloat) {

}

JNIEXPORT jfloat JNICALL Java_com_android_madv_glrenderer_MadvGLRenderer_getBankRadius
        (JNIEnv *, jobject) {
    return 0;
}

JNIEXPORT void JNICALL Java_com_android_madv_glrenderer_MadvGLRenderer_setBankRadius
                 (JNIEnv *, jobject, jfloat) {

}