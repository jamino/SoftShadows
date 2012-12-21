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

#include "SceneNode.hpp"

#include "Scene.hpp"
#include "MeshInstance.hpp"


SceneNode::SceneNode()
	: m_Name( "Root" )
{
	m_Transform.setIdentity();
}


SceneNode::SceneNode( const aiNode & assimpNode, const size_t baseMeshIndex )
	: m_Name( assimpNode.mName.C_Str() )
{
	copy( assimpNode.mTransformation[0], assimpNode.mTransformation[0] + 16, m_Transform.matrix().data() );

	for( unsigned meshInstanceIndex = 0; meshInstanceIndex < assimpNode.mNumMeshes; ++meshInstanceIndex )
		m_MeshInstances.push_back( new MeshInstance( GetScene().m_Meshes[ baseMeshIndex + assimpNode.mMeshes[ meshInstanceIndex ] ] ) );

	for( unsigned childNodeIndex = 0; childNodeIndex < assimpNode.mNumChildren; ++childNodeIndex )
		m_ChildNodes.push_back( new SceneNode( * assimpNode.mChildren[ childNodeIndex ], baseMeshIndex ) );
}


SceneNode::~SceneNode()
{}


void SceneNode::Render( const Affine3f & parentTransform )
{
	const Affine3f worldTransform = parentTransform * m_Transform;
	glLoadMatrixf( worldTransform.data() );

	foreach( MeshInstance & instance, m_MeshInstances )
		instance.Render();

	foreach( SceneNode & child, m_ChildNodes )
		child.Render( worldTransform );
}


void SceneNode::RenderShadowVolumes( const Affine3f & parentTransform )
{
	const Affine3f worldTransform = parentTransform * m_Transform;
	glLoadMatrixf( worldTransform.data() );

	foreach( MeshInstance & instance, m_MeshInstances )
		instance.RenderShadowVolumes();

	foreach( SceneNode & child, m_ChildNodes )
		child.RenderShadowVolumes( worldTransform );
}
