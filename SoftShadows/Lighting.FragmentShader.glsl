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


// GBuffer textures that contain surface properties.
uniform sampler2DRect diffuseTexture;
uniform sampler2DRect positionTexture;
uniform sampler2DRect normalTexture;

// The shadow texture records how much of the light can be seen from each
// fragment. 0 corresponds to 100% shadow, 1 corresponds to 0% shadow. This
// allows us to do eg fragmentColour = shadow * surfaceColour.
// TODO: Rename this. The name "shadow" could imply that the numbers should be
//		the other way around.
uniform sampler2DRect shadowTexture;


void main()
{
	// Pull surface info from the GBuffer textures.
	vec3 position = texture( positionTexture, gl_FragCoord.xy ).xyz;
	vec3 normal = 2.0 * texture( normalTexture, gl_FragCoord.xy ).xyz - 1.0;
	vec3 diffuseColour = texture( diffuseTexture, gl_FragCoord.xy ).rgb;

	// Get the shadow scalar from the shadow texture.
	float shadow = texture( shadowTexture, gl_FragCoord.xy ).r;

	vec3 directionToLight = normalize( gl_LightSource[0].position.xyz - position );

	// Simple diffuse lighting.
	// TODO: Fold ambient into the shadow scalar. This will allow us to do
	//		approximate ambient occlusion in the shadow shader.
	// TODO: Ambient should be configurable for each light (ie it should be a
	//		shader uniform).
	// TODO: Specular.
	float ambientScalar = 0.1;
	float diffuseScalar = max( dot( directionToLight, normal ), 0.0 );

	gl_FragColor.rgb = diffuseColour;
	gl_FragColor.rgb *= ambientScalar + ( 1.0 - ambientScalar ) * shadow * diffuseScalar;
}
