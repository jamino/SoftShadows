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


layout( triangles ) in;
layout( triangle_strip, max_vertices = 10 ) out;


// The fragment shader works out whether pixels are in shadow by clipping the
// position against the planes of the shadow volume. We don't need a plane for
// the far cap as nothing can be beyond that. However, we need to output the
// planes each time we emit a vertex in order to have access to the planes at
// any interpolated position. The planes are calculated once and cached in
// shadowPlanesCache. Every time we emit a vertex in EmitShadowVert the planes
// get copied into the shadowPlanes output variable.


//vec4 shadowPlanesCache[4];
//out vec4 shadowPlanes[4];


vec4 shadowPlaneCache;
//mat2x4 penumbraMatrixCache[3]; // First column = shadow direction, second column = shadow normal.
vec4 penumbraDirectionCache[3];
vec4 penumbraNormalCache[3];
float penumbraTangentCache[3];


out vec4 fragCoord;
out vec4 shadowPlane;
//out mat2x4 penumbraMatrix[3]; // First column = shadow direction, second column = shadow normal.
out vec4 penumbraDirection[3];
out vec4 penumbraNormal[3];
out float penumbraTangent[3];


void EmitShadowVertex( vec4 position )
{
	for( int shadowPlaneIndex = 0; shadowPlaneIndex != 3; ++shadowPlaneIndex )
	{
		//shadowPlanes[ shadowPlaneIndex ] = shadowPlanesCache[ shadowPlaneIndex ];
		//penumbraMatrix[ shadowPlaneIndex ] = penumbraMatrixCache[ shadowPlaneIndex ];
		penumbraDirection[ shadowPlaneIndex ] = penumbraDirectionCache[ shadowPlaneIndex ];
		penumbraNormal[ shadowPlaneIndex ] = penumbraNormalCache[ shadowPlaneIndex ];
		penumbraTangent[ shadowPlaneIndex ] = penumbraTangentCache[ shadowPlaneIndex ];
	}

	fragCoord = position;
	shadowPlane = shadowPlaneCache;
	gl_Position = position;

	EmitVertex();
}


void main()
{
	const float lightRadius = 10.0;
	vec3 lightPos = gl_LightSource[0].position.xyz;

	// Cache vectors for each edge.
	vec3 edges[3];

	for( int vertNumber = 0; vertNumber != 3; ++vertNumber )
	{
		edges[ vertNumber ] = gl_in[ ( vertNumber + 1 ) % 3 ].gl_Position.xyz - gl_in[ vertNumber ].gl_Position.xyz;
	}

	// Cache the triangle normal.
	vec3 triNormal = normalize( cross( edges[0], edges[1] ) );

	float lightDistance = dot( gl_in[0].gl_Position.xyz - lightPos, triNormal );
	float facingSign = sign( lightDistance );

	if( facingSign >= 0.0 )
		return;

	// Cache the plane equation corresponding to the original triangle.
	shadowPlaneCache.xyz = triNormal;
	shadowPlaneCache.w = -dot( gl_in[ 0 ].gl_Position.xyz, triNormal ) + 1.0;

	// Clamp the radius of the light so that it doesn't go "through" the plane
	// of the triangle. This prevents weird effects like penumbral wedges that
	// go "up" towards the light and bad maths calls like sqrt(-1).
	float clampedLightRadius = clamp( abs( lightDistance ) - 0.9 * lightRadius, 0.0, lightRadius );

	// Calculate the outer face of each penumbral wedge and cache the normals.
	vec3 outerNormal[3];

	for( int vertNumber = 0; vertNumber != 3; ++vertNumber )
	{
		vec3 vertPos = gl_in[ vertNumber ].gl_Position.xyz;
		vec3 lightToVert = vertPos - lightPos;

		// This is the normal of the inner hard/dark face of the penumbral
		// wedge (ie the face that would be used for regular stencil shadows).
		vec3 innerNormal = normalize( cross( lightToVert, edges[ vertNumber ] ) );

		float sinAngleSqr = clampedLightRadius * clampedLightRadius / dot( lightToVert, lightToVert );
		float cosAngle = sqrt( 1.0 - sinAngleSqr );

		// This is the first point on the sphere light that will be occluded as
		// you enter the penumbra from the un-shadowed side. The outer face of
		// the penumbral wedge is projected from this point.
		vec3 farLightPos = lightPos + sinAngleSqr * lightToVert - clampedLightRadius * cosAngle * innerNormal;

		outerNormal[ vertNumber ] = normalize( cross( vertPos - farLightPos, edges[ vertNumber ] ) );

		// Calculate penumbral wedge info.
		penumbraNormalCache[ vertNumber ].xyz = normalize( innerNormal + outerNormal[ vertNumber ] );
		//penumbraNormalCache[ vertNumber ].xyz = outerNormal[ vertNumber ];
		//penumbraNormalCache[ vertNumber ].xyz = innerNormal;
		penumbraNormalCache[ vertNumber ].w = -dot( vertPos, penumbraNormalCache[ vertNumber ].xyz );

		penumbraDirectionCache[ vertNumber ].xyz = cross( edges[ vertNumber ], penumbraNormalCache[ vertNumber ].xyz );
		penumbraDirectionCache[ vertNumber ].w = -dot( vertPos, penumbraDirectionCache[ vertNumber ].xyz );

		// TODO: This should be faster, but it doesn't work. Why not??!!
		/*cosAngle = dot( innerNormal, penumbraNormalCache[ vertNumber ].xyz );
		float sinAngle = dot( innerNormal, penumbraDirectionCache[ vertNumber ].xyz );
		penumbraTangentCache[ vertNumber ] = sinAngle / cosAngle;*/

		// Backup plan for calculating the tangent.
		vec3 foo = cross( edges[ vertNumber ], outerNormal[ vertNumber ] );
		penumbraTangentCache[ vertNumber ] = dot( foo, penumbraNormalCache[ vertNumber ].xyz );
		penumbraTangentCache[ vertNumber ] /= dot( foo, penumbraDirectionCache[ vertNumber ].xyz );
	}

	vec4 extrudedShadowVerts[3];

	for( int vertNumber = 0; vertNumber != 3; ++vertNumber )
	{
		// Extrude each vertex of the triangle to the far clip plane.
		//extrudedShadowVerts[ vertNumber ].xyz = normalize( gl_in[ vertNumber ].gl_Position.xyz - gl_LightSource[0].position.xyz );
		extrudedShadowVerts[ vertNumber ].xyz = normalize( cross( outerNormal[ vertNumber ], outerNormal[ ( vertNumber + 2 ) % 3 ] ) );
		extrudedShadowVerts[ vertNumber ].w = 0.0;

		// Cache the plane equation for each extruded edge.
		//shadowPlanesCache[ vertNumber ].xyz = normalize( cross( extrudedShadowVerts[ vertNumber ].xyz, edges[ vertNumber ] ) );
		//shadowPlanesCache[ vertNumber ].w = -dot( gl_in[ vertNumber ].gl_Position.xyz, shadowPlanesCache[ vertNumber ].xyz );

		// Transform the extruded verts into clip space.
		extrudedShadowVerts[ vertNumber ] = gl_ProjectionMatrix * extrudedShadowVerts[ vertNumber ];
	}

	// Emit the original triangle as is. This is the front cap of the shadow
	// volume.
	for( int vertNumber = 0; vertNumber != 3; ++vertNumber )
	{
		EmitShadowVertex( gl_ProjectionMatrix * gl_in[ vertNumber ].gl_Position );
	}

	// Continue the triangle strip around the extruded edges. This saves us
	// from having to emit multiple primitives (we just use the one triangle
	// strip). Using one strip also means duplicating fewer verts. Start with
	// extruding half of the last edge.
	EmitShadowVertex( extrudedShadowVerts[2] );

	// Extrude all of the first edge.
	EmitShadowVertex( gl_ProjectionMatrix * gl_in[0].gl_Position );
	EmitShadowVertex( extrudedShadowVerts[0] );

	// Extrude all of the second edge.
	EmitShadowVertex( gl_ProjectionMatrix * gl_in[1].gl_Position );
	EmitShadowVertex( extrudedShadowVerts[1] );

	// Now finish the other half of the last edge.
	EmitShadowVertex( extrudedShadowVerts[2] );

	// And finally emit a triangle that caps the volume at the far clip plane.
	EmitShadowVertex( extrudedShadowVerts[0] );

	// Done.
	EndPrimitive();
}
