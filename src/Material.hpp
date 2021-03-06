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


class Mesh;
class Texture;
class ShaderProgram;


class Material : private List< Mesh, Material >
{
public:
										Material( const aiMaterial & assimpMaterial, const filesystem::path & scenePath );
										~Material();

	void								RegisterMesh( Mesh & mesh );

	void								RenderSetup();

	std::shared_ptr< ShaderProgram >	m_pShaderProgram;
	std::shared_ptr< Texture >			m_pDiffuseTexture;

	friend class List< Mesh, Material >;
	friend class List< Mesh, Material >::Item;
};
