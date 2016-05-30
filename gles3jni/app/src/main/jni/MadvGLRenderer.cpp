//
//  MadvGLRenderer.mm
//  Madv360_v1
//
//  Created by QiuDong on 16/2/26.
//  Copyright © 2016年 Cyllenge. All rights reserved.
//

#include "MadvGLRenderer.h"
#include "gles2.h"

//#define USE_DISPLAYLINK

// Macros for Shader:
//#define DRAW_GRID_SPHERE
//#define ANTI_ALIAS
//#define LITTLE_PLANET

//#define EXPAND_AS_PLANE
//#define USE_MSAA

#define SPHERE_RADIUS 128
#define LONGITUDE_SEGMENTS 216
#define LATITUDE_SEGMENTS  108

#ifdef EXPAND_AS_PLANE
#define Z_SHIFT -1024
#else
#define Z_SHIFT  0
#endif

#define STRINGIZE(...) #__VA_ARGS__
#define STRINGIZE2(...) STRINGIZE(__VA_ARGS__)

const char* VertexShaderSource = STRINGIZE
(
 //#ifdef GLES
 precision highp float;
 //#endif
 
 //#define LITTLE_PLANET
 uniform int LITTLE_PLANET;
 varying float v_LITTLE_PLANET;
 
 attribute vec4 a_position; // 1
 attribute vec4 a_color; // 2
 attribute vec2 a_texCoord;
 
 attribute vec2 a_texCoordL;
 attribute vec2 a_texCoordR;
 
 uniform mat4 u_projectionMat;
 uniform mat4 u_cameraMat;
 uniform mat4 u_modelMat;
 
 varying vec4 v_color; // 3
 varying vec2 v_texCoord;
 
 varying vec2 v_texCoordL;
 varying vec2 v_texCoordR;
 
 void main(void) { // 4
     v_color = a_color; // 5
     v_texCoord = a_texCoord;
     v_texCoordL = a_texCoordL;
     v_texCoordR = a_texCoordR;
     // Modify gl_Position line as follows
     if (LITTLE_PLANET == 1)
     {
         v_LITTLE_PLANET = 1.0;
         gl_Position = a_position;
     }
     else
     {
         v_LITTLE_PLANET = 0.0;
         gl_Position = u_projectionMat * u_cameraMat * u_modelMat * a_position;
     }
 }
 
 );

///Ref: http://blog.csdn.net/opengl_es/article/details/17787495
const char* FragmentShaderSource = STRINGIZE
(
 
#ifdef EXTERNAL
#extension GL_OES_EGL_image_external : require
#endif
 
 precision highp float;
 
 uniform int FLIP_X;
 uniform int FLIP_Y;
 
 uniform int DRAW_GRID_SPHERE;
 
 uniform int CONVERT_WITH_LUT;
 
 uniform int PLAIN_STITCH;
 
 ///Ref: http://blog.csdn.net/opengl_es/article/details/17787495
 varying float v_LITTLE_PLANET;
 
 uniform int SEPARATE_SOURCE_TEXTURES;
 
 uniform int ANTI_ALIAS;
 
 varying highp vec2 v_texCoord;
 
 varying highp vec2 v_texCoordL;
 varying highp vec2 v_texCoordR;
 
#ifdef EXTERNAL
 uniform samplerExternalOES u_textureL;
 uniform samplerExternalOES u_textureR;
#else
 uniform sampler2D u_textureL;
 uniform sampler2D u_textureR;
#endif
 
 uniform highp vec2 u_dstSize;
 uniform highp vec2 u_srcSizeL;
 uniform highp vec2 u_srcSizeR;
 
 uniform vec3 u_colors[512];
 uniform int  u_longitudeFragments;
 uniform int  u_latitudeFragments;
 
 uniform float u_scale;
 uniform float u_aspect;//, time;
 //uniform mat3 transform;
 uniform mat4 u_modelMat;
 
 const float PI = 3.141592653589793;
 
 vec3 YUVTexel2RGB(vec3 yuvTexel) {
     float y = yuvTexel.r;
     float u = yuvTexel.g - 0.5;
     float v = yuvTexel.b - 0.5;
     vec3 rgb;
     rgb.r = y +             1.402 * v;
     rgb.g = y - 0.344 * u - 0.714 * v;
     rgb.b = y + 1.772 * u;
     return rgb;
 }
 
 highp vec4 gridCoord(highp vec2 texcoord, highp vec2 textureSize) {
     highp vec2 grid = texcoord * textureSize - vec2(0.5, 0.5);
     highp vec2 major = floor(grid);
     highp vec2 minor = grid - major;
     return vec4(major, minor);
 }
 
 highp vec4 texture2D_AA(sampler2D tex, highp vec2 textureSize, highp vec4 gridCoord) {
     if (ANTI_ALIAS == 1)
     {
         highp vec2 gridLB = gridCoord.xy;
         highp float p = gridCoord.z;
         highp float q = gridCoord.w;
         vec4 LB = texture2D(tex, gridLB / textureSize);
         vec4 LT = texture2D(tex, (gridLB + vec2(0.0, 1.0)) / textureSize);
         vec4 RT = texture2D(tex, (gridLB + vec2(1.0, 1.0)) / textureSize);
         vec4 RB = texture2D(tex, (gridLB + vec2(1.0, 0.0)) / textureSize);
         return (RT * p + LT * (1.0 - p)) * q + (RB * p + LB * (1.0 - p)) * (1.0 - q);
     }
     else
     {
         return texture2D(tex, (gridCoord.xy + gridCoord.zw) / textureSize);
     }
 }
 
const float MOLT_BAND_WIDTH = 20.0;
 
 vec2 lutWeights(vec2 dstTexCoord) {
     float weight = 1.0;
     float weight1 = 0.0;
     
     highp float bound0 = (u_dstSize.x * 0.25 - MOLT_BAND_WIDTH / 2.0) / u_dstSize.x;//
     highp float bound1 = (u_dstSize.x * 0.25 + MOLT_BAND_WIDTH / 2.0) / u_dstSize.x;//
     highp float bound2 = (u_dstSize.x * 0.75 - MOLT_BAND_WIDTH / 2.0) / u_dstSize.x;//
     highp float bound3 = (u_dstSize.x * 0.75 + MOLT_BAND_WIDTH / 2.0) / u_dstSize.x;//
     highp float band0Width = (bound1 - bound0) * u_dstSize.x;
     highp float band1Width = (bound3 - bound2) * u_dstSize.x;
     
     if (dstTexCoord.s >= bound1 && dstTexCoord.s < bound2)
     {
         // Use right LUT:
         weight1 = 1.0;
         weight = 0.0;
     }
     else if (dstTexCoord.s < bound0 || dstTexCoord.s >= bound3)
     {
         // Use left LUT:
         weight = 1.0;
         weight1 = 0.0;
     }
     else if (dstTexCoord.s >= bound2)
     {
         // Molt:
         weight = (dstTexCoord.s - bound2) * u_dstSize.x / band1Width;
         weight1 = 1.0 - weight;
     }
     else
     {
         weight1 = (dstTexCoord.s - bound0) * u_dstSize.x / band0Width;
         weight = 1.0 - weight1;
     }
     
     return vec2(weight, weight1);
 }
 
// highp vec4 lutMappedTexcoords(vec2 dstTexCoord) {
//     highp vec4 dstGridCoord = gridCoord(dstTexCoord, u_dstSize);
//     highp vec4 lutLX = texture2D_AA(u_lLUT_x, u_dstSize, dstGridCoord);
//     highp vec4 lutLY = texture2D_AA(u_lLUT_y, u_dstSize, dstGridCoord);
//     highp vec4 lutRX = texture2D_AA(u_rLUT_x, u_dstSize, dstGridCoord);
//     highp vec4 lutRY = texture2D_AA(u_rLUT_y, u_dstSize, dstGridCoord);
//     
//     highp vec2 texcoordL = vec2(lutLX.r * 4096.0 + lutLX.g * 4.096, lutLY.r * 4096.0 + lutLY.g * 4.096) / u_srcSizeL;
//     highp vec2 texcoordR;
//     
//     if (SEPARATE_SOURCE_TEXTURES == 1)
//     {
//         texcoordR = vec2(lutRX.r * 4096.0 + lutRX.g * 4.096, lutRY.r * 4096.0 + lutRY.g * 4.096) / u_srcSizeR;
//     }
//     else
//     {
//         texcoordR = vec2(lutRX.r * 4096.0 + lutRX.g * 4.096, lutRY.r * 4096.0 + lutRY.g * 4.096) / u_srcSizeR + vec2(0.5, 0.0);
//     }
//     
//     return vec4(texcoordL, texcoordR);
// }
 
 vec3 color3OfTexCoord(vec2 dstTexCoord) {
     if (PLAIN_STITCH == 1)
     {
         highp vec2 srcTexcoord;
         float H = (u_srcSizeL.y > u_srcSizeR.y ? u_srcSizeL.y : u_srcSizeR.y);
         if (dstTexCoord.s <= u_srcSizeL.x / (u_srcSizeL.x + u_srcSizeR.x))
         {
             srcTexcoord.s = dstTexCoord.s * (u_srcSizeL.x + u_srcSizeR.x) / u_srcSizeL.x;
             srcTexcoord.t = dstTexCoord.t * H / u_srcSizeL.y;
             return texture2D(u_textureL, srcTexcoord).rgb;
         }
         else
         {
             srcTexcoord.s = (dstTexCoord.s - u_srcSizeL.x / (u_srcSizeL.x + u_srcSizeR.x)) * (u_srcSizeL.x + u_srcSizeR.x) / u_srcSizeR.x;
             srcTexcoord.t = dstTexCoord.t * H / u_srcSizeR.y;
             return texture2D(u_textureR, srcTexcoord).rgb;
         }
     }
     else if (CONVERT_WITH_LUT == 1)
     {
         vec4 lTexel;
         vec4 rTexel;
         highp vec2 texcoordL = v_texCoordL;
         highp vec2 texcoordR = v_texCoordR;
         
//         lTexel = texture2D(u_textureL, texcoordL);
//         rTexel = texture2D(u_textureR, texcoordR);
//         highp vec4 gridCoordL = gridCoord(texcoordL, u_srcSizeL);
//         highp vec4 gridCoordR = gridCoord(texcoordR, u_srcSizeR);
//         
//         texcoordL = (gridCoordL.xy + gridCoordL.zw) / u_srcSizeL;
//         texcoordR = (gridCoordR.xy + gridCoordR.zw) / u_srcSizeR;
         
         lTexel = texture2D(u_textureL, texcoordL);//texture2D_AA_External(u_textureL, u_srcSizeL, gridCoordL);
         rTexel = texture2D(u_textureR, texcoordR);//texture2D_AA_External(u_textureR, u_srcSizeR, gridCoordR);
         
         vec2 weights = lutWeights(dstTexCoord);
         return (lTexel * weights.s + rTexel * weights.t).rgb;
     }
     else
     {
         return texture2D(u_textureL, dstTexCoord).rgb;
     }
 }
 
 void main()
{
    highp float texcoordS;
    highp float texcoordT;
    if (FLIP_X == 0)
        texcoordS = v_texCoord.s;
    else
        texcoordS = 1.0 - v_texCoord.s;
    
#ifdef FOR_520
    if (FLIP_Y == 1)
#else
        if (FLIP_Y == 0)
#endif
            texcoordT = v_texCoord.t;
        else
            texcoordT = 1.0 - v_texCoord.t;
    
    highp vec2 texcoord = vec2(texcoordS, texcoordT);
    
    if (v_LITTLE_PLANET >= 0.5)
    {
        vec2 rads = vec2(PI * 2., PI);
        
        vec2 pnt = vec2(v_texCoord.y - .5, v_texCoord.x - .5) * vec2(u_scale, u_scale * u_aspect);
        
        // Project to Sphere
        float x2y2 = pnt.x * pnt.x + pnt.y * pnt.y;
        vec3 sphere_pnt = vec3(2. * pnt, x2y2 - 1.) / (x2y2 + 1.);
        sphere_pnt = (vec4(sphere_pnt, 1.0) * u_modelMat).xyz;
        
        // Convert to Spherical Coordinates
        float r = length(sphere_pnt);
        float lon = atan(sphere_pnt.y, sphere_pnt.x);
        float lat = acos(sphere_pnt.z / r);
        texcoord = vec2(lon, lat) / rads;
        
        if (texcoord.s < 0.0) texcoord.s += 1.0;
        if (texcoord.t < 0.0) texcoord.t += 1.0;
        if (texcoord.s > 1.0) texcoord.s -= 1.0;
        if (texcoord.t > 1.0) texcoord.t -= 1.0;
    }
    
    if (DRAW_GRID_SPHERE == 1)
    {
        int row = int(texcoord.t * float(u_latitudeFragments));
        int col = int(texcoord.s * float(u_longitudeFragments));
        gl_FragColor = vec4(u_colors[row * u_longitudeFragments + col].rgb, 1.0);
    }
    else
    {
        gl_FragColor = vec4(color3OfTexCoord(texcoord), 1.0);
        //    gl_FragColor = texture2D(u_textureL, texcoord);
    }
}

 );

/// 000:Plain 001:Sphere 010:PlainV1 011:SphereV1 10X:LittlePlanet 11X:LittlePlanetV1

const int DisplayModes[] = {0x03, 0x06, 0x01, 0x04, 0x02, 0x00};
const int DisplayModesCount = 6;
const char* DisplayModeString[] = {"SphereV1", "PlanetV1", "Sphere", "Planet", "PlainV1", "Plain"};

MadvGLProgram::MadvGLProgram(const GLchar* const* vertexSources, int vertexSourcesCount, const GLchar* const* fragmentSources, int fragmentSourcesCount)
: GLProgram(vertexSources, vertexSourcesCount, fragmentSources, fragmentSourcesCount)
, _leftTextureSlot(-1)
, _rightTextureSlot(-1)
, _leftTexcoordSlot(-1)
, _rightTexcoordSlot(-1)
//, _lxLUTSlot(-1)
//, _lyLUTSlot(-1)
//, _rxLUTSlot(-1)
//, _ryLUTSlot(-1)
, _dstSizeSlot(-1)
, _leftSrcSizeSlot(-1)
, _rightSrcSizeSlot(-1)
, _scaleSlot(-1)
, _aspectSlot(-1)
, _transformSlot(-1)
, _yLeftTextureSlot(-1)
, _uLeftTextureSlot(-1)
, _vLeftTextureSlot(-1)
, _yRightTextureSlot(-1)
, _uRightTextureSlot(-1)
, _vRightTextureSlot(-1)
, _colorspaceYUVSlot(-1)
, _drawGridSphereSlot(-1)
, _convertWithLUTSlot(-1)
, _plainStitchSlot(-1)
, _littlePlanetSlot(-1)
, _separateSourceTexturesSlot(-1)
, _antiAliasSlot(-1)
, _flipXSlot(-1)
, _flipYSlot(-1)
{
    _colorspaceYUVSlot = glGetUniformLocation(_program, "COLORSPACE_YUV");
    _drawGridSphereSlot = glGetUniformLocation(_program, "DRAW_GRID_SPHERE");
    _convertWithLUTSlot = glGetUniformLocation(_program, "CONVERT_WITH_LUT");
    _plainStitchSlot = glGetUniformLocation(_program, "PLAIN_STITCH");
    _littlePlanetSlot = glGetUniformLocation(_program, "LITTLE_PLANET");
    _separateSourceTexturesSlot = glGetUniformLocation(_program, "SEPARATE_SOURCE_TEXTURES");
    _antiAliasSlot = glGetUniformLocation(_program, "ANTI_ALIAS");
    
    _flipXSlot = glGetUniformLocation(_program, "FLIP_X");
    _flipYSlot = glGetUniformLocation(_program, "FLIP_Y");
    
    _leftTexcoordSlot = glGetAttribLocation(_program, "a_texCoordL");
    _rightTexcoordSlot = glGetAttribLocation(_program, "a_texCoordR");
    
    _leftTextureSlot = glGetUniformLocation(_program, "u_textureL");
    _rightTextureSlot = glGetUniformLocation(_program, "u_textureR");
    
//    _lxLUTSlot = glGetUniformLocation(_program, "u_lLUT_x");
//    _lyLUTSlot = glGetUniformLocation(_program, "u_lLUT_y");
//    _rxLUTSlot = glGetUniformLocation(_program, "u_rLUT_x");
//    _ryLUTSlot = glGetUniformLocation(_program, "u_rLUT_y");
    _dstSizeSlot = glGetUniformLocation(_program, "u_dstSize");
    _leftSrcSizeSlot = glGetUniformLocation(_program, "u_srcSizeL");
    _rightSrcSizeSlot = glGetUniformLocation(_program, "u_srcSizeR");
    
    _scaleSlot = glGetUniformLocation(_program, "u_scale");
    _aspectSlot = glGetUniformLocation(_program, "u_aspect");
    _transformSlot = glGetUniformLocation(_program, "u_transform");
    
    _yLeftTextureSlot = glGetUniformLocation(_program, "u_textureL_Y");
    _uLeftTextureSlot = glGetUniformLocation(_program, "u_textureL_U");
    _vLeftTextureSlot = glGetUniformLocation(_program, "u_textureL_V");
    _yRightTextureSlot = glGetUniformLocation(_program, "u_textureR_Y");
    _uRightTextureSlot = glGetUniformLocation(_program, "u_textureR_U");
    _vRightTextureSlot = glGetUniformLocation(_program, "u_textureR_V");
}

static GLfloat* s_gridColors;

MadvGLRenderer::~MadvGLRenderer() {
    //    free(_gridColors);
    Mesh3DRelease(&_quadMesh);
    Mesh3DRelease(&_lutQuadMesh);
    Mesh3DRelease(&_sphereMesh);
    Mesh3DRelease(&_lutSphereMesh);
    
//    glDeleteFramebuffers(1, &_framebuffer);
    //        glDeleteRenderbuffers(1, &_depthbuffer);
#ifdef USE_MSAA
    glDeleteFramebuffers(1, &_msaaFramebuffer);
    glDeleteRenderbuffers(1, &_msaaRenderbuffer);
    glDeleteRenderbuffers(1, &_msaaDepthbuffer);
#endif
    printf("MadvGLRenderer $ dealloc\n");
    glDeleteTextures(1, &_textureL);
    glDeleteTextures(1, &_textureR);
//    glDeleteTextures(1, &_LXTexture);
//    glDeleteTextures(1, &_LYTexture);
//    glDeleteTextures(1, &_RXTexture);
//    glDeleteTextures(1, &_RYTexture);
    glDeleteTextures(3, _yuvTextures);
    _textureL = _textureR = 0;//_LXTexture = _LYTexture = _RXTexture = _RYTexture = 0;
    for (int i=0; i<3; i++)
    {
        _yuvTextures[i] = 0;
    }
    
    _glCamera = NULL;
}

MadvGLRenderer::MadvGLRenderer(const char* lutPath)//int framebuffer, int width, int height)
: _textureL(0)
, _textureR(0)
//, _LXTexture(0)
//, _LYTexture(0)
//, _RXTexture(0)
//, _RYTexture(0)
, _currentGLProgram(NULL)
#ifdef DRAW_GRID_SPHERE
, _uniGridColors(-1)
, _uniLongitudeFragments(-1)
, _uniLatitudeFragments(-1)
#endif
, _pCurrentMesh(NULL)
{
    _needRenderNewSource = false;
    
//    _framebuffer = framebuffer;
//    //                glGenRenderbuffers(1, &_depthbuffer);
//    //                glBindRenderbuffer(GL_RENDERBUFFER, _depthbuffer);
//    //                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, _width, _height);
//    //                glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
//    //                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthbuffer);
//    
//    _width = width;
//    _height = height;
    
#ifdef EXPAND_AS_PLANE
    _mesh = createGrids(2160, 1080, LONGITUDE_SEGMENTS, LATITUDE_SEGMENTS);
#else
    _quadMesh = createQuad(P4C4T2fMake(-1.f, -1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f),
                           P4C4T2fMake(-1.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f),
                           P4C4T2fMake(1.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f),
                           P4C4T2fMake(1.f, -1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f));
    _lutQuadMesh = createGrids(2.f, 2.f, LONGITUDE_SEGMENTS, LATITUDE_SEGMENTS);
    _sphereMesh = createSphereV0(SPHERE_RADIUS, LONGITUDE_SEGMENTS, LATITUDE_SEGMENTS);
    _lutSphereMesh = createSphereV0(SPHERE_RADIUS, LONGITUDE_SEGMENTS, LATITUDE_SEGMENTS);
#endif
    
#ifdef DRAW_GRID_SPHERE
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        s_gridColors = (GLfloat*) malloc(3 * sizeof(GLfloat) * LONGITUDE_SEGMENTS * LATITUDE_SEGMENTS);
        int index = 0;
        for (int iR=0; iR<LATITUDE_SEGMENTS; iR++)
        {
            for (int iC=0; iC<LONGITUDE_SEGMENTS; iC++)
            {
                index++;
                s_gridColors[index*3] = ((float)iR / (float)LATITUDE_SEGMENTS);
                s_gridColors[index*3+1] = ((float)iC / (float)LONGITUDE_SEGMENTS);
                s_gridColors[index*3+2] = s_gridColors[index*3+1];
            }
        }
        for (int i=0; i<LONGITUDE_SEGMENTS*LATITUDE_SEGMENTS; i++)
        {
            s_gridColors[i*3] = (float)(rand() % 256) / 255.f;
            s_gridColors[i*3+1] = (float)(rand() % 256) / 255.f;
            s_gridColors[i*3+2] = (float)(rand() % 256) / 255.f;
        }
    });
#endif
    
    _glCamera = new GLCamera;
    _glCamera->setYawRadius(M_PI / 2);
    
    _yuvTextures[0] = _yuvTextures[1] = _yuvTextures[2] = -1;
    
    pthread_mutex_init(&_mutex, NULL);
}


void MadvGLRenderer::prepareGLCanvas(GLint x, GLint y, GLint width, GLint height) {
    //    glEnable(GL_DEPTH_TEST);
    glViewport(x, y, width, height);
    CHECK_GL_ERROR();
}

void MadvGLRenderer::setRenderSource(void* renderSource) {
    pthread_mutex_lock(&_mutex);
    {
        _renderSource = renderSource;
        _needRenderNewSource = true;
    }
    pthread_mutex_unlock(&_mutex);
}

Vec4f gridCoord(Vec2f texcoord, Vec2f textureSize) {
    Vec2f grid = Vec2f{texcoord.s * textureSize.s - 0.5f, texcoord.t * textureSize.t - 0.5f};
    Vec2f major = Vec2f{floorf(grid.s), floorf(grid.t)};
    Vec2f minor = Vec2f{grid.s - major.s, grid.t - major.t};
    return Vec4f{major.s,major.t, minor.s,minor.t};
}

int ushortTexture(const GLushort* data, Vec2f lutDstSize, int x, int y) {
    if (x < 0) x = 0;
    if (x >= lutDstSize.s) x = lutDstSize.s - 1;
    if (y < 0) y = 0;
    if (y >= lutDstSize.t) y = lutDstSize.t - 1;
    
    return (int) data[(int)(lutDstSize.s * y + x)];
}

float ushortTexture(const GLushort* data, Vec2f lutDstSize, Vec4f gridCoord) {
    int x0 = gridCoord.x;
    int y0 = gridCoord.y;
    float p = gridCoord.z;
    float q = gridCoord.w;
    float LB = ushortTexture(data, lutDstSize, x0, y0);
    float LT = ushortTexture(data, lutDstSize, x0, y0 + 1);
    float RT = ushortTexture(data, lutDstSize, x0 + 1, y0 + 1);
    float RB = ushortTexture(data, lutDstSize, x0 + 1, y0);
    return (RT * p + LT * (1.0 - p)) * q + (RB * p + LB * (1.0 - p)) * (1.0 - q);
}

void convertMesh3DWithLUT(Mesh3D& mesh, Vec2f lutDstSize, Vec2f leftSrcSize,Vec2f rightSrcSize, int dataSizeInShort, const GLushort* lxIntData, const GLushort* lxMinData, const GLushort* lyIntData, const GLushort* lyMinData, const GLushort* rxIntData, const GLushort* rxMinData, const GLushort* ryIntData, const GLushort* ryMinData) {
    for (int i=0; i<mesh.vertexCount; ++i)
    {
        P4C4T2f& vertex = mesh.vertices[i];
        
        Vec2f dstTexCoord = Vec2f{vertex.s, vertex.t};
        
        /// lutMappedTexcoords() :
        Vec4f dstGridCoord = gridCoord(dstTexCoord, lutDstSize);
        float lxInt = ushortTexture(lxIntData, lutDstSize, dstGridCoord);
        float lxMin = ushortTexture(lxMinData, lutDstSize, dstGridCoord);
        float lyInt = ushortTexture(lyIntData, lutDstSize, dstGridCoord);
        float lyMin = ushortTexture(lyMinData, lutDstSize, dstGridCoord);
        float rxInt = ushortTexture(rxIntData, lutDstSize, dstGridCoord);
        float rxMin = ushortTexture(rxMinData, lutDstSize, dstGridCoord);
        float ryInt = ushortTexture(ryIntData, lutDstSize, dstGridCoord);
        float ryMin = ushortTexture(ryMinData, lutDstSize, dstGridCoord);
        
        Vec2f texcoordL = Vec2f{(lxInt + lxMin / 1000.f) / leftSrcSize.s, (lyInt + lyMin / 1000.f) / leftSrcSize.t};
        Vec2f texcoordR;
#ifdef SEPARATE_SOURCE_TEXTURES
        {
            texcoordR = Vec2f{(rxInt + rxMin / 1000.f) / rightSrcSize.s, (ryInt + ryMin / 1000.f) / rightSrcSize.t};
        }
#else
        {
            texcoordR = Vec2f{(rxInt + rxMin / 1000.f) / rightSrcSize.s + 0.5f, (ryInt + ryMin / 1000.f) / rightSrcSize.t};
        }
#endif
        vertex.r = texcoordL.s;
        vertex.g = texcoordL.t;
        vertex.b = texcoordR.s;
        vertex.a = texcoordR.t;
    }
}

void MadvGLRenderer::setLUTData(Vec2f lutDstSize, Vec2f leftSrcSize,Vec2f rightSrcSize, int dataSizeInShort, const GLushort* lxIntData, const GLushort* lxMinData, const GLushort* lyIntData, const GLushort* lyMinData, const GLushort* rxIntData, const GLushort* rxMinData, const GLushort* ryIntData, const GLushort* ryMinData) {
    Mesh3DRelease(&_lutSphereMesh);
    Mesh3DRelease(&_lutQuadMesh);
    
    _lutSphereMesh = createSphereV0(SPHERE_RADIUS, LONGITUDE_SEGMENTS, LATITUDE_SEGMENTS);
    _lutQuadMesh = createGrids(2.f, 2.f, LONGITUDE_SEGMENTS, LATITUDE_SEGMENTS);
    
    convertMesh3DWithLUT(_lutSphereMesh, lutDstSize, leftSrcSize, rightSrcSize, dataSizeInShort, lxIntData, lxMinData, lyIntData, lyMinData, rxIntData, rxMinData, ryIntData, ryMinData);
    convertMesh3DWithLUT(_lutQuadMesh, lutDstSize, leftSrcSize, rightSrcSize, dataSizeInShort, lxIntData, lxMinData, lyIntData, lyMinData, rxIntData, rxMinData, ryIntData, ryMinData);
}

void MadvGLRenderer::prepareGLPrograms() {
    if (_currentGLProgram) return;
    
    _currentGLProgram = NULL;
    
    //    const GLchar* const vertexSource = [[NSString stringOfBundleFile:@"PosColorTexMat_Vertex" extName:@"glsl"] UTF8String];
    //    const GLchar* lutRgbFragmentSource = [[NSString stringOfBundleFile:@"LUT_RGB_Fragment" extName:@"glsl"] UTF8String];
    _currentGLProgram = new MadvGLProgram(&VertexShaderSource,1, &FragmentShaderSource,1);
#ifdef DRAW_GRID_SPHERE
    _uniGridColors = glGetUniformLocation(_shaderProgram, "u_colors");
    _uniLongitudeFragments = glGetUniformLocation(_shaderProgram, "u_longitudeFragments");
    _uniLatitudeFragments = glGetUniformLocation(_shaderProgram, "u_latitudeFragments");
#endif
    
}

void MadvGLRenderer::setGLProgramVariables(GLint x, GLint y, GLint width, GLint height) {
    glUseProgram(_currentGLProgram->getProgram());
    CHECK_GL_ERROR();
#ifdef DRAW_GRID_SPHERE
    glUniform3fv(_uniGridColors, LONGITUDE_SEGMENTS*LATITUDE_SEGMENTS, s_gridColors);
    glUniform1i(_uniLongitudeFragments, LONGITUDE_SEGMENTS);
    glUniform1i(_uniLatitudeFragments, LATITUDE_SEGMENTS);
#endif
    
    glUniform1i(_currentGLProgram->getColorSpaceYUVSlot(), (_isYUVColorSpace? 1:0));
    glUniform1i(_currentGLProgram->getDrawGridSphereSlot(), 0);
    glUniform1i(_currentGLProgram->getConvertWithLUTSlot(), ((_currentDisplayMode & PanoramaDisplayModeLUT) ? 1 : 0));
    glUniform1i(_currentGLProgram->getPlainStitchSlot(), ((_currentDisplayMode & PanoramaDisplayModePlainStitch) ? 1 : 0));
    glUniform1i(_currentGLProgram->getLittlePlanetSlot(), ((_currentDisplayMode & PanoramaDisplayModeLittlePlanet) ? 1 : 0));
    glUniform1i(_currentGLProgram->getSeparateSourceTexturesSlot(), 0);
    glUniform1i(_currentGLProgram->getAntiAliasSlot(), 0);
    
    glUniform1i(_currentGLProgram->getFlipXSlot(), 0);
    glUniform1i(_currentGLProgram->getFlipYSlot(), 0);
    
    if (_isYUVColorSpace)
    {
        GLint yTextureSlot = _currentGLProgram->getLeftYTextureSlot();
        GLint uTextureSlot = _currentGLProgram->getLeftUTextureSlot();
        GLint vTextureSlot = _currentGLProgram->getLeftVTextureSlot();
        GLint yuvTextureSlots[3] = {yTextureSlot, uTextureSlot, vTextureSlot};
        for (int i = 0; i < 3; ++i)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, _yuvTextures[i]);
            glUniform1i(yuvTextureSlots[i], i);
        }
        
        if (_currentGLProgram->getRightYTextureSlot() >= 0)
        {
            glUniform1i(_currentGLProgram->getRightYTextureSlot(), 0);
            glUniform1i(_currentGLProgram->getRightUTextureSlot(), 1);
            glUniform1i(_currentGLProgram->getRightVTextureSlot(), 2);
        }
    }
    else
    {
        glUniform1i(_currentGLProgram->getLeftTextureSlot(), 0);
        glBindTexture(GL_TEXTURE_2D, _textureL);
        glActiveTexture(GL_TEXTURE0);
        
        if (_currentGLProgram->getRightTextureSlot() >= 0)
        {
            glUniform1i(_currentGLProgram->getRightTextureSlot(), 0);
        }
    }
    
    //#ifdef SPHERE_RENDERING
    if (!(_currentDisplayMode & PanoramaDisplayModeSphere) && !(_currentDisplayMode & PanoramaDisplayModeLittlePlanet))
    {
        kmMat4 projection;
        kmMat4Identity(&projection);
        glUniformMatrix4fv(_currentGLProgram->getProjectionMatrixSlot(), 1, 0, projection.mat);
    }
    else
    {
        _glCamera->setWidth(width);
        _glCamera->setHeight(height);
        _glCamera->setProjectionMatrix(_currentGLProgram->getProjectionMatrixSlot());
    }
    
    kmMat4 modelView;
    kmMat4Identity(&modelView);
#ifdef EXPAND_AS_PLANE
    kmMat4TranslationBy(&modelView, -500, -450, Z_SHIFT);
#else
    kmMat4TranslationBy(&modelView, 0, 0, Z_SHIFT);
#endif
    glUniformMatrix4fv(_currentGLProgram->getModelMatrixSlot(), 1, 0, modelView.mat);
    
    if (_currentDisplayMode & PanoramaDisplayModeLittlePlanet)
    {
        _glCamera->setEulerAngles(_glCamera->getPitchRadius(), _glCamera->getYawRadius(), _glCamera->getBankRadius());
    }
    
    if (!(_currentDisplayMode & PanoramaDisplayModeSphere) && !(_currentDisplayMode & PanoramaDisplayModeLittlePlanet))
    {
        kmMat4 cameraMatrix;
        kmMat4Identity(&cameraMatrix);
        glUniformMatrix4fv(_currentGLProgram->getCameraMatrixSlot(), 1, 0, cameraMatrix.mat);
    }
    else
    {
        _glCamera->setCameraMatrix(_currentGLProgram->getCameraMatrixSlot());
    }
    //#else
    //    CC3GLMatrix* identityMatrix = [CC3GLMatrix identity];
    //    glUniformMatrix4fv(_uniProjectionMat, 1, 0, identityMatrix.glMatrix);
    //    glUniformMatrix4fv(_uniModelMat, 1, 0, identityMatrix.glMatrix);
    //#endif
    
//    GLint uni_lLUT_x = _currentGLProgram->getLxLUTSlot();
//    GLint uni_lLUT_y = _currentGLProgram->getLyLUTSlot();
//    GLint uni_rLUT_x = _currentGLProgram->getRxLUTSlot();
//    GLint uni_rLUT_y = _currentGLProgram->getRyLUTSlot();
//    GLint uniLUTs[4] = {uni_lLUT_x, uni_lLUT_y, uni_rLUT_x, uni_rLUT_y};
//    GLuint texLUTs[4] = {_LXTexture, _LYTexture, _RXTexture, _RYTexture};
//    for (int i=0; i<4; i++)
//    {
//        glActiveTexture(GL_TEXTURE3 + i);
//        glBindTexture(GL_TEXTURE_2D, texLUTs[i]);
//        glUniform1i(uniLUTs[i], 3 + i);
//    }
    
    GLint uni_dstSize = _currentGLProgram->getDstSizeSlot();
    GLint uni_srcSizeL = _currentGLProgram->getLeftSrcSizeSlot();
    GLint uni_srcSizeR = _currentGLProgram->getRightSrcSizeSlot();
    glUniform2f(uni_dstSize, _lutDstSize.width, _lutDstSize.height);
    glUniform2f(uni_srcSizeL, _lutSrcSize.width, _lutSrcSize.height);
    glUniform2f(uni_srcSizeR, _lutSrcSize.width, _lutSrcSize.height);
    
    GLfloat scale = CLIP_Z_NEAR*CLIP_Z_NEAR / getFocalLength();
    glUniform1f(_currentGLProgram->getScaleSlot(), scale);
    GLfloat aspect = (GLfloat)width / (GLfloat)height;
    glUniform1f(_currentGLProgram->getAspectSlot(), 1.f / aspect);
    
    if (Vec2fEqualToPoint(Vec2fZero, _touchPoint))
    {
        glUniform2f(_currentGLProgram->getTouchTexcoordSlot(), -0.5, -0.5);
    }
    /*
     else
     {
     CC3Vector direction = CC3VectorMake((_touchPoint.x - CGRectGetMidX(self.bounds)) * CLIP_WIDTH / self.bounds.size.width,// phi::-y
     (CGRectGetMidY(self.bounds) - _touchPoint.y) * h / self.bounds.size.height,// theta::x
     -_focalLength);// phi::x, theta::-y
     CC3GLMatrix* mat = [CC3GLMatrix identity];
     CC3Vector4 quaternion = productQuaternion;
     quaternion.w = -quaternion.w;
     [mat rotateByQuaternion:quaternion];
     direction = [mat transformDirection:direction];
     
     GLfloat phi = atan(-direction.x / direction.z) + M_PI;
     GLfloat theta = (direction.y == 0.f ? M_PI /2 : atan(-direction.z / direction.y));
     if (theta < 0) theta += M_PI;
     glUniform2f(_currentGLProgram->touchTexcoordSlot, phi / 2 / M_PI, theta / M_PI);
     }
     //*/
}

void MadvGLRenderer::prepareVAO() {
    Mesh3DPrepareVAO(_pCurrentMesh);
}

void MadvGLRenderer::drawPrimitives() {
//    Mesh3DDrawVAO(_pCurrentMesh, _currentGLProgram->getPositionSlot(), _currentGLProgram->getColorSlot(), _currentGLProgram->getTexcoordSlot());
    Mesh3DDrawMadvSphere(_pCurrentMesh, _currentGLProgram->getPositionSlot(), _currentGLProgram->getLeftTexcoordSlot(), _currentGLProgram->getRightTexcoordSlot(), _currentGLProgram->getTexcoordSlot());
}

void MadvGLRenderer::draw(GLint x, GLint y, GLint width, GLint height) {
    if (_currentDisplayMode & PanoramaDisplayModeSphere)
    {
        if (_currentDisplayMode & PanoramaDisplayModeLUT)
            _pCurrentMesh = &_lutSphereMesh;
        else
            _pCurrentMesh = &_sphereMesh;
    }
    else
    {
        if (_currentDisplayMode & PanoramaDisplayModeLUT)
            _pCurrentMesh = &_lutQuadMesh;
        else
            _pCurrentMesh = &_quadMesh;
    }
    
    bool shouldUpdateTexture = false;
    void* currentRenderSource = NULL;
    pthread_mutex_lock(&_mutex);
    {
        if (_renderSource && _needRenderNewSource)
        {
            shouldUpdateTexture = true;
            _needRenderNewSource = false;
            currentRenderSource = _renderSource;
        }
    }
    pthread_mutex_unlock(&_mutex);
    if (shouldUpdateTexture)
    {
        prepareTextureWithRenderSource(currentRenderSource);
    }
    
    prepareGLCanvas(x,y, width,height);
    
//    if (!_LXTexture)
//    {
//        prepareLUTTextures();
//    }
    
    prepareGLPrograms();
    
    prepareVAO();
    
    setGLProgramVariables(x,y, width,height);
    
    drawPrimitives();
    
#ifdef USE_MSAA
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER_APPLE, _framebuffer);
    glBindFramebuffer(GL_READ_FRAMEBUFFER_APPLE, _msaaFramebuffer);
    glResolveMultisampleFramebufferAPPLE();
    
    if (_supportDiscardFramebuffer)
    {
        const GLenum discards[]  = {GL_COLOR_ATTACHMENT0,GL_DEPTH_ATTACHMENT};
        glDiscardFramebuffer(GL_READ_FRAMEBUFFER_APPLE,2,discards);
    }
#endif
}

void MadvGLRenderer::setEulerAngles(GLfloat yawRadius, GLfloat pitchRadius) {
    _glCamera->setEulerAngles(yawRadius, pitchRadius, getBankRadius());
}

void MadvGLRenderer::setQuaternion(float x, float y, float z, float w) {
    _glCamera->setQuaternion({x,y,z,w});
}
