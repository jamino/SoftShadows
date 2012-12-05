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

#include "Application.hpp"

#include "Scene.hpp"
#include "MainWindow.hpp"

#include <Magick++.h>


// wxWidgets needs this to create the application singleton.
wxIMPLEMENT_APP( Application );


bool Application::OnInit()
{
	if( ! wxApp::OnInit() )
		return false;

	// Initialise GraphicsMagick. Every app that uses GraphicsMagick needs
	// this.
	Magick::InitializeMagick( nullptr );

	// Optimisation: tell wxWidgets to only generate idle messages for those
	// controls that explicitly request it.
	wxIdleEvent::SetMode( wxIDLE_PROCESS_SPECIFIED );

	// Create the main window.
	SetTopWindow( new MainWindow );

	return true;
}


int Application::OnExit()
{
	// Destroy the global scene singleton when the application goes down. This
	// deallocates meshes, textures, shaders, materials and other types of asset.
	Scene::DestroySingleton();

	return wxApp::OnExit();
}


// Creates the main OpenGL context and initialises stuff that depends on it.
// The canvas will be the OpenGL target canvas after exit from this function.
void Application::InitGlContext( wxGLCanvas & glCanvas )
{
	// Create the shared OpenGL context if we haven't done so already.
	if( ! m_pGlContext )
	{
		// Create the context.
		m_pGlContext.reset( new wxGLContext( & glCanvas ) );

		// Set the canvas as the current OpenGL target canvas.
		// TODO: I'm assuming that any OpenGL calls made in the functions below
		//		need this, but is this really necessary?
		SetTargetGlCanvas( glCanvas );

		// Now that OpenGL is up we can init glew.
		glewInit();

		// And now we have glew we can create the global scene singleton as
		// well.
		Scene::CreateSingleton();
	}
	else
		// Ensure the canvas is always the current OpenGL target canvas on exit
		// from this function.
		SetTargetGlCanvas( glCanvas );
}


// Sets the GL canvas that will be drawn to when using OpenGL commands. Called
// by a GL window before it renders.
void Application::SetTargetGlCanvas( wxGLCanvas & glCanvas )
{
	m_pGlContext->SetCurrent( glCanvas );
}
