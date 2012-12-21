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
//
// A viewport specifies a rectangular region of pixels in a window that OpenGL
// will draw to. In general, a viewport can be any sub-rectangle of a window,
// but in this app the intention is for the viewport to fill the window. The
// Viewport class also manages extra framebuffers and textures needed to do
// deferred shading.
//
// TODO: Currently, each viewport maintains separate framebuffers and textures
//		for deferred shading. It may be better to have a single, global set of
//		these and share them between viewports. The global
//		framebuffers/textures would have to be as big as the biggest viewport
//		for this to work; smaller viewports would render into a smaller region.
//		The advantage is it would use less GPU memory overall when there are
//		several viewports in use.
///////////////////////////////////////////////////////////////////////////////

#pragma once


#include "Camera.hpp"


///////////////////////////////////////////////////////////////////////////////
// Viewport class
///////////////////////////////////////////////////////////////////////////////
class Viewport
{
public:
					Viewport( const GLint width, const GLint height );

	// Call this to resize the viewport, eg when the window is resized. The
	// width and height are in pixels.
	void			OnSize( const GLint width, const GLint height );

	// Render the current view from the attached camera into this viewport.
	void			Render();

	// The camera whose view is rendered into this viewport.
	Camera			m_Camera;

	// Eigen needs this so that vectors/matrices are aligned properly in
	// classes that are new'd.
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
	// The geometry framebuffer renders position, normal and colour into
	// respective textures for later use in deferred shading.
	GLuint			m_GeometryFramebufferId;

	// The shadow framebuffer renders a scalar between 0 and 1 into a texture
	// indicating how dark the shadow is at each fragment. The texture is
	// re-used later during deferred shading
	GLuint			m_ShadowFramebufferId;

	// Have to use a custom depth buffer when rendering to the geometry buffer.
	GLuint			m_DepthRenderBufferId;

	// The following textures are render targets that will store the position,
	// normal, colour and shadow at each fragment location. They are all
	// referenced during deferred shading.
	GLuint			m_PositionTextureId;
	GLuint			m_NormalTextureId;
	GLuint			m_ColourTextureId;
	GLuint			m_ShadowTextureId;
};
