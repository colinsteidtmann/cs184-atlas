#pragma once
#include "glad.h"
#include <string>
#include <gl/GL.h>
#include "glm/gtc/type_ptr.hpp"
enum ETextureFiltering
{
    TEXTURE_FILTER_MAG_NEAREST = 0,     // Nearest criterion for magnification
    TEXTURE_FILTER_MAG_BILINEAR,        // Bilinear criterion for magnification
    TEXTURE_FILTER_MIN_NEAREST,         // Nearest criterion for minification
    TEXTURE_FILTER_MIN_BILINEAR,        // Bilinear criterion for minification
    TEXTURE_FILTER_MIN_NEAREST_MIPMAP,  // Nearest criterion for minification, but on closest mipmap
    TEXTURE_FILTER_MIN_BILINEAR_MIPMAP, // Bilinear criterion for minification, but on closest mipmap
    TEXTURE_FILTER_MIN_TRILINEAR,       // Bilinear criterion for minification on two closest mipmaps, then averaged
};

/********************************

Class:		GrassTexture

Purpose:	Wraps OpenGL texture
            object and performs
            their loading.

********************************/

class GrassTexture
{
public:
    bool ReloadTexture();

    bool LoadTexture2D();
    void BindTexture(int iTextureUnit = 0);

    void SetFiltering(int a_tfMagnification, int a_tfMinification);

    void SetSamplerParameter(GLenum parameter, GLenum value);

    void SetWrap();

    int GetMinificationFilter();
    int GetMagnificationFilter();

    int GetWidth();
    int GetHeight();
    int GetBPP();

    int GetTextureID();
    void DeleteTexture();

    void SetAnisotropyLevel(float fAnisotropyLevel);

    GrassTexture(const std::string&);

private:
    int iWidth, iHeight, iBPP; // Texture width, height, and bytes per pixel
    GLuint uiTexture;          // Texture name
    GLuint uiSampler;         // Sampler name
    bool bMipMapsGenerated=true;

    int tfMinification, tfMagnification;
    std::string s_path;
};