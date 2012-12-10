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

#version 150 compatibility

//#version 420
//#extension GL_ARB_shading_language_420pack: enable //Use for GLSL versions before 420.

//layout( binding = 0 ) uniform sampler2DRect diffuseTexture;


uniform ivec2 viewport;
uniform sampler2DRect positionTexture;


//in vec4 shadowPlanes[4];


in vec4 fragCoord;
in vec4 shadowPlane;
//in mat2x4 penumbraMatrix[3]; // First column = shadow direction, second column = shadow normal.
in vec4 penumbraDirection[3];
in vec4 penumbraNormal[3];
in float penumbraTangent[3];


in vec4 penumbraPlane[3];


void main()
{
	vec2 windowPos = 0.5 * viewport * ( vec2( 1.0 ) + fragCoord.xy / fragCoord.w );
	vec4 position = vec4( texture( positionTexture, windowPos ).xyz, 1.0 );
	float shadow = step( 0.0, dot( position, shadowPlane ) );

#if 1
	for( int shadowPlaneIndex = 0; shadowPlaneIndex != 3; ++shadowPlaneIndex )
	{
		//shadow = max( shadow, step( 0.0, dot( position, penumbraNormal[ shadowPlaneIndex ] ) ) );

		//vec2 dists = position * penumbraMatrix[ shadowPlaneIndex ];

		float penumbraDist = dot( position, penumbraNormal[ shadowPlaneIndex ] );
		float shadowDist = abs( dot( position, penumbraDirection[ shadowPlaneIndex ] ) );
		float penumbraWidth = shadowDist * penumbraTangent[ shadowPlaneIndex ];

#if 1
		if( penumbraDist >= penumbraWidth )
			shadow = 1.0;
		else if( penumbraDist > -penumbraWidth )
			//shadow = 0.5;
			//shadow = clamp( penumbraDist / penumbraWidth, 0.0, 1.0 );
			shadow = max( shadow, 0.5 + 0.5 * penumbraDist / penumbraWidth );
			//shadow = max( shadow, penumbraDist / penumbraWidth );
#else
		if( penumbraDist > -penumbraWidth )
			//shadow = clamp( 2.0 * penumbraDist, 0.0, 1.0 );
			shadow =  clamp( 0.5 + 0.5 * penumbraDist / penumbraWidth, 0.0, 1.0 );
#endif
	}
#endif

	//gl_FragColor.r = shadow;
	gl_FragColor.r = shadow * shadow * ( 3.0 - 2.0 * shadow );
	//gl_FragColor.r = smoothstep( 0.0, 1.0, shadow );
}
