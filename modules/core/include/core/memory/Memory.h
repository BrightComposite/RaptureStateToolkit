//---------------------------------------------------------------------------

#ifndef MEMORY_H
#define MEMORY_H

//---------------------------------------------------------------------------

#include <core/meta/Types.h>

#include <string.h>
#include <stdlib.h>
#include <memory>
#include <malloc.h>

#undef min
#undef max

#define _new(size) ::operator new(size)
#define _delete(ptr) ::operator delete(ptr)
#define _new_nothrow(size, nt) ::operator new(size, nt)
#define _delete_nothrow(ptr, nt) ::operator delete(ptr, nt)

#define _newarray(size) ::operator new[](size)
#define _deletearray(ptr) ::operator delete[](ptr)
#define _newarray_nothrow(size, nt) ::operator new[](size, nt)
#define _deletearray_nothrow(ptr, nt) ::operator delete[](ptr, nt)

#define _malloc(type, size) reinterpret_cast<type *>(::malloc(size * sizeof(type)))
#define _free(ptr) ::free(ptr)
#define _realloc(type, ptr, size) reinterpret_cast<type *>(::realloc(ptr, size * sizeof(type)))
#define _memmove(type, to, from, size) ::memmove(to, from, size * sizeof(type));
#define _memcopy(type, to, from, size) ::memcpy(to, from, size * sizeof(type));
#define _memset(type, ptr, value, size) ::memset(ptr, value, size * sizeof(type));

#if defined(WIN32)
	#define _malloc_size(ptr) _msize(ptr)
#else
	#define _malloc_size(ptr) malloc_size(ptr)
#endif

//---------------------------------------------------------------------------

	/**
	 *	@brief
	 *	This class is just a memory helper, you can use standart functions instead of this
	 *	but Memory<wchar_t>::move() will always be more comfortable... :)
	 */

namespace Rapture
{
	using std::nothrow_t;
	using std::unique_ptr;

	template<typename T = byte>
	class Memory
    {
	public:
		static inline T * allocate(size_t size)
		{
			return _malloc(T, size);
		}

		static inline void free(const T * ptr)
		{
			_free(const_cast<T *>(ptr));
		}

		static inline T * reallocate(T * ptr, size_t size)
		{
			if(ptr == nullptr)
				return allocate(size);

			return _realloc(T, ptr, size);
		}

		static inline void move(T * to, const T * from, size_t size)
		{
			_memmove(T, to, from, size);
		}

		template<size_t N>
		static inline void move(T * to, const T(&from)[N])
		{
			move(to, from, N);
		}

		static inline T * copy(const T * src, size_t size)
		{
			T * buffer = allocate(size);
			_memcopy(T, buffer, src, size);

			return buffer;
		}

		static inline void fill(T * ptr, byte value, size_t size)
		{
			_memset(T, ptr, value, size);
		}
    };

	template<>
	class Memory<void>
	{
	public:
		static inline void * allocate(size_t size)
		{
			return malloc(size);
		}

		static inline void free(const void * ptr)
		{
			free(const_cast<void *>(ptr));
		}

		static inline void * reallocate(void * ptr, size_t size)
		{
			return realloc(ptr, size);
		}

		static inline void move(void * to, const void * from, size_t size)
		{
			memmove(to, from, size);
		}

		template<class T, size_t N>
		static inline void move(void * to, const T(&from)[N])
		{
			move(to, from, N * sizeof(T));
		}

		static inline void * copy(const void * src, size_t size)
		{
			void * buffer = allocate(size);
			move(buffer, src, size);

			return buffer;
		}

		static inline void fill(void * ptr, byte value, size_t size)
		{
			memset(ptr, value, size);
		}
	};
}

//---------------------------------------------------------------------------
#endif