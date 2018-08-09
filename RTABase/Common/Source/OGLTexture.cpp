#include "OGLTexture.h"

#include <stdlib.h>

OGLTexture::OGLTexture( )
{
    texture_id = 0;
    width = 0;
    height = 0;
    data_size = 0;
    data = 0;
}

OGLTexture::OGLTexture( const OGLTexture& o )
{
    texture_id = 0;
    CopyConstructor( o );
}

OGLTexture& OGLTexture::operator=( const OGLTexture& o )
{
    if ( this != &o )
    {
        free( data );
        CopyConstructor( o );
    }

    return *this;
}

OGLTexture::~OGLTexture( )
{
    free( data );
    glDeleteTextures( 1, &texture_id );
}

void OGLTexture::Bind( )
{
    if ( texture_id == 0 )
    {
        glGenTextures( 1, &texture_id );
        glBindTexture( GL_TEXTURE_2D, texture_id );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
    }
    else
    {
        glBindTexture( GL_TEXTURE_2D, texture_id );
    }
}

bool OGLTexture::Load( const char* file_name )
{
    size_t file_name_size;

    if ( file_name == 0 )
    {
        return false;
    }

    file_name_size = strlen( file_name );

    if ( file_name_size < 5 )
    {
        return false;
    }

    if ( strcmp( file_name + file_name_size - 3, "bmp" ) != 0 )
    {
        // Load only supports bmp at this time
        return false;
    }

    if ( LoadBMP( file_name ) == false )
    {
        return false;
    }

    Bind( );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data );

    return true;
}

void OGLTexture::CopyConstructor( const OGLTexture& o )
{
    width = o.width;
    height = o.height;
    data_size = o.data_size;

    if ( o.data == 0 )
    {
        data = 0;
    }
    else
    {
        data = malloc( data_size );
        memcpy( data, o.data, data_size );

        Bind( );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data );
    }
}

bool OGLTexture::LoadBMP( const char* file_name )
{
    HANDLE file_handle;
    BITMAPFILEHEADER bitmap_header;
    DWORD bytes_read;
    DWORD info_size;
    BITMAPINFO* bitmap_info;
    DWORD image_size;

    // Open a file handle
    file_handle = CreateFile( file_name, GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL );

    if ( file_handle == INVALID_HANDLE_VALUE )
    {
        return false;
    }

    // Read in bitmap header information
    if ( ReadFile( file_handle, &bitmap_header, sizeof( BITMAPFILEHEADER ), &bytes_read, NULL ) == FALSE )
    {
        CloseHandle( file_handle );
        return false;
    }

    // Make sure all bytes were read
    if ( bytes_read != sizeof( BITMAPFILEHEADER ))
    {
        CloseHandle( file_handle );
        return false;
    }

    // Check format of bitmap file
    if ( bitmap_header.bfType != 'MB' || bitmap_header.bfOffBits < sizeof( BITMAPFILEHEADER ))
    {
        CloseHandle( file_handle );
        return false;
    }

    // Read in bitmap information structure
    info_size = bitmap_header.bfOffBits - sizeof( BITMAPFILEHEADER );
    bitmap_info = ( BITMAPINFO* )malloc( info_size );

    if ( ReadFile( file_handle, bitmap_info, info_size, &bytes_read, NULL ) == FALSE )
    {
        free( bitmap_info );
        CloseHandle( file_handle );
        return false;
    }

    if ( bytes_read != info_size )
    {
        free( bitmap_info );
        CloseHandle( file_handle );
        return false;
    }

    if ( bitmap_info->bmiHeader.biBitCount != 24 )
    {
        free( bitmap_info );
        CloseHandle( file_handle );
        return false;
    }

    // Save the size and dimensions of the bitmap
    width = bitmap_info->bmiHeader.biWidth;
    height = abs( bitmap_info->bmiHeader.biHeight );

    image_size = bitmap_info->bmiHeader.biSizeImage;

    // If the size isn't specified, calculate it anyway
    if ( image_size == 0 )
    {
        image_size = ( width * bitmap_info->bmiHeader.biBitCount + 7 ) / 8 * height;
    }

    free( bitmap_info );

    // Allocate space for the actual bitmap
    free( data );
    data_size = image_size;
    data = malloc( image_size );

    // Read in the bitmap bits
    if ( ReadFile( file_handle, data, image_size, &bytes_read, NULL ) == FALSE )
    {
        CloseHandle( file_handle );
        return false;
    }

    if ( bytes_read != image_size )
    {
        CloseHandle( file_handle );
        return false;
    }

    // Close the bitmap file now that we have all the data we need
    CloseHandle( file_handle );

    return true;
}