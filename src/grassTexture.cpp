
#include <windows.h>
#include "grassTexture.h"
#include <FreeImage.h>

GrassTexture::GrassTexture(const std::string& path)
{
    s_path=path;
}

/*-----------------------------------------------

Name:	LoadTexture2D


Result:	Loads texture from a file, supports most
        graphics formats.

/*---------------------------------------------*/

bool GrassTexture::LoadTexture2D()
{

    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    FIBITMAP *dib(0);

    fif = FreeImage_GetFileType(s_path.c_str(), 0); // Check the file signature and deduce its format

    if (fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
        fif = FreeImage_GetFIFFromFilename(s_path.c_str());

    if (fif == FIF_UNKNOWN) // If still unknown, return failure
        return false;

    if (FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
        dib = FreeImage_Load(fif, s_path.c_str());
    if (!dib)
        return false;

    BYTE *bDataPointer = FreeImage_GetBits(dib); // Retrieve the image data

    // If somehow one of these failed (they shouldn't), return failure
    if (bDataPointer == NULL || FreeImage_GetWidth(dib) == 0 || FreeImage_GetHeight(dib) == 0)
        return false;

    GLenum format = 0;
    int bada = FreeImage_GetBPP(dib);
    if (FreeImage_GetBPP(dib) == 32)
        format = GL_RGBA;
    if (FreeImage_GetBPP(dib) == 24)
        format = GL_BGR;
    if (FreeImage_GetBPP(dib) == 8)
        format = GL_LUMINANCE;
    

    FreeImage_Unload(dib);

    if (format == 0)
        return false;

    SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_TRILINEAR);
    SetWrap();
    SetAnisotropyLevel(4.0f);

    return true; // Success
}

void GrassTexture::SetSamplerParameter(GLenum parameter, GLenum value)
{
    glSamplerParameteri(uiSampler, parameter, value);
}

/*-----------------------------------------------

Name:	SetFiltering

Params:	tfMagnification - mag. filter, must be from
                            ETextureFiltering enum
        tfMinification - min. filter, must be from
                            ETextureFiltering enum

Result:	Sets magnification and minification
            texture filter.

/*---------------------------------------------*/

void GrassTexture::SetFiltering(int a_tfMagnification, int a_tfMinification)
{
    glBindSampler(0, uiSampler);

    // Set magnification filter
    if (a_tfMagnification == TEXTURE_FILTER_MAG_NEAREST)
        glSamplerParameteri(uiSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    else if (a_tfMagnification == TEXTURE_FILTER_MAG_BILINEAR)
        glSamplerParameteri(uiSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Set minification filter
    if (a_tfMinification == TEXTURE_FILTER_MIN_NEAREST)
        glSamplerParameteri(uiSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    else if (a_tfMinification == TEXTURE_FILTER_MIN_BILINEAR)
        glSamplerParameteri(uiSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    else if (a_tfMinification == TEXTURE_FILTER_MIN_NEAREST_MIPMAP)
        glSamplerParameteri(uiSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    else if (a_tfMinification == TEXTURE_FILTER_MIN_BILINEAR_MIPMAP)
        glSamplerParameteri(uiSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    else if (a_tfMinification == TEXTURE_FILTER_MIN_TRILINEAR)
        glSamplerParameteri(uiSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    tfMinification = a_tfMinification;
    tfMagnification = a_tfMagnification;
}

/*-----------------------------------------------

Name:	BindTexture

Params:	iTextureUnit - texture unit to bind texture to

Result:	Guess what it does :)

/*---------------------------------------------*/

void GrassTexture::BindTexture(int iTextureUnit)
{
    glActiveTexture(GL_TEXTURE0 + iTextureUnit);
    glBindTexture(GL_TEXTURE_2D, uiTexture);
    glBindSampler(iTextureUnit, uiSampler);
}

/*-----------------------------------------------

Name:	DeleteTexture

Params:	none

Result:	Frees all memory used by texture.

/*---------------------------------------------*/


void GrassTexture::DeleteTexture()
{
    glDeleteSamplers(1, &uiSampler);
    glDeleteTextures(1, &uiTexture);
}

/*-----------------------------------------------

Name:	Getters

Params:	none

Result:	... They get something :D

/*---------------------------------------------*/

int GrassTexture::GetMinificationFilter()
{
    return tfMinification;
}

int GrassTexture::GetMagnificationFilter()
{
    return tfMagnification;
}

int GrassTexture::GetWidth()
{
    return iWidth;
}

int GrassTexture::GetHeight()
{
    return iHeight;
}

int GrassTexture::GetBPP()
{
    return iBPP;
}

int GrassTexture::GetTextureID()
{
    return uiTexture;
}


bool GrassTexture::ReloadTexture()
{
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    FIBITMAP *dib(0);

    fif = FreeImage_GetFileType(s_path.c_str(), 0); // Check the file signature and deduce its format

    if (fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
        fif = FreeImage_GetFIFFromFilename(s_path.c_str());

    if (fif == FIF_UNKNOWN) // If still unknown, return failure
        return false;

    if (FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
        dib = FreeImage_Load(fif, s_path.c_str());
    if (!dib)
        return false;

    BYTE *bDataPointer = FreeImage_GetBits(dib); // Retrieve the image data

    // If somehow one of these failed (they shouldn't), return failure
    if (bDataPointer == NULL || FreeImage_GetWidth(dib) == 0 || FreeImage_GetHeight(dib) == 0)
        return false;

    GLenum format;
    int bada = FreeImage_GetBPP(dib);
    if (FreeImage_GetBPP(dib) == 32)
        format = GL_RGBA;
    if (FreeImage_GetBPP(dib) == 24)
        format = GL_BGR;
    if (FreeImage_GetBPP(dib) == 8)
        format = GL_LUMINANCE;
    
    glBindTexture(GL_TEXTURE_2D, uiTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, iWidth, iHeight, format, GL_UNSIGNED_BYTE, bDataPointer);
    if (bMipMapsGenerated)
        glGenerateMipmap(GL_TEXTURE_2D);

    FreeImage_Unload(dib);

    return true; // Success
}

void GrassTexture::SetWrap()
{
    glBindSampler(0, uiSampler);
    glSamplerParameteri(uiSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(uiSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void GrassTexture::SetAnisotropyLevel(float fAnisotropyLevel)
{
    glSamplerParameterf(uiSampler, GL_TEXTURE_MAX_ANISOTROPY, fAnisotropyLevel);
}


// void LoadTexture()
// {
//     // Load textures


//     FOR(i, NUMTEXTURES)
//     {
//         tTextures[i].LoadTexture2D("data\\textures\\" + sTextureNames[i], true);
//         if (i != 7)
//             tTextures[i].SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
//         else
//         {
//             tTextures[i].SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_TRILINEAR);
//             tTextures[i].SetWrap();
//             tTextures[i].SetAnisotropyLevel(4.0f);
//         }
//     }
// }