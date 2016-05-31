//
// Created by QiuDong on 16/5/31.
//

#include "MadvGLRenderer_Android.h"
#include "Log.h"
#include <stdio.h>
#include <stdlib.h>
#include "libpng/png.h"

/******************************图片数据*********************************/
typedef struct _pic_data pic_data;
struct _pic_data
{
    int width, height; /* 尺寸 */
    int bit_depth;  /* 位深 */
    int flag;   /* 一个标志，表示是否有alpha通道 */

    unsigned char **rgba; /* 图片数组 */
};
/**********************************************************************/
#define PNG_BYTES_TO_CHECK 4
#define HAVE_ALPHA 1
#define NO_ALPHA 0

int decodePNG(char *filepath, pic_data *out)
/* 用于解码png图片 */
{
    FILE *pic_fp;
    pic_fp = fopen(filepath, "rb");
    if(pic_fp == NULL) /* 文件打开失败 */
        return -1;

    /* 初始化各种结构 */
    png_structp png_ptr;
    png_infop  info_ptr;
    char        buf[PNG_BYTES_TO_CHECK];
    int        temp;

    png_ptr  = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    info_ptr = png_create_info_struct(png_ptr);

    setjmp(png_jmpbuf(png_ptr)); // 这句很重要

    temp = fread(buf,1,PNG_BYTES_TO_CHECK,pic_fp);
    temp = png_sig_cmp(static_cast<png_const_bytep>((void*)buf), (png_size_t)0, PNG_BYTES_TO_CHECK);

    /*检测是否为png文件*/
    if (temp!=0) return 1;

    rewind(pic_fp);
    /*开始读文件*/
    png_init_io(png_ptr, pic_fp);
    // 读文件了
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);

    int color_type,channels;

    /*获取宽度，高度，位深，颜色类型*/
    channels      = png_get_channels(png_ptr, info_ptr); /*获取通道数*/
    out->bit_depth = png_get_bit_depth(png_ptr, info_ptr); /* 获取位深 */
    color_type    = png_get_color_type(png_ptr, info_ptr); /*颜色类型*/

    int i,j;
    int size, pos = 0;
    /* row_pointers里边就是rgba数据 */
    png_bytep* row_pointers;
    row_pointers = png_get_rows(png_ptr, info_ptr);
    out->width = png_get_image_width(png_ptr, info_ptr);
    out->height = png_get_image_height(png_ptr, info_ptr);

    size = out->width * out->height; /* 计算图片的总像素点数量 */

    ALOGE("channels = %d, bits = %d, withAlpha = %d, width = %d, height = %d, colorType = %d\n", channels, out->bit_depth, out->flag, out->width, out->height, color_type);

    if(channels == 4 || color_type == PNG_COLOR_TYPE_RGB_ALPHA)
    {/*如果是RGB+alpha通道，或者RGB+其它字节*/
        size *= (4*sizeof(unsigned char)); /* 每个像素点占4个字节内存 */
        out->flag = HAVE_ALPHA;    /* 标记 */
        out->rgba = (unsigned char**) malloc(size);
        if(out->rgba == NULL)
        {/* 如果分配内存失败 */
            fclose(pic_fp);
            puts("错误(png):无法分配足够的内存供存储数据!");
            return 1;
        }

        temp = (4 * out->width);/* 每行有4 * out->width个字节 */
        for(i = 0; i < out->height; i++)
        {
            for(j = 0; j < temp; j += 4)
            {/* 一个字节一个字节的赋值 */
                out->rgba[0][pos] = row_pointers[i][j]; // red
                out->rgba[1][pos] = row_pointers[i][j+1]; // green
                out->rgba[2][pos] = row_pointers[i][j+2];  // blue
                out->rgba[3][pos] = row_pointers[i][j+3]; // alpha
                ++pos;
            }
        }
    }
    else if(channels == 3 || color_type == PNG_COLOR_TYPE_RGB)
    {/* 如果是RGB通道 */
        size *= (3*sizeof(unsigned char)); /* 每个像素点占3个字节内存 */
        out->flag = NO_ALPHA;    /* 标记 */
        out->rgba = (unsigned char**) malloc(size);
        if(out->rgba == NULL)
        {/* 如果分配内存失败 */
            fclose(pic_fp);
            puts("错误(png):无法分配足够的内存供存储数据!");
            return 1;
        }

        temp = (3 * out->width);/* 每行有3 * out->width个字节 */
        for(i = 0; i < out->height; i++)
        {
            for(j = 0; j < temp; j += 3)
            {/* 一个字节一个字节的赋值 */
                out->rgba[0][pos] = row_pointers[i][j]; // red
                out->rgba[1][pos] = row_pointers[i][j+1]; // green
                out->rgba[2][pos] = row_pointers[i][j+2];  // blue
                ++pos;
            }
        }
    }
    else if (channels == 1 || color_type == PNG_COLOR_TYPE_GRAY)
    {
        size *= (2*sizeof(unsigned char)); /* 每个像素点占3个字节内存 */
        out->flag = NO_ALPHA;
        out->rgba = (unsigned char**) malloc(size);
        if(out->rgba == NULL)
        {/* 如果分配内存失败 */
            fclose(pic_fp);
            puts("错误(png):无法分配足够的内存供存储数据!");
            return 1;
        }

        unsigned char* pDst = (unsigned char*) out->rgba;
        for(i = 0; i < out->height; i++)
        {
            memcpy(pDst, row_pointers[i], out->width * 2);
            pDst += (out->width * 2);
        }
    }
    else
        return 1;

    /* 撤销数据占用的内存 */
    png_destroy_read_struct(&png_ptr, &info_ptr, 0);
    return 0;
}

MadvGLRenderer_Android::~MadvGLRenderer_Android() {
}

MadvGLRenderer_Android::MadvGLRenderer_Android(const char* lutPath)
        : MadvGLRenderer(lutPath)
{
    prepareLUT(lutPath);
}

void MadvGLRenderer_Android::prepareLUT(const char* lutPath) {
    pic_data lutDatas[8];
    const char* LUTFilePathPattern[] = {"%s/L_x_int.png", "%s/L_x_min.png", "%s/L_y_int.png", "%s/L_y_min.png", "%s/R_x_int.png", "%s/R_x_min.png", "%s/R_y_int.png", "%s/R_y_min.png"};

    char* lutFilePath = (char*) malloc(strlen(lutPath) + strlen("/L_x_int.png") + 1);
    for (int i=0; i<8; ++i)
    {
        sprintf(lutFilePath, LUTFilePathPattern[i], lutPath);
        int result = decodePNG((char*)lutFilePath, lutDatas + i);
        ALOGE("lutFilePath = %s, result = %d\n", lutFilePath, result);

        ///!!!For Debug
        unsigned short* pShort = (unsigned short*) lutDatas[i].rgba;
        unsigned short min = 10240, max = 0;
        int length = lutDatas[i].width * lutDatas[i].height;
        for (int j=length; j>0; --j)
        {
            unsigned short s = *pShort;
            s = ((s << 8) & 0xff00) | ((s >> 8) & 0x00ff);
            *pShort++ = s;

            if (s > max) max = s;
            if (s < min) min = s;
        }
        ALOGE("min = %d, max = %d", min, max);
    }

    setLUTData(Vec2f{(GLfloat)lutDatas[0].width, (GLfloat)lutDatas[0].height}, Vec2f{3456.f, 1728.f}, Vec2f{3456.f, 1728.f}, lutDatas[0].width * lutDatas[0].height,
               (unsigned short*)lutDatas[0].rgba, (unsigned short*)lutDatas[1].rgba, (unsigned short*)lutDatas[2].rgba, (unsigned short*)lutDatas[3].rgba, (unsigned short*)lutDatas[4].rgba, (unsigned short*)lutDatas[5].rgba, (unsigned short*)lutDatas[6].rgba, (unsigned short*)lutDatas[7].rgba);

    for (int i=0; i<8; ++i)
    {
        free(lutDatas[i].rgba);
    }
    free(lutFilePath);
}

void MadvGLRenderer_Android::prepareTextureWithRenderSource(void* renderSource) {
    
}
