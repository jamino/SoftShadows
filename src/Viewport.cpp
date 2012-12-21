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

#include "Viewport.hpp"

#include "Scene.hpp"
#include "ShaderProgram.hpp"


Viewport::Viewport( const GLint width, const GLint height )
{
	glGenFramebuffers( 1, & m_GeometryFramebufferId );
	glGenFramebuffers( 1, & m_ShadowFramebufferId );
	glGenRenderbuffers( 1, & m_DepthRenderBufferId );
	glGenTextures( 1, & m_PositionTextureId );
	glGenTextures( 1, & m_NormalTextureId );
	glGenTextures( 1, & m_ColourTextureId );
	glGenTextures( 1, & m_ShadowTextureId );

	OnSize( width, height );

	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_GeometryFramebufferId );
	{
		glFramebufferRenderbuffer( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthRenderBufferId );
		glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, m_PositionTextureId, 0 );
		glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, m_NormalTextureId, 0 );
		glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_RECTANGLE, m_ColourTextureId, 0 );

		const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers( sizeof( drawBuffers ) / sizeof( drawBuffers[0] ), drawBuffers );
	}

	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_ShadowFramebufferId );
	{
		glFramebufferRenderbuffer( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthRenderBufferId );
		glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, m_ShadowTextureId, 0 );

		glDrawBuffer( GL_COLOR_ATTACHMENT0 );
	}

	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
}


void Viewport::OnSize( const GLint width, const GLint height )
{
	glViewport( 0, 0, width, height );

	glBindRenderbuffer( GL_RENDERBUFFER, m_DepthRenderBufferId );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, width, height );
	glBindRenderbuffer( GL_RENDERBUFFER, 0 );

	glBindTexture( GL_TEXTURE_RECTANGLE, m_ColourTextureId );
	glTexImage2D( GL_TEXTURE_RECTANGLE, 0, GL_RGB8, width, height, 0, GL_RGB, GL_BYTE, nullptr );

	glBindTexture( GL_TEXTURE_RECTANGLE, m_PositionTextureId );
	glTexImage2D( GL_TEXTURE_RECTANGLE, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_BYTE, nullptr );

	// TODO: I wanted to use GL_RGB8_SNORM for the normal texture format (8-bit
	//		fixed point between -1 and 1), but the nVidia GTX 275 in my desktop
	//		(with a graphics driver from 2012) doesn't seem to support it as a
	//		framebuffer attachment. The ATI HD 5650 in my laptop (witb a
	//		graphics driver from 2012) copes fine.
	glBindTexture( GL_TEXTURE_RECTANGLE, m_NormalTextureId );
	glTexImage2D( GL_TEXTURE_RECTANGLE, 0, GL_RGB8, width, height, 0, GL_RGB, GL_BYTE, nullptr );

	glBindTexture( GL_TEXTURE_RECTANGLE, m_ShadowTextureId );
	glTexImage2D( GL_TEXTURE_RECTANGLE, 0, GL_R16, width, height, 0, GL_RGB, GL_BYTE, nullptr );

	glBindTexture( GL_TEXTURE_RECTANGLE, 0 );

	const float horizFov = pi< float >() / 2.0f;

	m_Camera.SetInfinitePerspectiveProjection( horizFov, 2.0f * atan( tan( horizFov / 2.0f ) * height / width ), 0.1f );
}


void Viewport::Render()
{
	// Render positions/colours/normals into a geometry buffer for later use in
	// shadowing/lighting (aka deferred shading).
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_GeometryFramebufferId );

	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LESS );
	glEnable( GL_DEPTH_TEST );
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

	glDisable( GL_CULL_FACE );

	m_Camera.Render();

	glEnable( GL_CULL_FACE );

	// For each light.

	// Render shadow volumes. The shader limits the proportion of light that
	// each occluded pixel receives.

	// Only process pixels that are in front of back-facing shadow faces.
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_ShadowFramebufferId );
	GetScene().m_pShadowShaderProgram->SetCurrent();

	glCullFace( GL_FRONT );
	glDepthMask( GL_FALSE );
	glDepthFunc( GL_GREATER );

	//glDisable( GL_DEPTH_TEST );

	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );

	glEnable( GL_BLEND );
	glBlendEquation( GL_MIN );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_RECTANGLE, m_PositionTextureId );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	const Vector4f lightPos = m_Camera.ViewMatrix() * Vector4f( 0.0f, 0.0f, -100.0f, 1.0f );
	glLightfv( GL_LIGHT0, GL_POSITION, lightPos.data() );

	GLint dimensions[4];
	//glGetFloati_v( GL_VIEWPORT, 0, dimensions.data() );
	glGetIntegerv( GL_VIEWPORT, dimensions );
	glUniform2iv( glGetUniformLocation( GetScene().m_pShadowShaderProgram->GetID(), "viewport" ), 1, & dimensions[2] );

	m_Camera.RenderShadowVolumes();

	glDisable( GL_BLEND );

	// Lighting pass.
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
	GetScene().m_pLightingShaderProgram->SetCurrent();

	glCullFace( GL_BACK );
	glDisable( GL_DEPTH_TEST );

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );

	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_RECTANGLE, m_NormalTextureId );

	glActiveTexture( GL_TEXTURE2 );
	glBindTexture( GL_TEXTURE_RECTANGLE, m_ColourTextureId );

	glActiveTexture( GL_TEXTURE3 );
	glBindTexture( GL_TEXTURE_RECTANGLE, m_ShadowTextureId );

	glBegin( GL_TRIANGLE_STRIP );
	{
		glVertex3f( -1.0f,  1.0f, 0.0f );
		glVertex3f(  1.0f,  1.0f, 0.0f );
		glVertex3f( -1.0f, -1.0f, 0.0f );
		glVertex3f(  1.0f, -1.0f, 0.0f );
	}
	glEnd();

	glActiveTexture( GL_TEXTURE3 );
	glBindTexture( GL_TEXTURE_RECTANGLE, 0 );

	glActiveTexture( GL_TEXTURE2 );
	glBindTexture( GL_TEXTURE_RECTANGLE, 0 );

	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_RECTANGLE, 0 );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_RECTANGLE, 0 );

	// Next light.

	glFinish();
}
