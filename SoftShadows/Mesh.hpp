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


#include "List.hpp"


class Material;
class MeshInstance;


class Mesh : private List< MeshInstance, Mesh >,
			 private List< Mesh, Material >::Item
{
public:
										Mesh( const aiMesh & assimpMesh, const size_t baseMaterialIndex );
										~Mesh();

	void								RegisterInstance( MeshInstance & instance );

	void								Render();
	void								RenderShadowVolumes();

	Material *							GetMaterial()				{ return GetList(); }

private:
	typedef vector< aiVector3D >		UvBuffer;
	typedef vector< aiColor4D >			ColourBuffer;
	typedef std::array< uint32_t, 3 >	TriVertIndices;

	vector< aiVector3D >				m_VertexBuffer;
	//vector< aiVector3D >				m_VertexNormals;
	//vector< ColourBuffer >				m_ColourBuffers;
	//vector< UvBuffer >					m_UvBuffers;
	vector< TriVertIndices >			m_TriIndexBuffer;
	string								m_Name;
	GLuint								m_VertexArrayId;
	GLuint								m_VertexBufferId;
	GLuint								m_ColourBufferId;
	GLuint								m_TexCoordBufferId;
	GLuint								m_NormalBufferId;
	GLuint								m_IndexBufferId;

	friend class List< Mesh, Material >;
	friend class List< Mesh, Material >::Item;

	friend class List< MeshInstance, Mesh >;
	friend class List< MeshInstance, Mesh >::Item;
};
