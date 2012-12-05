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

#include "MainWindow.hpp"

#include "Scene.hpp"
#include "GlCanvas.hpp"


MainWindow::LogStreambuf::LogStreambuf( wxTextCtrl & logWindow )
	: m_LogWindow	( logWindow )
	, m_OldStreambuf( * clog.rdbuf() )
{
	clog.rdbuf( this );
}


MainWindow::LogStreambuf::~LogStreambuf()
{
	clog.rdbuf( & m_OldStreambuf );
}


MainWindow::LogStreambuf::int_type MainWindow::LogStreambuf::overflow( int_type intChar )
{
	const wchar_t characters[2] = { intChar, '\0' };
	m_LogWindow.AppendText( characters[0] );
	OutputDebugString( characters );
	return 0;
}


streamsize MainWindow::LogStreambuf::xsputn( const char * pString, streamsize numChars )
{
	const wxString message( pString, numChars );
	m_LogWindow.AppendText( message );
	OutputDebugString( message.c_str() );
	return numChars;
}


BEGIN_EVENT_TABLE( MainWindow, wxFrame )
	EVT_MENU( wxID_OPEN, MainWindow::OnOpen )
	EVT_MENU( wxID_CLOSE, MainWindow::OnClose )
	EVT_MENU( EventId_ReloadShaders, MainWindow::OnReloadShaders )
END_EVENT_TABLE()


MainWindow::MainWindow()
	: wxFrame( NULL, wxID_ANY, "Soft Shadows", wxDefaultPosition, wxSize( 800, 600 ) )
{
	// Make a menubar
	wxMenuBar * const pMenuBar = new wxMenuBar;

	{
		wxMenu * const pFileMenu = new wxMenu;
		pFileMenu->Append( wxID_OPEN );
		pFileMenu->AppendSeparator();
		pFileMenu->Append( wxID_CLOSE );
		pMenuBar->Append( pFileMenu, wxT( "&File" ) );
	}

	{
		wxMenu * const pSceneMenu = new wxMenu;
		pSceneMenu->Append( EventId_ReloadShaders, wxT( "&Reload Shaders" ) );
		pMenuBar->Append( pSceneMenu, wxT( "&Scene" ) );
	}

	SetMenuBar( pMenuBar );
	CreateStatusBar();

	wxSplitterWindow & mainSplitter = * new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE );
	{
		wxSplitterWindow & objectsPropertiesSplitter = * new wxSplitterWindow( & mainSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE );
		objectsPropertiesSplitter.SetSashGravity( 0.5 );
		{
			wxNotebook & objectsNotebook = * new wxNotebook( & objectsPropertiesSplitter, wxID_ANY );
			wxPropertyGrid & propertyGrid = * new wxPropertyGrid( & objectsPropertiesSplitter );
			objectsPropertiesSplitter.SplitHorizontally( & objectsNotebook, & propertyGrid, GetClientSize().y / 2 );
		}

		wxSplitterWindow & canvasLogSplitter = * new wxSplitterWindow( & mainSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE );
		canvasLogSplitter.SetSashGravity( 1.0 );
		{
			m_pGlCanvas = new GlCanvas( & canvasLogSplitter );
			wxTextCtrl & logWindow = * new wxTextCtrl( & canvasLogSplitter, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL );
			canvasLogSplitter.SplitHorizontally( m_pGlCanvas, & logWindow, ( 75 * GetClientSize().y ) / 100 );

			m_pLogStreambuf.reset( new LogStreambuf( logWindow ) );
		}

		mainSplitter.SplitVertically( & objectsPropertiesSplitter, & canvasLogSplitter, ( 20 * GetClientSize().x ) / 100 );
	}

	Show();
}


void MainWindow::OnOpen( wxCommandEvent & event )
{
	const wxString fileName = wxFileSelector( "Open Scene", wxEmptyString, wxEmptyString, wxEmptyString, "*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST );

	if( ! fileName.IsEmpty() )
	{
		GetScene().LoadFromFile( fileName.ToStdString() );
		m_pGlCanvas->Refresh();
	}
}


void MainWindow::OnClose( wxCommandEvent & event )
{
	Close( true );
}


void MainWindow::OnReloadShaders( wxCommandEvent & event )
{
	GetScene().ReloadShaders();
	m_pGlCanvas->Refresh();
}
