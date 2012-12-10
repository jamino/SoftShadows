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
// A shader is a program written in a C-like language (GLSL) that can be
// plugged into a specific stage of the OpenGL rendering pipeline. They are
// very powerful, flexible and expressive allowing us to do things like custom
// rendering techniques and complex physics/maths simulations as two examples.
// There are currently five types of shader in OpenGL. Tessellation control
// shaders and tessellation evaluation shaders are generally used to generate
// more primitives (eg triangles) as you get closer to a surface, especially a
// curved and/or bumpy surface. This achieves automatic level-of-detail; you
// use more triangles to represent complex geometry that is closer to the
// camera than for geometry that is further away. They also generally achieve
// level-of-detail changes in a visually smoother way than apps that switch
// between a fixed number of meshes at different levels of detail. Tessellation
// shaders are not used in this app and are not discussed any further. The other
// types of shader are discussed below.
///////////////////////////////////////////////////////////////////////////////

#pragma once


#include "Asset.hpp"


///////////////////////////////////////////////////////////////////////////////
// ShaderBase class
// Common base class for all types of shader. Provides functionality for
// loading a shader from a file, which doesn't depend on shader type.
///////////////////////////////////////////////////////////////////////////////
class ShaderBase
{
public:
	// Returns the OpenGL ID that is reserved for this shader.
	GLint			GetId() const			{ return m_Id; }

protected:
	// The constructor takes the name of the file to load the shader from and
	// the OpenGL type ID integer value that identifies what type of shader
	// this is (eg GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, etc.).
					ShaderBase( const string & fileName, const GLenum typeEnum );

	// Loads (or reloads) the shader from the specified file.
	void			Load( const string & fileName );

private:
	const GLint		m_Id;
	const GLenum	m_TypeEnum;
};


///////////////////////////////////////////////////////////////////////////////
// GeometryShader class
// Geometry shaders take the vertices of a single primitive as input and output
// vertices for zero or more primitives. This is the natural stage to eg
// extrude single faces into shadow volumes, which is what we do in this app.
///////////////////////////////////////////////////////////////////////////////
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


///////////////////////////////////////////////////////////////////////////////
// VertexShader class
// Vertex shaders take a single vertex and its associated data as input and
// output a single (possibly transformed) vertex along with any associated
// data. The most common job that vertex shaders usually do is to transform the
// vertex position from model space into clip space.
///////////////////////////////////////////////////////////////////////////////
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


///////////////////////////////////////////////////////////////////////////////
// FragmentShader class
// The main job of the fragment shader is to set the colour of pixels in a
// framebuffer (or some arbitrary data type when doing off-screen rendering).
// The data corrpesonding to the vertices of a primitive are (optionally)
// interpolated to every pixel that the primitive covers. This interpolated
// data is what the fragment shader receives as input.
///////////////////////////////////////////////////////////////////////////////
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
