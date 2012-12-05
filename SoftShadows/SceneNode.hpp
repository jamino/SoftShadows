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


class MeshInstance;


class SceneNode
{
public:
								SceneNode();
								SceneNode( const aiNode & assimpNode, const size_t baseMeshIndex );
								~SceneNode();

	void						Render( const Affine3f & parentTransform );
	void						RenderShadowVolumes( const Affine3f & parentTransform );

	ptr_list< MeshInstance >	m_MeshInstances;
	ptr_list< SceneNode >		m_ChildNodes;
	Affine3f					m_Transform;
	string						m_Name;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};
