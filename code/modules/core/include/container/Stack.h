//---------------------------------------------------------------------------

#pragma once

#ifndef STACK_H
#define STACK_H

//---------------------------------------------------------------------------

#include <core/Handle.h>
#include <container/Container.h>

#include <stack>

//---------------------------------------------------------------------------

namespace asd
{
	using std::stack;

	template<typename T, class ... Owner>
	class Stack : public stack<Handle<T, Owner...>>
	{
	public:
		using stack<Handle<T, Owner...>>::stack;
	};
}

//---------------------------------------------------------------------------
#endif