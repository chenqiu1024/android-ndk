//
//  OpenGLHelper.c
//  Madv360
//
//  Created by FutureBoy on 11/5/15.
//  Copyright © 2015 Cyllenge. All rights reserved.
//

#include "OpenGLHelper.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

unsigned long nextPOT(unsigned long x)
{
    x = x - 1;
    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >>16);
    return x + 1;
}

GLint compileShader(const GLchar* const* shaderSources, int sourcesCount, GLenum type) {
    GLint shader = glCreateShader(type);
    glShaderSource(shader, sourcesCount, shaderSources, NULL);
    glCompileShader(shader);
    
    GLint compileSuccess;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSuccess);
    if (compileSuccess == GL_FALSE) {
        GLchar messages[256];
        glGetShaderInfoLog(shader, sizeof(messages), 0, &messages[0]);
        printf("%s\n", messages);
        ///!!!exit(1);
    }
    
    return shader;
}

GLint compileAndLinkShaderProgram(const GLchar* const* vertexSources, int vertexSourcesCount,
                                  const GLchar* const* fragmentSources, int fragmentSourcesCount,
                                  GLint* pVertexShader, GLint* pFragmentShader) {
    return compileAndLinkShaderProgramWithShaderPointers(vertexSources, vertexSourcesCount, fragmentSources, fragmentSourcesCount, NULL,NULL);
}

GLint compileAndLinkShaderProgramWithShaderPointers(const GLchar* const* vertexSources, int vertexSourcesCount,
                                  const GLchar* const* fragmentSources, int fragmentSourcesCount,
                                  GLint* pVertexShader, GLint* pFragmentShader) {
    GLint vertexShader, fragmentShader;
    if (!pVertexShader)
    {
        pVertexShader = &vertexShader;
    }
    if (!pFragmentShader)
    {
        pFragmentShader = &fragmentShader;
    }
    *pVertexShader = vertexShader = compileShader(vertexSources, vertexSourcesCount, GL_VERTEX_SHADER);
    *pFragmentShader = fragmentShader = compileShader(fragmentSources, fragmentSourcesCount, GL_FRAGMENT_SHADER);
    
    GLint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    GLint linkSuccess;
    glGetProgramiv(program, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE) {
        GLchar messages[256];
        glGetProgramInfoLog(program, sizeof(messages), 0, &messages[0]);
        printf("%s\n", messages);
        ///!!!exit(1);
    }
    
    return program;
}

void createOrUpdateTexture(GLuint* pTextureID, GLint width, GLint height, GLubyte** pTextureData, GLsizei* pTextureDataSize, void(*dataSetter)(GLubyte* data, GLint pow2Width, GLint pow2Height, void* userData), void* userData)
{
    GLsizei pow2Width = (GLsizei) width;///nextPOT(width);
    GLsizei pow2Height = (GLsizei) height;///nextPOT(height);
    
    GLubyte* textureData = NULL;
    if (NULL == pTextureData)
    {
        pTextureData = &textureData;
    }
    GLsizei textureDataSize = 0;
    if (NULL == pTextureDataSize)
    {
        pTextureDataSize = &textureDataSize;
    }
    
    if (0 == *pTextureID)
    {
        glGenTextures(1, pTextureID);
        glBindTexture(GL_TEXTURE_2D, *pTextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//GL_CLAMP_TO_EDGE);//
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//GL_CLAMP_TO_EDGE);//
        
        if (NULL == *pTextureData)
        {
            *pTextureDataSize = pow2Width * pow2Height * 4;
            *pTextureData = (GLubyte*) malloc(*pTextureDataSize);
        }
        else if (*pTextureDataSize < pow2Height*pow2Width*4)
        {
            free(*pTextureData);
            *pTextureDataSize = pow2Width * pow2Height * 4;
            *pTextureData = (GLubyte*) malloc(*pTextureDataSize);
        }
        
        if (dataSetter)
        {
            dataSetter(*pTextureData, pow2Width, pow2Height, userData);
        }
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)pow2Width, (GLsizei)pow2Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, *pTextureData);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, *pTextureID);
        
        if (NULL == *pTextureData)
        {
            *pTextureDataSize = pow2Width * pow2Height * 4;
            *pTextureData = (GLubyte*) malloc(*pTextureDataSize);
        }
        else if (*pTextureDataSize < pow2Height*pow2Width*4)
        {
            free(*pTextureData);
            *pTextureDataSize = pow2Width * pow2Height * 4;
            *pTextureData = (GLubyte*) malloc(*pTextureDataSize);
        }
        
        if (dataSetter)
        {
            dataSetter(*pTextureData, pow2Width, pow2Height, userData);
        }
        
        //        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (GLsizei)pow2Width, (GLsizei)pow2Height, GL_RGBA, GL_UNSIGNED_BYTE, *pTextureData);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)pow2Width, (GLsizei)pow2Height, 0, GL_RGBA,GL_UNSIGNED_BYTE, *pTextureData);
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

//dispatch_queue_t sharedOpenGLQueue() {
//    static dispatch_once_t once;
//    static dispatch_queue_t glQueue;
//    dispatch_once(&once, ^{
//        glQueue = dispatch_queue_create("com.madv360.openGLESContextQueue", NULL);
//    });
//    return glQueue;
//}

P4C4T2f P4C4T2fMake(GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat r, GLfloat g, GLfloat b, GLfloat a, GLfloat s, GLfloat t) {
    P4C4T2f ret = {x,y,z,w, r,g,b,a, s,t};
    return ret;
}

void DrawablePrimitiveRelease(DrawablePrimitive primitive) {
    if (primitive.indices) free(primitive.indices);
}

void Mesh3DRelease(Mesh3D* mesh) {
    if (mesh->vertices) free(mesh->vertices);
    
    if (mesh->primitiveCount > 0)
    {
        for (int i=0; i<mesh->primitiveCount; ++i)
        {
            DrawablePrimitiveRelease(mesh->primitives[i]);
        }
        free(mesh->primitives);
    }
    
    if (mesh->_indexBuffers)
    {
        glDeleteBuffers(mesh->primitiveCount, mesh->_indexBuffers);
        free(mesh->_indexBuffers);
        glDeleteBuffers(1, &mesh->_vertexBuffer);
        glDeleteVertexArraysOES(1, &mesh->_vao);
        mesh->_vao = 0;
    }
}

Mesh3D Mesh3DCreate(int vertexCount, int primitiveCount) {
    Mesh3D mesh;
    mesh.vertexCount = vertexCount;
    mesh.vertices = (P4C4T2f*) malloc(sizeof(P4C4T2f) * mesh.vertexCount);
    mesh.primitiveCount = primitiveCount;
    mesh.primitives = (DrawablePrimitive*) malloc(sizeof(DrawablePrimitive) * mesh.primitiveCount);
    
    mesh._indexBuffers = NULL;
    return mesh;
}

void Mesh3DPrepareVAO(Mesh3D* mesh) {
    if (mesh->_indexBuffers) return;
    
    GLint prevVAO;
    GLint prevElementBuffer;
    GLint prevBuffer;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING_OES, &prevVAO);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &prevElementBuffer);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prevBuffer);
    
    mesh->_indexBuffers = (GLuint*) malloc(sizeof(GLuint) * mesh->primitiveCount);
    
    mesh->_vao = 0;
    mesh->_vertexBuffer = 0;
    
    glGenVertexArraysOES(1, &mesh->_vao);
    glBindVertexArrayOES(mesh->_vao);
    
    glGenBuffers(1, &mesh->_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(P4C4T2f) * mesh->vertexCount, mesh->vertices, GL_STATIC_DRAW);
    
    glGenBuffers(mesh->primitiveCount, mesh->_indexBuffers);
    for (int i=0; i<mesh->primitiveCount; ++i)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->_indexBuffers[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * mesh->primitives[i].indexCount, mesh->primitives[i].indices, GL_STATIC_DRAW);
    }
    
    glBindVertexArrayOES(prevVAO);
    glBindBuffer(GL_ARRAY_BUFFER, prevBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, prevElementBuffer);
}

void Mesh3DDrawVAO(Mesh3D* mesh, int positionSlot, int colorSlot, int texcoordSlot) {
    GLint prevVAO;
    GLint prevElementBuffer;
    GLint prevBuffer;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING_OES, &prevVAO);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &prevElementBuffer);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prevBuffer);
    
    glBindVertexArrayOES(mesh->_vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->_vertexBuffer);
    
    // Enable the "aPosition" vertex attribute.
    if (positionSlot >= 0)
    {
        glEnableVertexAttribArray(positionSlot);
//        GlUtil.checkGlError("glEnableVertexAttribArray");
        glVertexAttribPointer(positionSlot, 4, GL_FLOAT, false, sizeof(GLfloat) * 10, 0);
    }
    if (colorSlot >= 0) {
        glEnableVertexAttribArray(colorSlot);
//        GlUtil.checkGlError("glEnableVertexAttribArray");
        glVertexAttribPointer(colorSlot, 4, GL_FLOAT, false, sizeof(GLfloat) * 10, (const GLvoid*) (sizeof(GLfloat) * 4));
    }
    if (texcoordSlot >= 0)
    {
        // Enable the "aTextureCoord" vertex attribute.
        glEnableVertexAttribArray(texcoordSlot);
//        GlUtil.checkGlError("glEnableVertexAttribArray");
        glVertexAttribPointer(texcoordSlot, 2, GL_FLOAT, false, sizeof(GLfloat) * 10, (const GLvoid*) (sizeof(GLfloat) * 8));
    }
    
    for (int i = 0; i < mesh->primitiveCount; ++i)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->_indexBuffers[i]);
        glDrawElements(mesh->primitives[i].type, mesh->primitives[i].indexCount, GL_UNSIGNED_SHORT, 0);
    }
//    GlUtil.checkGlError("glDrawElements");
    
    if (positionSlot >= 0) glDisableVertexAttribArray(positionSlot);
    if (colorSlot >= 0) glDisableVertexAttribArray(colorSlot);
    if (texcoordSlot >= 0) glDisableVertexAttribArray(texcoordSlot);
    
    glBindVertexArrayOES(prevVAO);
    glBindBuffer(GL_ARRAY_BUFFER, prevBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, prevElementBuffer);
}

void Mesh3DDrawMadvSphere(Mesh3D* mesh, int positionSlot, int leftTexcoordSlot, int rightTexcoordSlot, int dstTexcoordSlot) {
//  P4C4T2f: C0~1 LeftTexcoord, C2~3 RightTexcoord, T1 Weight
    GLint prevVAO;
    GLint prevElementBuffer;
    GLint prevBuffer;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING_OES, &prevVAO);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &prevElementBuffer);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prevBuffer);
    
    glBindVertexArrayOES(mesh->_vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->_vertexBuffer);
    
    // Enable the "aPosition" vertex attribute.
    if (positionSlot >= 0)
    {
        glEnableVertexAttribArray(positionSlot);
        //        GlUtil.checkGlError("glEnableVertexAttribArray");
        glVertexAttribPointer(positionSlot, 4, GL_FLOAT, false, sizeof(GLfloat) * 10, 0);
    }
    if (leftTexcoordSlot >= 0) {
        glEnableVertexAttribArray(leftTexcoordSlot);
        //        GlUtil.checkGlError("glEnableVertexAttribArray");
        glVertexAttribPointer(leftTexcoordSlot, 2, GL_FLOAT, false, sizeof(GLfloat) * 10, (const GLvoid*) (sizeof(GLfloat) * 4));
    }
    if (rightTexcoordSlot >= 0) {
        glEnableVertexAttribArray(rightTexcoordSlot);
        //        GlUtil.checkGlError("glEnableVertexAttribArray");
        glVertexAttribPointer(rightTexcoordSlot, 2, GL_FLOAT, false, sizeof(GLfloat) * 10, (const GLvoid*) (sizeof(GLfloat) * 6));
    }
    if (dstTexcoordSlot)
    {
        glEnableVertexAttribArray(dstTexcoordSlot);
        glVertexAttribPointer(dstTexcoordSlot, 2, GL_FLOAT, false, sizeof(GLfloat) * 10, (const GLvoid*) (sizeof(GLfloat) * 8));
    }
    
    for (int i = 0; i < mesh->primitiveCount; ++i)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->_indexBuffers[i]);
        glDrawElements(mesh->primitives[i].type, mesh->primitives[i].indexCount, GL_UNSIGNED_SHORT, 0);
    }
    //    GlUtil.checkGlError("glDrawElements");
    
    if (positionSlot >= 0) glDisableVertexAttribArray(positionSlot);
    if (leftTexcoordSlot >= 0) glDisableVertexAttribArray(leftTexcoordSlot);
    if (rightTexcoordSlot >= 0) glDisableVertexAttribArray(rightTexcoordSlot);
    if (dstTexcoordSlot >= 0) glDisableVertexAttribArray(dstTexcoordSlot);
    
    glBindVertexArrayOES(prevVAO);
    glBindBuffer(GL_ARRAY_BUFFER, prevBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, prevElementBuffer);
}

Mesh3D createSphere(GLfloat radius, int longitudeSegments, int latitudeSegments) {
    // 2 Polars, (longitudeSegments + 1) Longitude circles, (latitudeSegments - 1) Latitude circles:
    // 2 Polar fans, (latitudeSegments - 2) strips:
    Mesh3D mesh = Mesh3DCreate(2 + (longitudeSegments + 1) * (latitudeSegments - 1), latitudeSegments);
    // Vertices:
    mesh.vertices[mesh.vertexCount - 2] = P4C4T2fMake(0,radius,0,1, 0,1,0,1, 0.5,0);//North
    mesh.vertices[mesh.vertexCount - 1] = P4C4T2fMake(0,-radius,0,1, 0,1,0,1, 0.5,1);//South
    int iVertex = 0;
    for (int iLat=1; iLat<latitudeSegments; ++iLat)
    {
        GLfloat theta = M_PI * iLat / latitudeSegments;
        GLfloat y = radius * cos(theta);
        GLfloat xzRadius = radius * sin(theta);
        for (int iLon=0; iLon<=longitudeSegments; ++iLon)
        {
            GLfloat phi = 2*M_PI * iLon / longitudeSegments;
            GLfloat x = -xzRadius * sin(phi);
            GLfloat z = xzRadius * cos(phi);
            GLfloat s = (GLfloat)iLon / (GLfloat)longitudeSegments;
            GLfloat t = (GLfloat)iLat / (GLfloat)latitudeSegments;
            mesh.vertices[iVertex++] = P4C4T2fMake(x,y,z,1, 0,1,0,1, s,t);
        }
    }
    // Indices:
    // North&South polar fan:
    for (int i=0; i<2; i++)
    {
        mesh.primitives[i].type = GL_TRIANGLE_FAN;
        mesh.primitives[i].indexCount = longitudeSegments + 2;
        mesh.primitives[i].indices = (GLshort*) malloc(sizeof(GLshort) * (longitudeSegments + 2));
        mesh.primitives[i].indices[0] = mesh.vertexCount - 2 + i;
        GLshort* pDst = &mesh.primitives[i].indices[1];
        int index = (i == 0 ? 0 : mesh.vertexCount - 3 - longitudeSegments);
        for (int i=longitudeSegments; i>=0; --i) *pDst++ = index++;
    }
    // Strips parallel with latitude circles:
    for (int i=2; i<mesh.primitiveCount; ++i)
    {
        mesh.primitives[i].type = GL_TRIANGLE_STRIP;
        mesh.primitives[i].indexCount = 2 * (longitudeSegments + 1);
        mesh.primitives[i].indices = (GLshort*) malloc(sizeof(GLshort) * mesh.primitives[i].indexCount);
        GLshort* pDst = mesh.primitives[i].indices;
        GLshort index = (i - 2) * (longitudeSegments + 1);
        for (int j=longitudeSegments; j>=0; --j)
        {
            *pDst++ = index;
            *pDst++ = (index + longitudeSegments + 1);
            ++index;
        }
    }
    return mesh;
}

Mesh3D createGrids(GLfloat width, GLfloat height, int columns, int rows) {
    Mesh3D mesh = Mesh3DCreate((columns + 1) * (rows + 1), rows);
    int iVertex = 0;
    for (int iRow=0; iRow<=rows; ++iRow)
    {
        GLfloat y = height * iRow / rows - height / 2.f;
        GLfloat t = (GLfloat)iRow / (GLfloat)rows;
        for (int iCol=0; iCol<=columns; ++iCol)
        {
            GLfloat x = width * iCol / columns - width / 2.f;
            GLfloat z = 0;
            GLfloat s = (GLfloat)iCol / (GLfloat)columns;
            mesh.vertices[iVertex++] = P4C4T2fMake(x,y,z,1, 0,1,0,1, s,t);
        }
    }
    // Indices:
    // Strips parallel with latitude circles:
    GLshort index = 0;
    for (int i=0; i<mesh.primitiveCount; ++i)
    {
        mesh.primitives[i].type = GL_TRIANGLE_STRIP;
        mesh.primitives[i].indexCount = 2 * (columns + 1);
        mesh.primitives[i].indices = (GLshort*) malloc(sizeof(GLshort) * mesh.primitives[i].indexCount);
        GLshort* pDst = mesh.primitives[i].indices;
        for (int j=columns; j>=0; --j)
        {
            *pDst++ = index;
            *pDst++ = (index + columns + 1);
            ++index;
        }
    }
    return mesh;
}

Mesh3D createSphereV0(GLfloat radius, int longitudeSegments, int latitudeSegments) {
    // (longitudeSegments + 1) Longitude circles, (latitudeSegments + 1) Latitude circles:
    // (latitudeSegments) strips:
    Mesh3D mesh = Mesh3DCreate((longitudeSegments + 1) * (latitudeSegments + 1), latitudeSegments);
    // Vertices:
    int iVertex = 0;
    for (int iLat=0; iLat<=latitudeSegments; ++iLat)
    {
        GLfloat theta = M_PI * iLat / latitudeSegments;
        GLfloat y = radius * cos(theta);
        GLfloat t = (GLfloat)iLat / (GLfloat)latitudeSegments;
        GLfloat xzRadius = radius * sin(theta);
        for (int iLon=0; iLon<longitudeSegments; ++iLon)
        {
            GLfloat phi = 2*M_PI * iLon / longitudeSegments;
            GLfloat x = -xzRadius * sin(phi);
            GLfloat z = xzRadius * cos(phi);
            GLfloat s = (GLfloat)iLon / (GLfloat)longitudeSegments;
//            ///!!!For Debug:
//            s += 0.5f;
//            if (s > 1.0) s -= 1.0;
//            ///!!!:For Debug
            mesh.vertices[iVertex++] = P4C4T2fMake(x,y,z,1, 0,1,0,1, s,t);
        }
        /*/!!!For Debug:
        mesh.vertices[iVertex++] = P4C4T2fMake(0,y,xzRadius,1, 0,1,0,1, 0.5,t);
        /*/
        mesh.vertices[iVertex++] = P4C4T2fMake(0,y,xzRadius,1, 0,1,0,1, 1.0,t);
        //*/
    }
    // Indices:
    // Strips parallel with latitude circles:
    GLshort index = 0;
    for (int i=0; i<mesh.primitiveCount; ++i)
    {
        mesh.primitives[i].type = GL_TRIANGLE_STRIP;
        mesh.primitives[i].indexCount = 2 * (longitudeSegments + 1);
        mesh.primitives[i].indices = (GLshort*) malloc(sizeof(GLshort) * mesh.primitives[i].indexCount);
        GLshort* pDst = mesh.primitives[i].indices;
        for (int j=longitudeSegments; j>=0; --j)
        {
            *pDst++ = index;
            *pDst++ = (index + longitudeSegments + 1);
            ++index;
        }
    }
    return mesh;
}

Mesh3D createQuad(P4C4T2f v0, P4C4T2f v1, P4C4T2f v2, P4C4T2f v3) {
    Mesh3D quad = Mesh3DCreate(4, 1);
    quad.vertices[0] = v0;
    quad.vertices[1] = v1;
    quad.vertices[2] = v2;
    quad.vertices[3] = v3;
    
    quad.primitives[0].type = GL_TRIANGLE_STRIP;
    quad.primitives[0].indexCount = 4;
    quad.primitives[0].indices = (GLshort*) malloc(sizeof(GLshort) * 4);
    GLshort indices[] = {1,2,0,3};
    memcpy(quad.primitives[0].indices, indices, sizeof(indices));
    return quad;
}
