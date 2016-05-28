//
//  GLCamera.hpp
//  Madv360_v1
//
//  Created by QiuDong on 16/4/27.
//  Copyright © 2016年 Cyllenge. All rights reserved.
//

#ifndef GLCamera_hpp
#define GLCamera_hpp

#include "gles2.h"

#include "AutoRef.h"
#include "kazmath/include/kazmath/kazmath.h"

#define CLIP_WIDTH    6
#define CLIP_Z_NEAR   3
#define CLIP_Z_FAR    65536

#define REFERENCE_VIEWPORT_WIDTH    375
#define REFERENCE_VIEWPORT_HEIGHT   667

class GLCamera {
public:
    
    virtual ~GLCamera();
    
    GLCamera();
    
    void setProjectionMatrix(GLint projectionMatrixSlot);
    void setCameraMatrix(GLint cameraMatrixSlot);
    
    inline void setProjectionFrustum(GLint width, GLint height, GLint zNear, GLint zFar) {
        _width = width;
        _height = height;
        _near = zNear;
        _far = zFar;
    }
    
    inline void setEulerAngles(GLfloat yaw, GLfloat pitch, GLfloat bank) {
        _yawRadius = yaw;
        _pitchRadius = pitch;
        _bankRadius = bank;
    }
    
    inline void setQuaternion(kmQuaternion quaternion) {_quaternion = quaternion;}
    inline kmQuaternion getQuaternion() {return _quaternion;}
    
    inline GLint getWidth() {return _width;}
    inline void setWidth(GLint width) {_width = width;}
    
    inline GLint getHeight() {return _height;}
    inline void setHeight(GLint height) {_height = height;}
    
    inline GLfloat getZNear() {return _near;}
    inline void setZNear(GLfloat zNear) {_near = zNear;}
    
    inline GLfloat getZFar() {return _far;}
    inline void setZFar(GLfloat zFar) {_far = zFar;}
    
    inline GLfloat getYawRadius() {return _yawRadius;}
    inline void setYawRadius(GLfloat yawRadius) {_yawRadius = yawRadius;}
    
    inline GLfloat getPitchRadius() {return _pitchRadius;}
    inline void setPitchRadius(GLfloat pitchRadius) {_pitchRadius = pitchRadius;}
    
    inline GLfloat getBankRadius() {return _bankRadius;}
    inline void setBankRadius(GLfloat bankRadius) {_bankRadius = bankRadius;}
    
private:
    
    GLfloat _yawRadius;
    GLfloat _pitchRadius;
    GLfloat _bankRadius;
    kmQuaternion _quaternion;
    
    GLint _width;
    GLint _height;
    GLint _near;
    GLint _far;
};

typedef AutoRef<GLCamera> GLCameraRef;

#endif /* GLCamera_hpp */
