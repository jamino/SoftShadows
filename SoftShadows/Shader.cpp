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

#include "Shader.hpp"

#include "Scene.hpp"


ShaderBase::ShaderBase( const string & fileName, const GLenum typeEnum )
	: m_Id		( glCreateShader( typeEnum ) )
	, m_TypeEnum( typeEnum )
{
	Load( fileName );
}


void ShaderBase::Load( const string & fileName )
{
	// Open the source file.
	ifstream shaderFile( fileName );

	// Find out how long it is.
	shaderFile.seekg( 0, ifstream::end );
	const GLint size = shaderFile.tellg();

	// Read it into a buffer.
	vector< char > sourceString( size );
	shaderFile.seekg( 0, ifstream::beg );
	shaderFile.read( & sourceString.front(), size );

	// Upload it to the GL and compile it.
	const char * pString = & sourceString.front();
	glShaderSource( m_Id, 1, & pString, & size );
	glCompileShader( m_Id );

	GLint infoLogLength = 0;
	glGetShaderiv( m_Id, GL_INFO_LOG_LENGTH, & infoLogLength );

	if( infoLogLength > 0 )
	{
		vector< char > infoLog( infoLogLength );
		glGetShaderInfoLog( m_Id, infoLogLength, nullptr, & infoLog.front() );
		clog << & infoLog.front() << endl;
	}

	GLint compileStatus = GL_FALSE;
	glGetShaderiv( m_Id, GL_COMPILE_STATUS, & compileStatus );

	if( compileStatus != GL_TRUE )
		throw runtime_error( "Failed to compile shader" );
}


VertexShader::VertexShader( const string & fileName )
	: ShaderBase( fileName, GL_VERTEX_SHADER )
	, Asset< VertexShader >( fileName )
{}


VertexShader::~VertexShader()
{}


FragmentShader::FragmentShader( const string & fileName )
	: ShaderBase( fileName, GL_FRAGMENT_SHADER )
	, Asset< FragmentShader >( fileName )
{}


FragmentShader::~FragmentShader()
{}


GeometryShader::GeometryShader( const string & fileName )
	: ShaderBase( fileName, GL_GEOMETRY_SHADER )
	, Asset< GeometryShader >( fileName )
{}


GeometryShader::~GeometryShader()
{}
