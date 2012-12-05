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


#include "Asset.hpp"
#include "SceneNode.hpp"


class Mesh;
class Light;
class Texture;
class Material;
class VertexShader;
class ShaderProgram;
class FragmentShader;
class GeometryShader;


class Scene : public AssetCache< Texture >,
			  public AssetCache< VertexShader >,
			  public AssetCache< ShaderProgram >,
			  public AssetCache< FragmentShader >,
			  public AssetCache< GeometryShader >
{
public:
											~Scene();

	void									LoadFromFile( const string & fileName );
	void									ReloadShaders();
	void									Render();

	template< class AssetType >
	std::shared_ptr< AssetType >			GetAsset( const string & fileName );

	std::shared_ptr< ShaderProgram >		m_pGeometryShaderProgram;
	std::shared_ptr< ShaderProgram >		m_pShadowShaderProgram;
	std::shared_ptr< ShaderProgram >		m_pLightingShaderProgram;
	ptr_vector< Material >					m_Materials;
	ptr_vector< Mesh >						m_Meshes;
	ptr_vector< Light >						m_Lights;
	SceneNode								m_RootNode;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
	// TODO: Move asset stuff out into AssetManager?
	template< class AssetType >
	struct AssetFileNameComparator
	{
		bool operator () ( const string & fileName, const AssetType & asset ) const
		{
			return fileName < asset.m_FileName;
		}

		bool operator () ( const AssetType & asset, const string & fileName ) const
		{
			return asset.m_FileName < fileName;
		}
	};

	class AssimpLogger : public Assimp::Logger
	{
	public:
											AssimpLogger();

	private:
		virtual bool						attachStream( Assimp::LogStream * pStream, unsigned int severity )			{ return false; }
		virtual bool						detatchStream( Assimp::LogStream * pStream, unsigned int severity )			{ return false; }

		virtual void						OnDebug( const char * message );
		virtual void						OnError( const char * message );
		virtual void						OnInfo( const char * message );
		virtual void						OnWarn( const char * message );
	};

											Scene();
											
	static void APIENTRY					OnOpenGlDebugMessage( GLenum			source,
																  GLenum			type,
																  GLuint			id,
																  GLenum			severity,
																  GLsizei			length,
																  const GLchar *	pMessageString,
																  GLvoid *			pUserParam );

	template< class AssetType >
	AssetCache< AssetType > & GetAssetCache()
	{
		return * this;
	}

	template< class AssetType >
	typename AssetCache< AssetType >::AssetSet & GetAssetSet()
	{
		return GetAssetCache< AssetType >().m_AssetSet;
	}

	template< class ShaderType >
	void ReloadShadersOfType();

	static void CreateSingleton();
	static void DestroySingleton();

	// TODO: This is hacky. We need to access the global scene object during
	//		its constructor and destructor (and in between). To do this, we set
	//		a static pointer during the constructor. It never gets cleared so
	//		that we can use it during the destructor (and it is left dangling
	//		afterwards). Need to think of a better way of doing this.
	static Scene * m_gpSingleton;

	template< class >
	friend class Asset;

	friend class Application;
	friend Scene & GetScene();
	friend class ShaderProgram;
};


inline Scene & GetScene() { return * Scene::m_gpSingleton; }


template< class AssetType >
std::shared_ptr< AssetType > Scene::GetAsset( const string & fileName )
{
	AssetCache< AssetType >::AssetSet::insert_commit_data insertData;
	const auto insertCheck = GetAssetSet< AssetType >().insert_check( fileName, AssetFileNameComparator< AssetType >(), insertData );

	if( insertCheck.second )
	{
		const std::shared_ptr< AssetType > pAsset( new AssetType( fileName ) );
		GetAssetSet< AssetType >().insert_commit( * pAsset, insertData );
		return pAsset;
	}
	else return insertCheck.first->shared_from_this();
}
