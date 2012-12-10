///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012, Ben Lane
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
// SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
// OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
// CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
///////////////////////////////////////////////////////////////////////////////

#include "Precomp.hpp"
#include "Common.hpp"

#include "Texture.hpp"

// TODO: Move this into the precompiled header.
#include <Magick++.h>

#include "Scene.hpp"


// We use GraphicsMagick to do all the hard work of loading an image from a
// file.
using namespace Magick;


// Useful little class template that maps C++ types to their respective integer
// type IDs in OpenGL. This is useful when you are eg given a bitmap of
// SomeType[]. You can then do a glTexImage2D specifiying
// GetGlTypeId< SomeType >::value as the image format parameter, which will
// automatically work if SomeType changes in future.
// TODO: This class doesn't belong here, maybe we need a GlHelpers.hpp header?
template< typename >
struct GetGlTypeId;


template<> struct GetGlTypeId< int8_t >		{ static const GLenum value = GL_BYTE; };
template<> struct GetGlTypeId< uint8_t >	{ static const GLenum value = GL_UNSIGNED_BYTE; };
template<> struct GetGlTypeId< int16_t >	{ static const GLenum value = GL_SHORT; };
template<> struct GetGlTypeId< uint16_t >	{ static const GLenum value = GL_UNSIGNED_SHORT; };
template<> struct GetGlTypeId< int32_t >	{ static const GLenum value = GL_INT; };
template<> struct GetGlTypeId< uint32_t >	{ static const GLenum value = GL_UNSIGNED_INT; };
template<> struct GetGlTypeId< float >		{ static const GLenum value = GL_FLOAT; };
template<> struct GetGlTypeId< double >		{ static const GLenum value = GL_DOUBLE; };


Texture::Texture( const string & fileName )
	: Asset< Texture >( fileName )
{
	clog << "Loading texture " << fileName << endl;

	// Use GraphicsMagick to load the image from the file.
	const Image image( fileName );

	// Create a texture object in OpenGL.
	glGenTextures( 1, & m_Id );
	glBindTexture( GL_TEXTURE_2D, m_Id );

	// Use linear filtering as we're not using mip-maps.
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

#if( MAGICK_PIXELS_RGBA == 1 )
#	define MAGICK_GL_FORMAT GL_RGBA
#elif( MAGICK_PIXELS_BGRA == 1 )
#	define MAGICK_GL_FORMAT GL_BGRA
#else
#	error( "Unsupported image format, expected RGBA or BGRA" )
#endif

	// Transfer image data to the GL.
	glTexImage2D(
		GL_TEXTURE_2D,		// All textures are simple 2D in this app.
		0,					// Level-of-detail 0, no mip-mapping in this app either.
		GL_RGBA8,			// The internal format that OpenGL will use to store the texture. OpenGL converts the image to this format if necessary. We'll just use standard 32-bit RGBA true colour (8-bits per channel) for simplicity.
		image.columns(),	// Width of the texture.
		image.rows(),		// Height of the texture.
		0,					// Texture border width. Not used by OpenGL anymore, should always be zero.
		MAGICK_GL_FORMAT,	// The format of the image data. GraphicsMagick is always either RGBA or BGRA depending on the value of a preprocessor constant (see above).
		GetGlTypeId< Quantum >::value,	// The data type of pixels in the image. This is dictated by the GraphicsMagick "Quantum" type. Note that OpenGL wants an integer constant to identify the type, hence the use of GetGlTypeId.
		image.getConstPixels( 0, 0, image.columns(), image.rows() )		// And finally, give OpenGL a pointer to the actual image data.
	);
}


void Texture::Bind()
{
	glBindTexture( GL_TEXTURE_2D, m_Id );
}
