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


class GlCanvas;


class MainWindow : public wxFrame
{
public:
									MainWindow();

private:
	enum EventId
	{
		EventId_ReloadShaders
	};

	class LogStreambuf : public streambuf
	{
	public:
									LogStreambuf( wxTextCtrl & logWindow );
		virtual						~LogStreambuf();

	private:
		virtual int_type			overflow( int_type intChar );
		virtual streamsize			xsputn( const char * pString, streamsize numChars );

		wxTextCtrl &				m_LogWindow;
		streambuf &					m_OldStreambuf;
	};

	void							OnOpen( wxCommandEvent & event );
	void							OnClose( wxCommandEvent & event );
	void							OnReloadShaders( wxCommandEvent & event );

	GlCanvas *						m_pGlCanvas;
	unique_ptr< LogStreambuf >		m_pLogStreambuf;

	DECLARE_EVENT_TABLE()

	friend class LogStreambuf;
};


MainWindow & GetMainWindow();
