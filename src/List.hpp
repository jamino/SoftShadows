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


template< class ItemTypeParam, class ListTypeParam = void, class Tag = void >
class List
{
public:
	class Item;

	typedef typename conditional< std::is_void< ItemTypeParam >::value, Item, ItemTypeParam >::type ItemType;
	typedef typename conditional< std::is_void< ListTypeParam >::value, List, ListTypeParam >::type ListType;

	class Item
	{
	public:
						Item()		: m_pList( nullptr ), m_pNext( nullptr ), m_pPrev( nullptr )	{}
						~Item()																		{ Remove(); }

		ItemType *		Next() const																{ return m_pNext; }
		ItemType *		Prev() const																{ return m_pPrev; }
		ListType *		GetList() const																{ return m_pList; }

		bool			IsInAList() const															{ return ( GetList() != nullptr ); }

		void			InsertAfter( ItemType & item );
		void			InsertBefore( ItemType & item );

		void			Remove();

	private:
		ItemType *		m_pNext;
		ItemType *		m_pPrev;
		ListType *		m_pList;

		friend class List;
	};

					List()		: m_Size( 0 ), m_pHead( nullptr ), m_pTail( nullptr )		{}
					~List()																	{ Clear(); }

	ItemType *		Head() const															{ return m_pHead; }
	ItemType *		Tail() const															{ return m_pTail; }
	size_t			Size() const															{ return m_Size; }

	bool			IsEmpty() const															{ return ( Size() == 0 ); }
	void			Clear()																	{ while( ! IsEmpty() ) Head()->Remove(); }

	void			AddHead( ItemType & item );
	void			AddTail( ItemType & item );

private:
	size_t			m_Size;
	ItemType *		m_pHead;
	ItemType *		m_pTail;
};


template< class ItemTypeParam, class ListTypeParam, class Tag >
void List< ItemTypeParam, ListTypeParam, Tag >::Item::InsertAfter( ItemType & item )
{
	if( IsInAList() && ( & item != this ) )
	{
		item.Remove();

		item.m_pNext = m_pNext;
		item.m_pPrev = static_cast< ItemType * >( this );
		item.m_pList = m_pList;

		if( m_pNext != nullptr )
			m_pNext->m_pPrev = & item;
		else
			m_pList->m_pTail = & item;

		m_pNext = & item;
		++m_pList->m_Size;
	}
}


template< class ItemTypeParam, class ListTypeParam, class Tag >
void List< ItemTypeParam, ListTypeParam, Tag >::Item::InsertBefore( ItemType & item )
{
	if( IsInAList() && ( & item != this ) )
	{
		item.Remove();

		item.m_pNext = static_cast< ItemType * >( this );
		item.m_pPrev = m_pPrev;
		item.m_pList = m_pList;

		if( m_pPrev != nullptr )
			m_pPrev->m_pNext = & item;
		else
			m_pList->m_pHead = & item;

		m_pPrev = & item;
		++m_pList->m_Size;
	}
}


template< class ItemTypeParam, class ListTypeParam, class Tag >
void List< ItemTypeParam, ListTypeParam, Tag >::Item::Remove()
{
	if( IsInAList() )
	{
		if( m_pNext != nullptr )
			m_pNext->m_pPrev = m_pPrev;
		else
			m_pList->m_pTail = m_pPrev;

		if( m_pPrev != nullptr )
			m_pPrev->m_pNext = m_pNext;
		else
			m_pList->m_pHead = m_pNext;

		--m_pList->m_Size;

		m_pNext = nullptr;
		m_pPrev = nullptr;
		m_pList = nullptr;
	}
}


template< class ItemTypeParam, class ListTypeParam, class Tag >
void List< ItemTypeParam, ListTypeParam, Tag >::AddHead( ItemType & item )
{
	item.Remove();

	if( m_pHead != nullptr )
		m_pHead->m_pPrev = & item;
	else
		m_pTail = & item;

	item.m_pList = static_cast< ListType * >( this );
	item.m_pNext = m_pHead;

	++m_Size;
	m_pHead = & item;
}


template< class ItemTypeParam, class ListTypeParam, class Tag >
void List< ItemTypeParam, ListTypeParam, Tag >::AddTail( ItemType & item )
{
	item.Remove();

	if( m_pTail != nullptr )
		m_pTail->m_pNext = & item;
	else
		m_pHead = & item;

	item.m_pList = static_cast< ListType * >( this );
	item.m_pPrev = m_pTail;

	++m_Size;
	m_pTail = & item;
}
