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

#include "Material.hpp"

#include "Mesh.hpp"
#include "Scene.hpp"
#include "Texture.hpp"
#include "ShaderProgram.hpp"


Material::Material( const aiMaterial & assimpMaterial, const filesystem::path & scenePath )
{
	aiString diffuseTexturePath;

	if( assimpMaterial.GetTexture( aiTextureType_DIFFUSE, 0, & diffuseTexturePath ) == aiReturn_SUCCESS )
	{
		filesystem::path texturePath( scenePath );
		texturePath += diffuseTexturePath.C_Str();
		m_pDiffuseTexture = GetScene().GetAsset< Texture >( texturePath.string() );

		m_pShaderProgram = GetScene().m_pGeometryShaderProgram;
	}
}


Material::~Material()
{
}


void Material::RegisterMesh( Mesh & mesh )
{
	AddTail( mesh );
}


void Material::RenderSetup()
{
	assert( m_pShaderProgram );

	m_pShaderProgram->SetCurrent();

	if( m_pDiffuseTexture )
	{
		m_pDiffuseTexture->Bind();
		//glEnable( GL_TEXTURE_2D );
	}
	//else glDisable( GL_TEXTURE_2D );
}
