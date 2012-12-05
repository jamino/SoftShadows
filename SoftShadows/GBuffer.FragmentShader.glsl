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

//#version 420
//#extension GL_ARB_shading_language_420pack: enable //Use for GLSL versions before 420.
#version 150 compatibility

//layout( binding = 0 ) uniform sampler2D diffuseTexture;

uniform sampler2D diffuseTexture;


in vec3 normal;
in vec4 viewPos;

//in vec3 colour;


void main()
{
	gl_FragData[0].xyz = viewPos.xyz;
	gl_FragData[1].xyz = normalize( normal );
	//gl_FragData[2].rgb = colour;
	//gl_FragData[2].rgb = vec3( 1.0, 0.0, 0.0 );
	gl_FragData[2].rgb = texture( diffuseTexture, gl_TexCoord[0].xy ).rgb;
}
