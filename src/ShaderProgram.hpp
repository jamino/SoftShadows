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
// Like other compiled languages, individual shaders are compiled separately
// and then linked into a program object. Current day OpenGL programs require
// at least a vertex shader and a fragment shader. Before rendering you need to
// select the active shader program to use. This class is just a simple C++
// wrapper for OpenGL shader programs.
///////////////////////////////////////////////////////////////////////////////

#pragma once


#include "Asset.hpp"
#include "Shader.hpp"


///////////////////////////////////////////////////////////////////////////////
// ShaderProgram class
///////////////////////////////////////////////////////////////////////////////
class ShaderProgram : public Asset< ShaderProgram >
{
public:
	// The constructor requires a string name for the program, a pointer to the
	// vertex shader to use, a pointer to the fragment shader to use and
	// optionally a pointer to a geometry shader to use.
												ShaderProgram( const string & name,
															   const std::shared_ptr< VertexShader > & pVertexShader,
															   const std::shared_ptr< FragmentShader > & pFragmentShader,
															   const std::shared_ptr< GeometryShader > & pGeometryShader = std::shared_ptr< GeometryShader >() );

	// Sets this as the current program used for rendering.
	void										SetCurrent();

	// Returns the unique integer value that OpenGL uses to identify this
	// program.
	GLint										GetID() const						{ return m_Id; }

	const std::shared_ptr< VertexShader > &		GetVertexShader() const				{ return m_pVertexShader; }
	const std::shared_ptr< FragmentShader > &	GetFragmentShader() const			{ return m_pFragmentShader; }
	const std::shared_ptr< GeometryShader > &	GetGeometryShader() const			{ return m_pGeometryShader; }

	// Links (or re-links) the individual shaders together to form the program
	// object. The attached shaders must be successfully compiled prior to this
	// in order for the program to link successfully.
	void										Link();

private:
	const GLint									m_Id;
	std::shared_ptr< VertexShader >				m_pVertexShader;
	std::shared_ptr< FragmentShader >			m_pFragmentShader;
	std::shared_ptr< GeometryShader >			m_pGeometryShader;
};
