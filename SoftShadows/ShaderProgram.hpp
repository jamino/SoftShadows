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
#include "Shader.hpp"


class ShaderProgram : public Asset< ShaderProgram >
{
public:
												ShaderProgram( const string & name,
															   const std::shared_ptr< VertexShader > & pVertexShader,
															   const std::shared_ptr< FragmentShader > & pFragmentShader,
															   const std::shared_ptr< GeometryShader > & pGeometryShader = std::shared_ptr< GeometryShader >() );

	void										SetCurrent();
	GLint										GetID() const						{ return m_Id; }

	const std::shared_ptr< VertexShader > &		GetVertexShader() const				{ return m_pVertexShader; }
	const std::shared_ptr< FragmentShader > &	GetFragmentShader() const			{ return m_pFragmentShader; }
	const std::shared_ptr< GeometryShader > &	GetGeometryShader() const			{ return m_pGeometryShader; }

	void										Link();

private:
	const GLint									m_Id;
	std::shared_ptr< VertexShader >				m_pVertexShader;
	std::shared_ptr< FragmentShader >			m_pFragmentShader;
	std::shared_ptr< GeometryShader >			m_pGeometryShader;
};
