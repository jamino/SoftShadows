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
// The Application class deals with initialisation, cleanup and the message
// loop for an app, amongst other things. Most of this functionality is
// inherited from the wxWidgets base class. There is only ever one global
// singleton instance of this class, which is managed by wxWidgets. The only
// extra stuff this class does on top of the base class is to create the main
// window and manage a shared OpenGL context.
///////////////////////////////////////////////////////////////////////////////
#pragma once


///////////////////////////////////////////////////////////////////////////////
// Forward declarations
///////////////////////////////////////////////////////////////////////////////
class MainWindow;


///////////////////////////////////////////////////////////////////////////////
// Application class
///////////////////////////////////////////////////////////////////////////////
class Application : public wxGLApp
{
public:
	// Creates the main OpenGL context and initialises stuff that depends on
	// it. Creating a GL context requires a GL canvas and an active device
	// context, so this is called when a GL canvas draws for the first time.
	// The canvas will be the OpenGL target canvas after exit from this
	// function.
	void						InitGlContext( wxGLCanvas & glCanvas );

	// Sets the GL canvas that will be drawn to when using OpenGL commands.
	// Called by a GL window before it renders.
	void						SetTargetGlCanvas( wxGLCanvas & glCanvas );

private:
	// Called when the app starts. The main thing this needs to do is create
	// the main window.
	virtual bool				OnInit();

	// Called when the app quits. Destroys the global scene singleton. The
	// scene destructor cleans up all the assets that were used.
	virtual int					OnExit();

	unique_ptr< wxGLContext >	m_pGlContext;		// The shared OpenGL context.

	friend MainWindow & GetMainWindow();
};


// This macro provides a global wxGetApp() helper function that returns the
// application singleton, cast to our derived type.
wxDECLARE_APP( Application );
