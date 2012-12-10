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

#include "Camera.hpp"

#include "Scene.hpp"
#include "ShaderProgram.hpp"


Camera::Camera()
	// Initialise stuff to zero.
	: m_Yaw		( 0.0f )
	, m_Pitch	( 0.0f )
{
	// Initialise more stuff to zero.
	m_Position.setZero();
	m_ViewMatrix.setIdentity();

	// Construct the initial view matrix. Should just be identity.
	ConstructViewMatrix();

	// Setup a default perspective projection. Set both FOVs to 90. Probably
	// not that useful, should probably be overridden in user code.
	SetInfinitePerspectiveProjection( pi< float >() / 2.0f, pi< float >() / 2.0f, 0.1f );
}


void Camera::SetPerspectiveProjectionCommon( const float horizFov,
											 const float vertFov,
											 const float zNear,
											 const float matrixValue22 )
{
	// Standard perspective projection matrix stuff here. The only difference
	// is how we handle the third row when we do infinite projection. See Eric
	// Lengyel's Gamasutra article "The Mechanics of Robust Stencil Shadows"
	// (http://www.gamasutra.com/view/feature/2942/the_mechanics_of_robust_stencil_.php).

	m_ProjectionMatrix.setIdentity();

	m_ProjectionMatrix( 0, 0 ) = 1.0f / tan( horizFov / 2.0f );
	m_ProjectionMatrix( 1, 1 ) = 1.0f / tan( vertFov / 2.0f );

	m_ProjectionMatrix( 2, 2 ) = matrixValue22;
	m_ProjectionMatrix( 2, 3 ) = -( 1.0f + matrixValue22 ) * zNear;

	m_ProjectionMatrix( 3, 2 ) = 1.0f;
	m_ProjectionMatrix( 3, 3 ) = 0.0f;
}


void Camera::SetPerspectiveProjection( const float horizFov,
									   const float vertFov,
									   const float zNear,
									   const float zFar )
{
	SetPerspectiveProjectionCommon( horizFov, vertFov, zNear, zFar / ( zFar - zNear ) );
}


void Camera::SetInfinitePerspectiveProjection( const float horizFov,
											   const float vertFov,
											   const float zNear )
{
	// As above, the (2, 2) matrix component is usually zFar / (zFar - zNear).
	// In the limit as zFar goes to infinity, this goes to 1 (ie zNear becomes
	// negligible).
	SetPerspectiveProjectionCommon( horizFov, vertFov, zNear, 1.0f );
}


void Camera::Move( const Vector3f & translation )
{
	// Update position and reconstruct the view matrix.
	m_Position += translation;
	ConstructViewMatrix();
}


void Camera::Look( const float yaw, const float pitch )
{
	// Update yaw and pitch. Clamping yaw within 0..2pi maintains precision
	// without loss of generality.
	m_Yaw = fmod( m_Yaw + yaw, 2.0f * pi< float >() );
	m_Pitch += pitch;

	// Clamp pitch so that we can't go upside down.
	const float maxPitch = pi< float >() * 80.0f / 180.0f;

	if( m_Pitch > maxPitch )
		m_Pitch = maxPitch;
	else if( m_Pitch < -maxPitch )
		m_Pitch = -maxPitch;

	// Need to reconstruct the view matrix.
	ConstructViewMatrix();
}


void Camera::ConstructViewMatrix()
{
	// Standard stuff for constructing a translation-rotation matrix.
	m_ViewMatrix( 0, 0 ) = cos( m_Yaw );
	m_ViewMatrix( 0, 1 ) = 0.0f;
	m_ViewMatrix( 0, 2 ) = -sin( m_Yaw );

	m_ViewMatrix( 1, 0 ) = -sin( m_Pitch ) * sin( m_Yaw );
	m_ViewMatrix( 1, 1 ) = cos( m_Pitch );
	m_ViewMatrix( 1, 2 ) = -sin( m_Pitch ) * cos( m_Yaw );

	m_ViewMatrix( 2, 0 ) = cos( m_Pitch ) * sin( m_Yaw );
	m_ViewMatrix( 2, 1 ) = sin( m_Pitch );
	m_ViewMatrix( 2, 2 ) = cos( m_Pitch ) * cos( m_Yaw );

	m_ViewMatrix.translation() = m_ViewMatrix.linear() * ( -m_Position );
}


void Camera::Render()
{
	// Clear the colour and depth buffers before we start drawing. Because we
	// are doing deferred rendering, this also clears all planes of the
	// geometry buffer.
	// TODO: This step should probably be done by the viewport or an
	//		intermediate framebuffer object.
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Set OpenGL matrices.
	glMatrixMode( GL_PROJECTION );
	glLoadMatrixf( m_ProjectionMatrix.data() );

	glMatrixMode( GL_MODELVIEW );
	glLoadMatrixf( m_ViewMatrix.data() );

	// Render objects in the scene.
	GetScene().m_RootNode.Render( m_ViewMatrix );
}


// TODO: Merge this with the regular Render method above.
void Camera::RenderShadowVolumes()
{
	// Clear the shadow buffer.
	glClear( GL_COLOR_BUFFER_BIT );

	// Set OpenGL matrices.
	glMatrixMode( GL_PROJECTION );
	glLoadMatrixf( m_ProjectionMatrix.data() );

	glMatrixMode( GL_MODELVIEW );
	glLoadMatrixf( m_ViewMatrix.data() );

	// Render shadow volumes cast by objects in the scene.
	GetScene().m_RootNode.RenderShadowVolumes( m_ViewMatrix );
}
