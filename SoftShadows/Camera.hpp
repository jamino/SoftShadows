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
// A Camera controls viewing of a scene. Just like a real cmaera, it has a
// position and orientation relative to the scene, as well as a field-of-view.
// Unlike a real camera, it also has near and far clipping planes, which are
// necessary due to the limited precision of maths on computers. In terms of
// OpenGL, this class encapsulates a view matrix and a projection matrix. It
// controls how objects in 3D space will get mapped to pixels in the 2D
// viewport (eg screen/application window).
///////////////////////////////////////////////////////////////////////////////
#pragma once


///////////////////////////////////////////////////////////////////////////////
// Camera class
///////////////////////////////////////////////////////////////////////////////
class Camera
{
public:
						Camera();

	// Use normal perspective with the given horizontal field-of-view, vertical
	// field-of-view, near clip distance and far clip distance.
	void				SetPerspectiveProjection( const float horizFov,
												  const float vertFov,
												  const float zNear,
												  const float zFar );

	// Use infinite perspective with the given horizontal field-of-view,
	// vertical field-of-view and near clip distance. Don't need a far clip
	// distance as it can project to infinity. You need to use this type of
	// projection when you render shadow volumes that can project to infinity.
	void				SetInfinitePerspectiveProjection( const float horizFov,
														  const float vertFov,
														  const float zNear );

	// Render the scene from this camera's point of view.
	void				Render();

	// Render shadow volumes from this camera's point of view.
	// TODO: I don't like having a second render function for this that is so
	//		similar to the first.
	void				RenderShadowVolumes();

	// Moves the camera by the specified delta. For example, you could use this
	// to move the camera when the user presses the arrow keys or WASD.
	void				Move( const Vector3f & translation );

	// Rotate the camera by the specified yaw and pitch angles (don't need roll
	// in this app). For example, you could use this to rotate the view when
	// the user moves the mouse (ie mouse-look). Values are in radians. Note
	// that a camera's pitch is currently clamped to +/- 80 degrees so that you
	// can't turn upside down.
	void				Look( const float yaw, const float pitch );

	const Affine3f &	ViewMatrix() const									{ return m_ViewMatrix; }
	const Affine3f &	ProjectionMatrix() const							{ return m_ProjectionMatrix; }

	// Eigen needs this so that vectors/matrices are aligned properly in
	// classes that are new'd.
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
	// Reconstructs the view matrix from the current camera position and
	// yaw/pitch. Call this whenever a function changes position/yaw/pitch (eg
	// the Move and Look functions above).
	void				ConstructViewMatrix();

	// Encapsulates code that is common to both of the perspective projection
	// functions above.
	void				SetPerspectiveProjectionCommon( const float horizFov,
														const float vertFov,
														const float zNear,
														const float matrixValue22 );

	Affine3f			m_ViewMatrix;
	Affine3f			m_ProjectionMatrix;
	Vector3f			m_Position;
	float				m_Yaw;
	float				m_Pitch;
};
