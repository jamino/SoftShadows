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
// An Asset is an object wrapper for some kind of data that is most often
// loaded from a file. The classes here provide a hnady way to cache assets by
// file name. The asset is loaded from the file once when it is first
// requested; subsequent requests of the same file will return the asset stored
// in the cache. Assets are intended to be used with shared_ptrs: when an asset
// is no longer used (ie there aren't any shared_ptrs to it) it is
// automatically removed from the cache and deallocated. Assets of type X
// should derive from Asset<X>. The Scene class will also need to derive from
// AssetCache<X> as all asset management is currently controlled centrally via
// the global Scene singleton object.
// TODO: This method requires a little too much coupling between the Scene
//		class and asset types. However, there are reasons for this (see
//		Scene.hpp). Is there a better way?
// TODO: The Scene class currently needs privileged access to the AssetCache
//		class. Should try and move the majority of that code into the
//		AssetCache class itself.
///////////////////////////////////////////////////////////////////////////////

#pragma once


///////////////////////////////////////////////////////////////////////////////
// AssetBase class
// Common base type for all asset types. Don't derive from this class directly,
// derive from the Asset class template below instead.
///////////////////////////////////////////////////////////////////////////////
class AssetBase : public intrusive::set_base_hook<>
{
public:
	const string		m_FileName;

private:
	// Revoked.
	AssetBase();
	AssetBase( const AssetBase & copy );

	AssetBase( const string & fileName )
		: m_FileName( fileName )
	{}

	template< class >
	friend class Asset;
};


///////////////////////////////////////////////////////////////////////////////
// Asset class template
// Base class for specific asset types. Assets of type X should derive from
// Asset<X>.
///////////////////////////////////////////////////////////////////////////////
template< class AssetType >
class Asset : public AssetBase,
			  public std::enable_shared_from_this< AssetType >
{
protected:
	Asset( const string & fileName )
		: AssetBase( fileName )
	{}

public:
	~Asset()
	{
		// Remove assets from the cache automatically when they are deallocated
		// (if they are in the cache).
		if( is_linked() )
			GetScene().GetAssetSet< AssetType >().erase( GetScene().GetAssetSet< AssetType >().iterator_to( * static_cast< AssetType * >( this ) ) );
	}
};


///////////////////////////////////////////////////////////////////////////////
// AssetCache class template
// Caches assets of a specific type by file name. Assets are only loaded from a
// file when there is a cache miss.
///////////////////////////////////////////////////////////////////////////////
template< class AssetType >
class AssetCache
{
protected:
	// Comparison function object used for ordering items in the cache. Just
	// uses the standard string less than operator on the assets' file names.
	struct Comparator
	{
		bool operator () ( const AssetType & left, const AssetType & right ) const
		{
			return left.m_FileName < right.m_FileName;
		}
	};

	// The cache is implemented as a boost::intrusive::set (ie a red-black tree
	// at time of writing). An intrusive implementation is used over std::set
	// so that assets can remove themselves quickly from the set when they are
	// destroyed without having to do an extra O(log n) lookup. Getting an
	// iterator to an item in an intrusive container is guaranteed to take
	// constant time; getting an iterator to an item in a std::set generally
	// takes O(log n) time. See the boost::intrusive documentation for more
	// details. You may argue that it's not a huge win, but I like it :)
	typedef intrusive::base_hook< intrusive::set_base_hook<> >							SetBaseHook;
	typedef intrusive::set< AssetType, SetBaseHook, intrusive::compare< Comparator > >	AssetSet;

	AssetSet			m_AssetSet;

	// The global Scene object currently needs priveleged access to the set.
	friend class Scene;
};
