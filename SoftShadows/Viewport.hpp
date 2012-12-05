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


#include "Camera.hpp"


class Viewport
{
public:
					Viewport( const GLint width, const GLint height );

	void			OnSize( const GLint width, const GLint height );
	void			Render();

	Camera			m_Camera;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
	GLuint			m_GeometryFramebufferId;
	GLuint			m_ShadowFramebufferId;
	GLuint			m_DepthRenderBufferId;
	GLuint			m_PositionTextureId;
	GLuint			m_NormalTextureId;
	GLuint			m_ColourTextureId;
	GLuint			m_ShadowTextureId;
};
