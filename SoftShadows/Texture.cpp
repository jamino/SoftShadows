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

#include <Magick++.h>

#include "Scene.hpp"


using namespace Magick;


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

	const Image image( fileName );

	glGenTextures( 1, & m_Id );
	glBindTexture( GL_TEXTURE_2D, m_Id );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

#if( MAGICK_PIXELS_RGBA == 1 )
#	define MAGICK_GL_FORMAT GL_RGBA
#elif( MAGICK_PIXELS_BGRA == 1 )
#	define MAGICK_GL_FORMAT GL_BGRA
#else
#	error( "Unsupported image format, expected RGBA or BGRA" )
#endif

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		4,
		image.columns(),
		image.rows(),
		0,
		MAGICK_GL_FORMAT,
		GetGlTypeId< Quantum >::value,
		image.getConstPixels( 0, 0, image.columns(), image.rows() )
	);
}


void Texture::Bind()
{
	glBindTexture( GL_TEXTURE_2D, m_Id );
}
