//
//  GLRenderTexture.cpp
//  Madv360_v1
//
//  Created by FutureBoy on 4/2/16.
//  Copyright © 2016 Cyllenge. All rights reserved.
//

#include "GLRenderTexture.h"
#include <stdlib.h>

GLRenderTexture::~GLRenderTexture() {
    if (_texture) glDeleteTextures(1, &_texture);
    if (_framebuffer) glDeleteFramebuffers(1, &_framebuffer);
}

GLRenderTexture::GLRenderTexture(GLint width, GLint height)
: _framebuffer(0), _texture(0)
{
    _width = width;
    _height = height;
    
    glGenFramebuffers(1, &_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    
    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//GL_CLAMP_TO_EDGE);//
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//GL_CLAMP_TO_EDGE);//
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
//    GLubyte* pixelData = (GLubyte*) malloc(destSize.width * destSize.height * 4);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);
//    GLenum error = glGetError();
//    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//    NSLog(@"error = %x, status = %d", error, status);
}

GLubyte* GLRenderTexture::copyPixelData() {
    GLubyte* pixelData = (GLubyte*) malloc(_width * _height * 4);
    glReadPixels(0, 0, _width, _height, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
    return pixelData;
}
