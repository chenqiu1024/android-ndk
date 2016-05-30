package com.android.madv.glrenderer;

//import android.util.SizeF;

import android.opengl.GLES20;

/**
 * Created by qiudong on 16/5/30.
 */
public class MadvGLRenderer {
    public static class SizeF {
        public SizeF(float width, float height) {
            this.width = width;
            this.height = height;
        }

        public float width;
        public float height;
    }

    static {
        System.loadLibrary("gles3jni");
    }

    protected void finalize() {
        releaseNativeGLRenderer(nativeGLRendererPointer);
    }

    public MadvGLRenderer(String lutPath) {
        nativeGLRendererPointer = createNativeGLRenderer(lutPath);
        ///!!!For Debug
        setSourceTextures(false, 0,1, new SizeF(1920,1440), new SizeF(1920,1080), GLES20.GL_TEXTURE_2D, true);
    }

    public native long createNativeGLRenderer(String lutPath);
    public native void releaseNativeGLRenderer(long pointer);

    public native void setSourceTextures(boolean separateSourceTexture, int srcTextureL, int srcTextureR, SizeF srcTextureSizeL, SizeF srcTextureSizeR, int srcTextureTarget, boolean isYUVColorSpace);

    public native void draw(int x, int y, int width, int height);

    public native void setEulerAngles(float yawRadius, float pitchRadius);

    public native void setQuaternion(float x, float y, float z, float w);

    public native  int getDisplayMode();
    public native  void setDisplayMode(int displayMode);

    public native  boolean getIsYUVColorSpace();
    public native  void setIsYUVColorSpace(boolean isYUVColorSpace);

    public native  float getFocalLength();
    public native  void setFocalLength(float focalLength);

    public native  float getYawRadius();
    public native  void setYawRadius(float yawRadius);

    public native  float getPitchRadius();
    public native  void setPitchRadius(float pitchRadius);

    public native  float getBankRadius();
    public native  void setBankRadius(float bankRadius);

    private long nativeGLRendererPointer = 0;
}
