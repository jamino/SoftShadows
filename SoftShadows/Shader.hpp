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

#pragma once


#include "Asset.hpp"


class ShaderBase
{
public:
	GLint			GetId() const			{ return m_Id; }

protected:
					ShaderBase( const string & fileName, const GLenum typeEnum );

	void			Load( const string & fileName );

private:
	const GLint		m_Id;
	const GLenum	m_TypeEnum;
};


class VertexShader : public ShaderBase,
					 public Asset< VertexShader >
{
public:
					~VertexShader();

	void			Load()					{ ShaderBase::Load( m_FileName ); }

private:
					VertexShader( const string & fileName );

	friend class Scene;
};


class FragmentShader : public ShaderBase,
					   public Asset< FragmentShader >
{
public:
					~FragmentShader();

	void			Load()					{ ShaderBase::Load( m_FileName ); }

private:
					FragmentShader( const string & fileName );

	friend class Scene;
};


class GeometryShader : public ShaderBase,
					   public Asset< GeometryShader >
{
public:
					~GeometryShader();

	void			Load()					{ ShaderBase::Load( m_FileName ); }

private:
					GeometryShader( const string & fileName );

	friend class Scene;
};
