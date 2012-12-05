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
	, m_Id( glCreateProgram() )
	, m_pVertexShader( pVertexShader )
	, m_pFragmentShader( pFragmentShader )
	, m_pGeometryShader( pGeometryShader )
{
	glAttachShader( m_Id, m_pVertexShader->GetId() );
	glAttachShader( m_Id, m_pFragmentShader->GetId() );

	if( m_pGeometryShader )
		glAttachShader( m_Id, m_pGeometryShader->GetId() );

	Link();

	auto * const ptr = & GetScene().GetAssetSet< ShaderProgram >();

	GetScene().GetAssetSet< ShaderProgram >().insert( * this );
}


void ShaderProgram::Link()
{
	glLinkProgram( m_Id );

	GLint infoLogLength = 0;
	glGetProgramiv( m_Id, GL_INFO_LOG_LENGTH, & infoLogLength );

	if( infoLogLength > 0 )
	{
		vector< char > infoLog( infoLogLength );
		glGetProgramInfoLog( m_Id, infoLogLength, nullptr, & infoLog.front() );
		clog << & infoLog.front() << endl;
	}

	GLint linkStatus = GL_FALSE;
	glGetProgramiv( m_Id, GL_LINK_STATUS, & linkStatus );

	if( linkStatus != GL_TRUE )
		throw runtime_error( "Failed to linker shader program" );
}


void ShaderProgram::SetCurrent()
{
	glUseProgram( m_Id );
}
