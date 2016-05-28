//
//  GLCamera.cpp
//  Madv360_v1
//
//  Created by QiuDong on 16/4/27.
//  Copyright © 2016年 Cyllenge. All rights reserved.
//

#include "GLCamera.h"

GLCamera::~GLCamera() {
    
}

GLCamera::GLCamera()
: _width(640)
, _height(960)
, _near(CLIP_Z_NEAR)
, _far(CLIP_Z_FAR)
, _yawRadius(0)
, _pitchRadius(0)
, _bankRadius(0)
, _quaternion({0,0,0,1})
{
    
}

void GLCamera::setProjectionMatrix(GLint projectionMatrixSlot) {
    kmMat4 projection;
    float scale = (float)REFERENCE_VIEWPORT_WIDTH / (float)_width;
    float zNear = _near * scale;
    float zFar = _far * scale;
    float h = CLIP_WIDTH * _height / _width;
    float fovY = kmRadiansToDegrees(atanf(h/2 / zNear)) * 2;
    float aspect = (float)_width/(float)_height;
    kmMat4PerspectiveProjection(&projection, fovY, aspect, zNear, zFar);
    glUniformMatrix4fv(projectionMatrixSlot, 1, 0, projection.mat);
}

void GLCamera::setCameraMatrix(GLint cameraMatrixSlot) {
    kmMat4 cameraMatrix;
    kmMat4Identity(&cameraMatrix);
    
    kmQuaternion pitchQuaternion;
    kmVec3 pitchAxis = {-1,0,0};
    kmQuaternionRotationAxis(&pitchQuaternion, &pitchAxis, _pitchRadius);
    
    kmMat4 yawMatrix;
    kmMat4Identity(&yawMatrix);
    kmMat4RotationQuaternionBy(&yawMatrix, &_quaternion);
    kmMat4RotationXBy(&yawMatrix, M_PI/2);
    
    kmVec3 yawAxis = {0, -1, 0};
    kmVec3Transform(&yawAxis, &yawAxis, &yawMatrix);
    
    kmQuaternion yawQuaternion, pitchBaseQuaternion;
    kmQuaternionRotationAxis(&yawQuaternion, &yawAxis, _yawRadius);
    
    kmVec3 baseAxis = {-1,0,0};
    kmQuaternionRotationAxis(&pitchBaseQuaternion, &baseAxis, M_PI/2);
    
    kmQuaternion bankQuaternion;
    kmVec3 bankAxis = {0,0,1};
    kmQuaternionRotationAxis(&bankQuaternion, &bankAxis, _bankRadius);//Clock-Wise
    
    kmQuaternion productQuaternion = pitchBaseQuaternion;
    kmQuaternionMultiply(&productQuaternion, &productQuaternion, &_quaternion);
    kmQuaternionMultiply(&productQuaternion, &productQuaternion, &yawQuaternion);
    kmQuaternionMultiply(&productQuaternion, &productQuaternion, &bankQuaternion);//Notice: Rotation Order Matters!!
    kmQuaternionMultiply(&productQuaternion, &productQuaternion, &pitchQuaternion);
    
    kmMat4RotationQuaternionBy(&cameraMatrix, &productQuaternion);
    
    glUniformMatrix4fv(cameraMatrixSlot, 1, 0, cameraMatrix.mat);
}
