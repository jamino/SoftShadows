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
//
// Very basic C++ wrapper class for an OpenGL 2D texture object. Doesn't do
// much other than loading a texture from a file.
///////////////////////////////////////////////////////////////////////////////

#pragma once


#include "Asset.hpp"


///////////////////////////////////////////////////////////////////////////////
// Texture class
///////////////////////////////////////////////////////////////////////////////
class Texture : public Asset< Texture >
{
public:
	// Binds this texture to the active texture unit (ie just calls
	// glBindTexture).
	void				Bind();

private:
	// The constructor loads a texture from the specified file.
						Texture( const string & fileName );

	// The OpenGL ID reserved for this texture object.
	GLuint				m_Id;

	friend class Scene;
};
