//
//  OpenGLHelper.h
//  Madv360
//
//  Created by FutureBoy on 11/5/15.
//  Copyright © 2015 Cyllenge. All rights reserved.
//

#ifndef OpenGLHelper_h
#define OpenGLHelper_h

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#define CHECK_GL_ERROR() \
do { \
GLenum __error = glGetError(); \
if(__error) { \
printf("OpenGL error 0x%04X in %s %s %d\n", __error, __FILE__, __FUNCTION__, __LINE__); \
} \
} while (false)

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct Texcoord2fStruct {
        GLfloat s;
        GLfloat t;
    } Texcoord2f;
    
    typedef struct Size2fStruct {
        GLfloat width;
        GLfloat height;
    } Size2f;
    
    typedef struct Point2fStruct {
        GLfloat x;
        GLfloat y;
    } Point2f;
    
    typedef struct Vec2fUnion {
        union {GLfloat s; GLfloat width; GLfloat x;};
        union {GLfloat t; GLfloat height; GLfloat y;};
    } Vec2f;
    
#define Vec2fZero   Vec2f{0,0}
    
    typedef struct Color3fStruct {
        GLfloat r;
        GLfloat g;
        GLfloat b;
    } Color3f;
    
    typedef struct Vec3fStruct {
        GLfloat x;
        GLfloat y;
        GLfloat z;
    } Vec3f;
    
    typedef struct Color4fStruct {
        GLfloat r;
        GLfloat g;
        GLfloat b;
        GLfloat a;
    } Color4f;
    
    typedef struct Vec4fStruct {
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat w;
    } Vec4f;
    
    typedef struct P4C4T2fStruct {
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat w;
        GLfloat r;
        GLfloat g;
        GLfloat b;
        GLfloat a;
        GLfloat s;
        GLfloat t;
    } P4C4T2f;
    
    typedef struct DrawablePrimitiveStruct {
        GLshort* indices;
        GLsizei indexCount;
        GLenum type;
    } DrawablePrimitive;
    
    typedef struct Mesh3DStruct {
        P4C4T2f* vertices;
        GLsizei vertexCount;
        
        DrawablePrimitive* primitives;
        GLsizei primitiveCount;
        
        GLuint _vao;
        GLuint _vertexBuffer;
        GLuint* _indexBuffers;
    } Mesh3D;
    
    typedef struct QuadfStruct {
        P4C4T2f leftbottom;
        P4C4T2f lefttop;
        P4C4T2f righttop;
        P4C4T2f rightbottom;
    } Quadf;
    
    inline bool Vec2fEqualToPoint(Vec2f vec0, Vec2f vec1) {
        return (vec0.x == vec1.x && vec1.y == vec0.y);
    }
    
    unsigned long nextPOT(unsigned long x);
    
    GLint compileShader(const GLchar* const* shaderSources, int sourcesCount, GLenum type);
    
    GLint compileAndLinkShaderProgram(const GLchar* const* vertexSources, int vertexSourcesCount,
                                      const GLchar* const* fragmentSources, int fragmentSourcesCount);
    
    GLint compileAndLinkShaderProgramWithShaderPointers(const GLchar* const* vertexSources, int vertexSourcesCount,
                                      const GLchar* const* fragmentSources, int fragmentSourcesCount,
                                      GLint* pVertexShader, GLint* pFragmentShader);
    
    void createOrUpdateTexture(GLuint* pTextureID, GLint width, GLint height, GLubyte** pTextureData, GLsizei* pTextureDataSize, void(*dataSetter)(GLubyte* data, GLint pow2Width, GLint pow2Height, void* userData), void* userData);
    
//    dispatch_queue_t sharedOpenGLQueue();
    
    P4C4T2f P4C4T2fMake(GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat r, GLfloat g, GLfloat b, GLfloat a, GLfloat s, GLfloat t);
    
    void DrawablePrimitiveRelease(DrawablePrimitive primitive);
    
    void Mesh3DRelease(Mesh3D* mesh);
    
    Mesh3D Mesh3DCreate(int vertexCount, int primitiveCount);
    
    void Mesh3DPrepareVAO(Mesh3D* mesh);
    
    void Mesh3DDrawVAO(Mesh3D* mesh, int positionSlot, int colorSlot, int texcoordSlot);
    
    void Mesh3DDrawMadvSphere(Mesh3D* mesh, int positionSlot, int leftTexcoordSlot, int rightTexcoordSlot, int dstTexcoordSlot);
    
    Mesh3D createSphere(GLfloat radius, int longitudeSegments, int latitudeSegments);
    Mesh3D createSphereV0(GLfloat radius, int longitudeSegments, int latitudeSegments);
    
    Mesh3D createGrids(GLfloat width, GLfloat height, int columns, int rows);
    
    Mesh3D createQuad(P4C4T2f v0, P4C4T2f v1, P4C4T2f v2, P4C4T2f v3);
    
#ifdef __cplusplus
}
#endif

#endif /* OpenGLHelper_h */
