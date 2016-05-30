//
//  MadvGLRenderer.h
//  Madv360_v1
//
//  Created by QiuDong on 16/2/26.
//  Copyright © 2016年 Cyllenge. All rights reserved.
//

#include "OpenGLHelper.h"
#include "GLProgram.h"
#include "GLCamera.h"
#include "kazmath.h"
#include <pthread.h>

typedef enum {
    PanoramaDisplayModeSphere = 0x01,
    PanoramaDisplayModeLUT = 0x02,
    PanoramaDisplayModeLittlePlanet = 0x04,
    PanoramaDisplayModePlainStitch = 0x08,
} PanoramaDisplayMode;

extern const int DisplayModes[];
extern const int DisplayModesCount;
extern const char* DisplayModeString[];

#pragma mark    GLSL Shaders

class MadvGLProgram : public GLProgram {
public:

    MadvGLProgram(const GLchar* const* vertexSources, int vertexSourcesCount, const GLchar* const* fragmentSources, int fragmentSourcesCount);

    inline GLint getLeftTextureSlot() {return _leftTextureSlot;}
    inline GLint getRightTextureSlot() {return _rightTextureSlot;}

    inline GLint getLeftTexcoordSlot() {return _leftTexcoordSlot;}
    inline GLint getRightTexcoordSlot() {return _rightTexcoordSlot;}
    //    inline GLint getLxLUTSlot() {return _lxLUTSlot;}
    //    inline GLint getLyLUTSlot() {return _lyLUTSlot;}
    //    inline GLint getRxLUTSlot() {return _rxLUTSlot;}
    //    inline GLint getRyLUTSlot() {return _ryLUTSlot;}
    inline GLint getDstSizeSlot() {return _dstSizeSlot;}
    inline GLint getLeftSrcSizeSlot() {return _leftSrcSizeSlot;}
    inline GLint getRightSrcSizeSlot() {return _rightSrcSizeSlot;}

    inline GLint getScaleSlot() {return _scaleSlot;}
    inline GLint getAspectSlot() {return _aspectSlot;}
    inline GLint getTransformSlot() {return _transformSlot;}

    inline GLint getLeftYTextureSlot() {return _yLeftTextureSlot;}
    inline GLint getLeftUTextureSlot() {return _uLeftTextureSlot;}
    inline GLint getLeftVTextureSlot() {return _vLeftTextureSlot;}
    inline GLint getRightYTextureSlot() {return _yRightTextureSlot;}
    inline GLint getRightUTextureSlot() {return _uRightTextureSlot;}
    inline GLint getRightVTextureSlot() {return _vRightTextureSlot;}

    inline GLint getColorSpaceYUVSlot() {return _colorspaceYUVSlot;}
    inline GLint getDrawGridSphereSlot() {return _drawGridSphereSlot;}
    inline GLint getConvertWithLUTSlot() {return _convertWithLUTSlot;}
    inline GLint getPlainStitchSlot() {return _plainStitchSlot;}
    inline GLint getLittlePlanetSlot() {return _littlePlanetSlot;}
    inline GLint getSeparateSourceTexturesSlot() {return _separateSourceTexturesSlot;}
    inline GLint getAntiAliasSlot() {return _antiAliasSlot;}

    inline GLint getFlipXSlot() {return _flipXSlot;}
    inline GLint getFlipYSlot() {return _flipYSlot;}

protected:
    /*
     //#define COLORSPACE_YUV
     uniform int COLORSPACE_YUV;
     
     //#define DRAW_GRID_SPHERE
     uniform int DRAW_GRID_SPHERE;
     
     //#define CONVERT_WITH_LUT
     uniform int CONVERT_WITH_LUT;
     
     //#define LITTLE_PLANET
     uniform int LITTLE_PLANET;
     
     //#define SEPARATE_SOURCE_TEXTURES
     uniform int SEPARATE_SOURCE_TEXTURES;
     
     //#define ANTI_ALIAS
     uniform int ANTI_ALIAS;
     */
    GLint _colorspaceYUVSlot;
    GLint _drawGridSphereSlot;
    GLint _convertWithLUTSlot;
    GLint _plainStitchSlot;
    GLint _littlePlanetSlot;
    GLint _separateSourceTexturesSlot;
    GLint _antiAliasSlot;

    GLint _flipXSlot;
    GLint _flipYSlot;

    GLint _leftTextureSlot;
    GLint _rightTextureSlot;

    GLint _leftTexcoordSlot;
    GLint _rightTexcoordSlot;

    //    GLint _lxLUTSlot;
    //    GLint _lyLUTSlot;
    //    GLint _rxLUTSlot;
    //    GLint _ryLUTSlot;
    GLint _dstSizeSlot;
    GLint _leftSrcSizeSlot;
    GLint _rightSrcSizeSlot;

    GLint _scaleSlot;
    GLint _aspectSlot;
    GLint _transformSlot;

    GLint _yLeftTextureSlot;
    GLint _uLeftTextureSlot;
    GLint _vLeftTextureSlot;
    GLint _yRightTextureSlot;
    GLint _uRightTextureSlot;
    GLint _vRightTextureSlot;
};

typedef AutoRef<MadvGLProgram> MadvGLProgramRef;

class MadvGLRenderer {
public:

    virtual ~MadvGLRenderer();

    MadvGLRenderer(const char* lutPath);

    void setEulerAngles(GLfloat yawRadius, GLfloat pitchRadius);

    void setQuaternion(float x, float y, float z, float w);

    void setRenderSource(void* renderSource);

    void setLUTData(Vec2f lutDstSize, Vec2f leftSrcSize,Vec2f rightSrcSize, int dataSizeInShort, const GLushort* lxIntData, const GLushort* lxMinData, const GLushort* lyIntData, const GLushort* lyMinData, const GLushort* rxIntData, const GLushort* rxMinData, const GLushort* ryIntData, const GLushort* ryMinData);

    void draw(GLint x, GLint y, GLint width, GLint height);

    inline int getDisplayMode() {return _currentDisplayMode;}
    inline void setDisplayMode(int displayMode) {_currentDisplayMode = displayMode;}

    inline bool getIsYUVColorSpace() {return _isYUVColorSpace;}
    inline void setIsYUVColorSpace(bool isYUVColorSpace) {_isYUVColorSpace = isYUVColorSpace;}

    inline GLfloat getFocalLength() {return _glCamera->getZNear();}
    inline void setFocalLength(GLfloat focalLength) {_glCamera->setZNear(focalLength);}

    inline GLfloat getYawRadius() {return _glCamera->getYawRadius();}
    inline void setYawRadius(GLfloat yawRadius) {_glCamera->setYawRadius(yawRadius);}

    inline GLfloat getPitchRadius() {return _glCamera->getPitchRadius();}
    inline void setPitchRadius(GLfloat pitchRadius) {_glCamera->setPitchRadius(pitchRadius);}

    inline GLfloat getBankRadius() {return _glCamera->getBankRadius();}
    inline void setBankRadius(GLfloat bankRadius) {_glCamera->setBankRadius(bankRadius);}

    inline Vec2f getTouchPoint() {return _touchPoint;}
    inline void setTouchPoint(Vec2f touchPoint) {_touchPoint = touchPoint;}

    virtual void prepareLUT(const char* lutPath) = 0;

protected:

    virtual void prepareTextureWithRenderSource(void* renderSource) = 0;

    void setSourceTextures(bool separateSourceTexture, GLint srcTextureL, GLint srcTextureR, Vec2f srcTextureSizeL, Vec2f srcTextureSizeR, GLenum srcTextureTarget, bool isYUVColorSpace);

    void prepareGLPrograms();

    void prepareGLCanvas(GLint x, GLint y, GLint width, GLint height);
    void prepareVAO();

    void setGLProgramVariables(GLint x, GLint y, GLint width, GLint height);
    void drawPrimitives();

private:

    void* _renderSource;
    bool _needRenderNewSource;

#ifdef USE_MSAA
    GLuint _msaaFramebuffer;
    GLuint _msaaRenderbuffer;
    GLuint _msaaDepthbuffer;
    
    bool   _supportDiscardFramebuffer;
#endif
    GLuint _srcTextureL;
    GLuint _srcTextureR;
    GLenum _srcTextureTarget;
    bool   _separateSourceTexture;

    Vec2f _lutDstSize;
    Vec2f _lutSrcSizeL, _lutSrcSizeR;

    GLuint _yuvTextures[3];

    MadvGLProgramRef _currentGLProgram;

#ifdef DRAW_GRID_SPHERE
    GLint _uniGridColors;
    GLint _uniLongitudeFragments;
    GLint _uniLatitudeFragments;
#endif

    Vec2f _touchPoint;

    GLCameraRef _glCamera;

    Mesh3D _quadMesh;
    Mesh3D _lutQuadMesh;
    Mesh3D _sphereMesh;
    Mesh3D _lutSphereMesh;

    Mesh3D* _pCurrentMesh;

    bool _isYUVColorSpace;

    int _currentDisplayMode;

    pthread_mutex_t _mutex;
};

typedef AutoRef<MadvGLRenderer> MadvGLRendererRef;
