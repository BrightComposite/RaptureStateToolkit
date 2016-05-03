//---------------------------------------------------------------------------

#ifndef LIST_H
#define LIST_H

//---------------------------------------------------------------------------

#include <core/Handle.h>
#include <list>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::list;

	template<typename T, class ... OwnerAttr>
	class List : public list<Handle<T, OwnerAttr...>>
	{
	public:
		using list<Handle<T, OwnerAttr...>>::list;
	};

	template<typename T>
	class PointerList : public list<T *>
	{
	public:
		using list<T *>::list;
	};
}

//---------------------------------------------------------------------------
#endif
