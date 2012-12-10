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

#include "Mesh.hpp"

#include "Scene.hpp"
#include "Material.hpp"
#include "MeshInstance.hpp"


Mesh::Mesh( const aiMesh & assimpMesh, const size_t baseMaterialIndex )
	: m_VertexBuffer( assimpMesh.mVertices, assimpMesh.mVertices + assimpMesh.mNumVertices )
	//, m_VertexNormals( assimpMesh.mNormals, assimpMesh.mNormals + assimpMesh.mNumVertices )
	//, m_ColourBuffers( assimpMesh.GetNumColorChannels() )
	//, m_UvBuffers( assimpMesh.mNumUVComponents )
	, m_TriIndexBuffer( assimpMesh.mNumFaces )
	, m_Name( assimpMesh.mName.C_Str() )
	, m_ColourBufferId( 0 )
	, m_TexCoordBufferId( 0 )
	, m_NormalBufferId( 0 )
{
	GetScene().m_Materials[ baseMaterialIndex + assimpMesh.mMaterialIndex ].RegisterMesh( * this );

	for( unsigned faceIndex = 0; faceIndex < assimpMesh.mNumFaces; ++faceIndex )
	{
		const unsigned int * const pIndices = assimpMesh.mFaces[ faceIndex ].mIndices;
		copy( pIndices, pIndices + 3, m_TriIndexBuffer[ faceIndex ].data() );
	}

	glGenVertexArrays( 1, & m_VertexArrayId );
	glBindVertexArray( m_VertexArrayId );

	glGenBuffers( 1, & m_VertexBufferId );
	glBindBuffer( GL_ARRAY_BUFFER, m_VertexBufferId );

	glBufferData( GL_ARRAY_BUFFER, m_VertexBuffer.size() * sizeof( m_VertexBuffer.front() ), m_VertexBuffer.data(), GL_STATIC_DRAW );

	//glEnableVertexAttribArray( 0 );
	//glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );

	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 3, GL_FLOAT, 0, 0 );

	if( assimpMesh.HasTextureCoords( 0 ) )
	{
		glGenBuffers( 1, & m_TexCoordBufferId );
		glBindBuffer( GL_ARRAY_BUFFER, m_TexCoordBufferId );

		glBufferData( GL_ARRAY_BUFFER, m_VertexBuffer.size() * sizeof( * assimpMesh.mTextureCoords[ 0 ] ), assimpMesh.mTextureCoords[ 0 ], GL_STATIC_DRAW );

		//glEnableVertexAttribArray( 1 );
		//glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, 0 );

		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer( 3, GL_FLOAT, 0, 0 );
	}

	assert( assimpMesh.HasNormals() );

	glGenBuffers( 1, & m_NormalBufferId );
	glBindBuffer( GL_ARRAY_BUFFER, m_NormalBufferId );

	glBufferData( GL_ARRAY_BUFFER, m_VertexBuffer.size() * sizeof( * assimpMesh.mNormals ), assimpMesh.mNormals, GL_STATIC_DRAW );

	glEnableClientState( GL_NORMAL_ARRAY );
	glNormalPointer( GL_FLOAT, 0, 0 );

	glGenBuffers( 1, & m_IndexBufferId );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferId );

	glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_TriIndexBuffer.size() * sizeof( m_TriIndexBuffer.front() ), m_TriIndexBuffer.data(), GL_STATIC_DRAW );

	glBindVertexArray( 0 );
}


Mesh::~Mesh()
{
}


void Mesh::RegisterInstance( MeshInstance & instance )
{
	AddTail( instance );
}


void Mesh::Render()
{
	glBindVertexArray( m_VertexArrayId );

	GetMaterial()->RenderSetup();

	if( m_TexCoordBufferId == 0 )
		glDisable( GL_TEXTURE_2D );

	glDrawElements( GL_TRIANGLES, 3 * m_TriIndexBuffer.size(), GL_UNSIGNED_INT, 0 );
}


void Mesh::RenderShadowVolumes()
{
	glBindVertexArray( m_VertexArrayId );

	if( m_TexCoordBufferId == 0 )
		glDisable( GL_TEXTURE_2D );

	glDrawElements( GL_TRIANGLES, 3 * m_TriIndexBuffer.size(), GL_UNSIGNED_INT, 0 );
}
