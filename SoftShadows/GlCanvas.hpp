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
// A GlCanvas is a window in wxWidgets that can be drawn to using OpenGL. This
// class extends the base wxWidgets class with some app specific features for
// moving the camera with the WASD keys and doing mouse-look.
//
// TODO: De-couple movement code from this class.
// TODO: Always use the 60Hz timer whenever we want to redraw? Eg currently,
//		mouse-look and OnPaint use immediate rendering, which can block other
//		movement.
// TODO: Sometimes mouse buttons and movement keys when the app loses focus.
//		Not sure why this is yet.
///////////////////////////////////////////////////////////////////////////////

#pragma once


#include "Viewport.hpp"


///////////////////////////////////////////////////////////////////////////////
// GlCanvas class
///////////////////////////////////////////////////////////////////////////////
class GlCanvas : public wxGLCanvas
{
public:
								GlCanvas( wxWindow * pParent );

	// Renders the scene now without waiting for a paint message. Useful for
	// updating the view after the camera changes to prevent input lag.
	// TODO: Remove this and only ever render in the 60Hz timer callback? Would
	//		probably need to add a RequestRedraw/Invalidate and a 'dirty' bit
	//		if so.
	void						RenderImmediate();

	// Eigen needs this so that vectors/matrices are aligned properly in
	// classes that are new'd.
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
	// Keys that the user can press to move the camera around.
	// TODO: Should be user configurable.
	enum
	{
		MoveKey_Forwards	= 'W',
		MoveKey_Backwards	= 'S',
		MoveKey_Left		= 'A',
		MoveKey_Right		= 'D',
		MoveKey_Up			= WXK_SPACE,
		MoveKey_Down		= WXK_SHIFT
	};

	// Called when the window is resized. Need to resize the geometry buffer
	// when this happens as it needs to have the same resolution as the window.
	void						OnSize( wxSizeEvent & event );

	// Called when the contents of the window need to be redrawn (eg because
	// the window was moved off screen and then back on screen again). For us
	// this means rendering the scene.
	void						OnPaint( wxPaintEvent & event );

	// Called whenever the main update timer reaches zero (or at the earliest
	// opportunity after that). We use this timer to try and maintain 60
	// frames-per-second when the user is moving the camera. This ensures that
	// the app is responsive without hogging more CPU time than necessary
	// (humans generally can't perceive more than 60Hz plus it is equal to the
	// refresh rate of most modern LCD monitors, so it's not worth drawing any
	// quicker). Movement code compensates for variable frame-rate so it
	// doesn't matter if it drops below 60. If the user is not moving the
	// camera then the scene is static and we don't use the timer or re-draw
	// the scene (ie the app is idle and uses no CPU time; you should be able
	// to see this in eg Windows Task Manager).
	// TODO: It can be nice to see the unrestricted maximum frame-rate. This is
	//		sometimes used as a vague measure of performance. If nothing else,
	//		if the app can update substantially quicker than 60Hz then you can
	//		probably throw more polys around.
	void						OnTimer( wxTimerEvent & event );

	// Called when the window loses focus (eg because the user clicked in a
	// different window). We need to stop tracking user input so reset all key
	// states to 'up' (ie not pressed).
	void						OnLoseFocus( wxFocusEvent & event );

	// Called when the canvas is drawn for the first time. We can only use
	// OpenGL after we create a wxGLContext. However, the GL context can only
	// be created from an existing wxGLCanvas that has acquired a device
	// context (see the wxWidgets documentation/examples). Hence we create the
	// GL context during this function (if it hasn't already been created by
	// another canvas).
	void						OnFirstPaint( wxPaintEvent & event );

	// Called when the user presses/releases a key whilst this window has input
	// focus. We maintain a cache of what keys are currently pressed and refer
	// to it when we update camera movement once every 60th of a second (or
	// less frequently depending on frame-rate). Hence these functions update
	// the value in the cache for the key that was pressed/released.
	void						OnKeyDown( wxKeyEvent & event );
	void						OnKeyUp( wxKeyEvent & event );

	// Called whenever the uses clicks/releases the left mouse button in the
	// window. Holding down the left mouse button enables mouse-look.
	void						OnMouseUp( wxMouseEvent & event );
	void						OnMouseDown( wxMouseEvent & event );

	// Called whenever the user moves the mouse and mouse-look is enabled.
	// Updates the rotation of the camera depending on how the mouse was moved.
	void						OnMouseLook( wxMouseEvent & event );

	// Renders the scene to this window using the camera associated with the
	// viewport.
	// IMPORTANT: You MUST acquire a device context before calling this (ie
	//		either a wxClientDC or a wxPaintDC). That's why this function is
	//		private. Generally, if you want to render from outside of a paint
	//		event, use RenderImmediate instead.
	void						Render();

	// These next 2 lines use Boost funkiness to get the highest value movement
	// key as a compile time constant, which can then be used as the size for
	// an array. The array is used to track what movement keys are currently
	// pressed. Bit of an experiment on my part to see if I could make the
	// array as big as necessary, but no bigger, without having to hard-code
	// it.
	// TODO: Clever, but complicated, plus allowing users to reconfigure the
	//		controls will require a different solution.
	typedef mpl::vector_c< int, MoveKey_Forwards, MoveKey_Backwards, MoveKey_Left, MoveKey_Right, MoveKey_Up, MoveKey_Down > KeyCodes;

	static const int MaxKeyCode = mpl::deref< mpl::max_element< KeyCodes >::type >::type::value;

	unique_ptr< Viewport >		m_pViewport;		// The viewport associated with this canvas.
	wxTimer						m_UpdateTimer;		// The 60Hz timer used to update movement and redraw the scene.
	wxStopWatch					m_UpdateWatch;		// We use this to check the length of the 60Hz timer event as we might get it late depending on system load.
	bitset< MaxKeyCode + 1 >	m_KeyDownBits;		// Records what movement keys are currently held down.
	wxPoint						m_LastMousePos;		// Records the position of the mouse the last time we updated the camera. We use this to work out mow much it has moved.
	int							m_NumKeysDown;		// The number of movement keys that are currently held down. When this is zero we may not need to update the camera/redraw.

	// This array specifies the OpenGL related attributes used when creating a
	// GL canvas (eg bits-per-pixel, depth bits, stencil bits, etc.). They are
	// the same for every GL canvas in this app, hence why it's a static
	// constant.
	static const int			m_gGlAttributes[];

	DECLARE_EVENT_TABLE()
};
