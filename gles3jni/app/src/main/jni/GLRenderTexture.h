//
//  GLRenderTexture.hpp
//  Madv360_v1
//
//  Created by FutureBoy on 4/2/16.
//  Copyright © 2016 Cyllenge. All rights reserved.
//

#ifndef GLRenderBuffer_hpp
#define GLRenderBuffer_hpp

#include "gles2.h"

class GLRenderTexture {
public:
    
    virtual ~GLRenderTexture();
    
    GLRenderTexture(GLint width, GLint height);
    
    inline GLuint getFramebuffer() {return _framebuffer;}
    inline GLuint getTexture() {return _texture;}
    
    GLubyte* copyPixelData();
    
private:
    
    GLuint _framebuffer;
    GLuint _texture;
    
    GLint _width;
    GLint _height;
};

#endif /* GLRenderBuffer_hpp */
