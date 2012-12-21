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
// This file is used to generate the precompiled header in Visual Studio. I
// also use it to tell the compiler what libraries to link with. I feel this is
// better than hiding it away in a compiler specific project file. The hope is
// to make this as platform/compiler independent as possible.
//
// TODO: Test this on different platforms/compilers and fix as necessary. It's
//		probably a bit specific to my setup.
///////////////////////////////////////////////////////////////////////////////

#include "Precomp.hpp"


#pragma comment( lib, "opengl32.lib" )


#ifdef _DEBUG
#	pragma comment( lib, "glew32sd.lib" )
#	pragma comment( lib, "CORE_DB_bzlib_.lib" )
#	pragma comment( lib, "CORE_DB_coders_.lib" )
#	pragma comment( lib, "CORE_DB_filters_.lib" )
#	pragma comment( lib, "CORE_DB_jbig_.lib" )
#	pragma comment( lib, "CORE_DB_jp2_.lib" )
#	pragma comment( lib, "CORE_DB_jpeg_.lib" )
#	pragma comment( lib, "CORE_DB_lcms_.lib" )
#	pragma comment( lib, "CORE_DB_libxml_.lib" )
#	pragma comment( lib, "CORE_DB_magick_.lib" )
#	pragma comment( lib, "CORE_DB_Magick++_.lib" )
#	pragma comment( lib, "CORE_DB_png_.lib" )
#	pragma comment( lib, "CORE_DB_tiff_.lib" )
#	pragma comment( lib, "CORE_DB_ttf_.lib" )
#	pragma comment( lib, "CORE_DB_wand_.lib" )
#	pragma comment( lib, "CORE_DB_xlib_.lib" )
#	pragma comment( lib, "CORE_DB_zlib_.lib" )
#	pragma comment( lib, "CORE_DB_wmf_.lib" )
#	pragma comment( lib, "assimp_debug_Win32/assimp.lib" )
#else
#	pragma comment( lib, "glew32s.lib" )
#	pragma comment( lib, "CORE_RL_bzlib_.lib" )
#	pragma comment( lib, "CORE_RL_coders_.lib" )
#	pragma comment( lib, "CORE_RL_filters_.lib" )
#	pragma comment( lib, "CORE_RL_jbig_.lib" )
#	pragma comment( lib, "CORE_RL_jp2_.lib" )
#	pragma comment( lib, "CORE_RL_jpeg_.lib" )
#	pragma comment( lib, "CORE_RL_lcms_.lib" )
#	pragma comment( lib, "CORE_RL_libxml_.lib" )
#	pragma comment( lib, "CORE_RL_magick_.lib" )
#	pragma comment( lib, "CORE_RL_Magick++_.lib" )
#	pragma comment( lib, "CORE_RL_png_.lib" )
#	pragma comment( lib, "CORE_RL_tiff_.lib" )
#	pragma comment( lib, "CORE_RL_ttf_.lib" )
#	pragma comment( lib, "CORE_RL_wand_.lib" )
#	pragma comment( lib, "CORE_RL_xlib_.lib" )
#	pragma comment( lib, "CORE_RL_zlib_.lib" )
#	pragma comment( lib, "CORE_RL_wmf_.lib" )
#	pragma comment( lib, "assimp_release_Win32/assimp.lib" )
#endif
