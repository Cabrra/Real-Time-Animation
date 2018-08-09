#ifndef _OGLTEXTURE_H_
#define _OGLTEXTURE_H_

#include "GLee.h"

class OGLTexture
{
public:

    OGLTexture( );
    OGLTexture( const OGLTexture& o );
    OGLTexture& OGLTexture::operator=( const OGLTexture& o );
    virtual ~OGLTexture( );

    void Bind( );
    bool Load( const char* file_name );

private:

    void CopyConstructor( const OGLTexture& o );
    bool LoadBMP( const char* file_name );


    GLuint texture_id;
    unsigned int width, height;
    unsigned int data_size;
    void* data;
};

#endif // _OGLTEXTURE_H_