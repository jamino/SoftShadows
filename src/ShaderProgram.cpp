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

#include "ShaderProgram.hpp"

#include "Scene.hpp"


ShaderProgram::ShaderProgram( const string & name,
							  const std::shared_ptr< VertexShader > & pVertexShader,
							  const std::shared_ptr< FragmentShader > & pFragmentShader,
							  const std::shared_ptr< GeometryShader > & pGeometryShader )
	: Asset( name )
	, m_Id( glCreateProgram() )			// Create an OpenGL program object and save the unique ID value that the GL assigned to it.
	, m_pVertexShader( pVertexShader )
	, m_pFragmentShader( pFragmentShader )
	, m_pGeometryShader( pGeometryShader )
{
	// Attach the vertex and fragment shaders.
	glAttachShader( m_Id, m_pVertexShader->GetId() );
	glAttachShader( m_Id, m_pFragmentShader->GetId() );

	// Attach the geometry shader if one was specified.
	if( m_pGeometryShader )
		glAttachShader( m_Id, m_pGeometryShader->GetId() );

	// Link the shaders into a program.
	Link();

	// Shader programs are currently only created from code and not loaded from
	// a file so we need to insert them into the respective asset cache manually.
	// TODO: This is messy, change this.
	auto * const ptr = & GetScene().GetAssetSet< ShaderProgram >();

	GetScene().GetAssetSet< ShaderProgram >().insert( * this );
}


void ShaderProgram::Link()
{
	// Link the program.
	glLinkProgram( m_Id );

	// See if we got any info back from the GL.
	GLint infoLogLength = 0;
	glGetProgramiv( m_Id, GL_INFO_LOG_LENGTH, & infoLogLength );

	if( infoLogLength > 0 )
	{
		// Got some info back, add it to the log.
		vector< char > infoLog( infoLogLength );
		glGetProgramInfoLog( m_Id, infoLogLength, nullptr, & infoLog.front() );
		clog << & infoLog.front() << endl;
	}

	// Check if the link was successful.
	GLint linkStatus = GL_FALSE;
	glGetProgramiv( m_Id, GL_LINK_STATUS, & linkStatus );

	if( linkStatus != GL_TRUE )
		// There was an error linking the program, throw an exception.
		// TODO: Might be better to pop up an error box rather than throw an
		//		exception here. Logging an error gives the user a chance to fix
		//		the shader files and re-compile them.
		throw runtime_error( "Failed to link shader program" );
}


// Sets this as the current program used for rendering.
void ShaderProgram::SetCurrent()
{
	glUseProgram( m_Id );
}
