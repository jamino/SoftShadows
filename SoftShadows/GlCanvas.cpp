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

#include "GlCanvas.hpp"

#include "Scene.hpp"
#include "Application.hpp"


BEGIN_EVENT_TABLE( GlCanvas, wxGLCanvas )
    EVT_KEY_UP( GlCanvas::OnKeyUp )
	EVT_LEFT_UP( GlCanvas::OnMouseUp )
    EVT_KEY_DOWN( GlCanvas::OnKeyDown )
	EVT_LEFT_DOWN( GlCanvas::OnMouseDown )
	EVT_KILL_FOCUS( GlCanvas::OnLoseFocus )
	EVT_TIMER( wxID_ANY, GlCanvas::OnTimer )
END_EVENT_TABLE()


const int GlCanvas::m_gGlAttributes[] =
{
	WX_GL_RGBA,
	WX_GL_DOUBLEBUFFER,
	WX_GL_DEPTH_SIZE,		32,
	WX_GL_STENCIL_SIZE,		8
};


GlCanvas::GlCanvas( wxWindow * pParent )
    : wxGLCanvas	( pParent, wxID_ANY, m_gGlAttributes, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE )
	, m_UpdateTimer	( this )
	, m_NumKeysDown	( 0 )
{
	Bind( wxEVT_PAINT, & GlCanvas::OnFirstPaint, this );
}


void GlCanvas::OnLoseFocus( wxFocusEvent & event )
{
	m_NumKeysDown = 0;
	m_UpdateTimer.Stop();
	m_KeyDownBits.reset();
	Unbind( wxEVT_MOTION, & GlCanvas::OnMouseLook, this );
}


void GlCanvas::RenderImmediate()
{
	wxClientDC deviceContext( this );
	Render();
}


void GlCanvas::OnSize( wxSizeEvent & event )
{
	wxClientDC deviceContext( this );
	wxGetApp().SetTargetGlCanvas( * this );

	m_pViewport->OnSize( event.GetSize().GetWidth(), event.GetSize().GetHeight() );
}


void GlCanvas::Render()
{
	wxGetApp().SetTargetGlCanvas( * this );
	m_pViewport->Render();
	SwapBuffers();
}


void GlCanvas::OnFirstPaint( wxPaintEvent & event )
{
	Unbind( wxEVT_PAINT, & GlCanvas::OnFirstPaint, this );
	wxPaintDC deviceContext( this );

	wxGetApp().InitGlContext( * this );

	const wxSize clientSize = GetClientSize();
	m_pViewport.reset( new Viewport( clientSize.x, clientSize.y ) );

	Bind( wxEVT_SIZE, & GlCanvas::OnSize, this );
	Bind( wxEVT_PAINT, & GlCanvas::OnPaint, this );

	Render();
}


void GlCanvas::OnPaint( wxPaintEvent & event )
{
	wxPaintDC deviceContext( this );
	Render();
}


void GlCanvas::OnKeyDown( wxKeyEvent & event )
{
	switch( event.GetKeyCode() )
	{
	case MoveKey_Forwards:
	case MoveKey_Backwards:
	case MoveKey_Left:
	case MoveKey_Right:
	case MoveKey_Up:
	case MoveKey_Down:
		if( ! m_KeyDownBits[ event.GetKeyCode() ] )
		{
			++m_NumKeysDown;
			m_KeyDownBits.set( event.GetKeyCode() );

			if( ! m_UpdateTimer.IsRunning() )
			{
				m_UpdateTimer.Start( 16 );
				m_UpdateWatch.Start();
			}
		}
		break;

	case 'R':
		if( event.ControlDown() )
		{
			GetScene().ReloadShaders();
			Refresh();
		}
	}
}


void GlCanvas::OnKeyUp( wxKeyEvent & event )
{
	switch( event.GetKeyCode() )
	{
	case MoveKey_Forwards:
	case MoveKey_Backwards:
	case MoveKey_Left:
	case MoveKey_Right:
	case MoveKey_Up:
	case MoveKey_Down:
		if( m_KeyDownBits[ event.GetKeyCode() ] )
		{
			m_KeyDownBits.reset( event.GetKeyCode() );

			if( m_NumKeysDown > 0 )
			{
				--m_NumKeysDown;

				if( m_NumKeysDown == 0 )
				{
					m_UpdateTimer.Stop();
					m_UpdateWatch.Pause();
				}
			}
		}
		break;
	}
}


void GlCanvas::OnMouseDown( wxMouseEvent & event )
{
	event.Skip();
	m_LastMousePos = wxGetMousePosition();
	Bind( wxEVT_MOTION, & GlCanvas::OnMouseLook, this );
}


void GlCanvas::OnMouseUp( wxMouseEvent & event )
{
	Unbind( wxEVT_MOTION, & GlCanvas::OnMouseLook, this );
}


void GlCanvas::OnMouseLook( wxMouseEvent & event )
{
	const wxPoint delta = wxGetMousePosition() - m_LastMousePos;
	m_LastMousePos = wxGetMousePosition();
	m_pViewport->m_Camera.Look( delta.x * pi< float >() / 180.0f, delta.y * pi< float >() / 180.0f );

	RenderImmediate();
}


void GlCanvas::OnTimer( wxTimerEvent & event )
{
	const float frameLength = static_cast< float >( m_UpdateWatch.TimeInMicro().GetValue() ) / 1e6f;

	m_UpdateWatch.Start();

	Vector3f translation = Vector3f::Zero();

	if( m_KeyDownBits[ MoveKey_Left ] )
		translation.x() -= 1.0f;

	if( m_KeyDownBits[ MoveKey_Right ] )
		translation.x() += 1.0f;

	if( m_KeyDownBits[ MoveKey_Forwards ] )
		translation.z() += 1.0f;

	if( m_KeyDownBits[ MoveKey_Backwards ] )
		translation.z() -= 1.0f;

	translation = m_pViewport->m_Camera.ViewMatrix().linear().transpose() * translation;

	if( m_KeyDownBits[ MoveKey_Up ] )
		translation.y() += 1.0f;

	if( m_KeyDownBits[ MoveKey_Down ] )
		translation.y() -= 1.0f;

	m_pViewport->m_Camera.Move( 10.0f * frameLength * translation );

	RenderImmediate();
}
