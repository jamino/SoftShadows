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

#include "Scene.hpp"

#include "Mesh.hpp"
#include "Light.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "ShaderProgram.hpp"


namespace
{
	const unsigned int ImportFlags = aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded;
}


Scene * Scene::m_gpSingleton = nullptr;


Scene::AssimpLogger::AssimpLogger()
	: Assimp::Logger( VERBOSE )
{}


void Scene::AssimpLogger::OnDebug( const char * message )
{
	clog << message << endl;
}


void Scene::AssimpLogger::OnError( const char * message )
{
	clog << message << endl;
}


void Scene::AssimpLogger::OnInfo( const char * message )
{
	clog << message << endl;
}


void Scene::AssimpLogger::OnWarn( const char * message )
{
	clog << message << endl;
}


void APIENTRY Scene::OnOpenGlDebugMessage( GLenum			source,
										   GLenum			type,
										   GLuint			id,
										   GLenum			severity,
										   GLsizei			length,
										   const GLchar *	pMessageString,
										   GLvoid *			pUserParam )
{
	clog << pMessageString << endl;
}


Scene::Scene()
{
	// TODO: This is hacky.
	m_gpSingleton = this;

	//glDebugMessageCallback( & OnOpenGlDebugMessage, nullptr );
	//glEnable( GL_DEBUG_OUTPUT );

	Assimp::DefaultLogger::set( new AssimpLogger );

	m_pGeometryShaderProgram.reset( new ShaderProgram( "GeometryShaderProgram", GetAsset< VertexShader >( "GBuffer.VertexShader.glsl" ), GetAsset< FragmentShader >( "GBuffer.FragmentShader.glsl" ) ) );
	m_pShadowShaderProgram.reset( new ShaderProgram( "ShadowShaderProgram", GetAsset< VertexShader >( "Shadow.VertexShader.glsl" ), GetAsset< FragmentShader >( "Shadow.FragmentShader.glsl" ), GetAsset< GeometryShader >( "Shadow.GeometryShader.glsl" ) ) );
	m_pLightingShaderProgram.reset( new ShaderProgram( "LightingShaderProgram", GetAsset< VertexShader >( "Lighting.VertexShader.glsl" ), GetAsset< FragmentShader >( "Lighting.FragmentShader.glsl" ) ) );

	m_pShadowShaderProgram->SetCurrent();
	glUniform1i( glGetUniformLocation( m_pShadowShaderProgram->GetID(), "positionTexture" ), 0 );

	m_pLightingShaderProgram->SetCurrent();
	glUniform1i( glGetUniformLocation( m_pLightingShaderProgram->GetID(), "positionTexture" ), 0 );
	glUniform1i( glGetUniformLocation( m_pLightingShaderProgram->GetID(), "normalTexture" ), 1 );
	glUniform1i( glGetUniformLocation( m_pLightingShaderProgram->GetID(), "diffuseTexture" ), 2 );
	glUniform1i( glGetUniformLocation( m_pLightingShaderProgram->GetID(), "shadowTexture" ), 3 );

	glUseProgram( 0 );

	glEnable( GL_CULL_FACE );
	glFrontFace( GL_CW );
}


Scene::~Scene()
{}


void Scene::LoadFromFile( const string & fileName )
{
	Assimp::Importer assimpImporter;
	const aiScene & assimpScene = * assimpImporter.ReadFile( fileName, ImportFlags );

	if( & assimpScene == nullptr )
		throw runtime_error( "Error loading scene from " + fileName );

	const size_t baseMaterialIndex = m_Materials.size();
	const filesystem::path scenePath = filesystem::path( fileName ).parent_path();

	for( unsigned materialIndex = 0; materialIndex < assimpScene.mNumMaterials; ++materialIndex )
		m_Materials.push_back( new Material( * assimpScene.mMaterials[ materialIndex ], scenePath ) );

	const size_t baseMeshIndex = m_Meshes.size();

	for( unsigned meshIndex = 0; meshIndex < assimpScene.mNumMeshes; ++meshIndex )
		m_Meshes.push_back( new Mesh( * assimpScene.mMeshes[ meshIndex ], baseMaterialIndex ) );

	for( unsigned lightIndex = 0; lightIndex < assimpScene.mNumLights; ++lightIndex )
		m_Lights.push_back( new Light( * assimpScene.mLights[ lightIndex ] ) );

	m_RootNode.m_ChildNodes.push_back( new SceneNode( * assimpScene.mRootNode, baseMeshIndex ) );
}


/*void Scene::Render()
{
	m_RootNode.Render();
}*/


template< class ShaderType >
void Scene::ReloadShadersOfType()
{
	foreach( ShaderType & shader, GetAssetSet< ShaderType >() )
		shader.Load();
}


void Scene::ReloadShaders()
{
	glUseProgram( 0 );

	ReloadShadersOfType< VertexShader >();
	ReloadShadersOfType< GeometryShader >();
	ReloadShadersOfType< FragmentShader >();

	foreach( ShaderProgram & program, GetAssetSet< ShaderProgram >() )
		program.Link();

	m_pShadowShaderProgram->SetCurrent();
	glUniform1i( glGetUniformLocation( m_pShadowShaderProgram->GetID(), "positionTexture" ), 0 );

	m_pLightingShaderProgram->SetCurrent();
	glUniform1i( glGetUniformLocation( m_pLightingShaderProgram->GetID(), "positionTexture" ), 0 );
	glUniform1i( glGetUniformLocation( m_pLightingShaderProgram->GetID(), "normalTexture" ), 1 );
	glUniform1i( glGetUniformLocation( m_pLightingShaderProgram->GetID(), "diffuseTexture" ), 2 );
	glUniform1i( glGetUniformLocation( m_pLightingShaderProgram->GetID(), "shadowTexture" ), 3 );
}


void Scene::CreateSingleton()
{
	assert( m_gpSingleton == nullptr );
	new Scene;
}


void Scene::DestroySingleton()
{
	if( m_gpSingleton != nullptr )
	{
		delete m_gpSingleton;
		m_gpSingleton = nullptr;
	}
}
