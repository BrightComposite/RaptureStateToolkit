//---------------------------------------------------------------------------

#ifndef HASH_H
#define HASH_H

//---------------------------------------------------------------------------

#include <core/meta/Meta.h>
#include <core/String.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	template<class T, bool isPod = std::is_pod<T>::value>
	class Hashed : public T
	{
		size_t _hashValue;

	public:
		template<typename ... A,
			require(
				can_construct(T, A...)
				)>
		Hashed(A &&... args) : T(forward<A>(args)...), _hashValue(Rapture::hash(static_cast<const T &>(*this))) {}

		Hashed(const Hashed & val) : T(val), _hashValue(val._hashValue) {}
		Hashed(Hashed && val) : T(forward<Hashed>(val)), _hashValue(val._hashValue) {}

		Hashed & operator = (const Hashed & val)
		{
			T::operator = (val);
			_hashValue = val._hashValue;

			return *this;
		}

		Hashed & operator = (Hashed && val)
		{
			T::operator = (forward<Hashed>(val));
			_hashValue = val._hashValue;

			return *this;
		}

		template<class A,
			require(
				not_based_on(A, Hashed)
				)>
		Hashed & operator = (A && val)
		{
			T::operator = (forward<A>(val));
			_hashValue = Rapture::hash(static_cast<T &>(*this));

			return *this;
		}

		bool operator == (const Hashed & val) const
		{
			return _hashValue == val._hashValue && Rapture::equal_to(static_cast<const T &>(*this), static_cast<const T &>(val));
		}

		size_t hash() const
		{
			return _hashValue;
		}
	};

	template<class T>
	class Hashed<T, true>
	{
		size_t _hashValue;
		T _inner;

	public:
		Hashed() : _inner(), _hashValue(Rapture::hash(_inner)) {}
		Hashed(T inner) : _inner(inner), _hashValue(Rapture::hash(_inner)) {}
		Hashed(const Hashed & val) : _inner(val._inner), _hashValue(val._hashValue) {}

		Hashed & operator = (const Hashed & val)
		{
			_inner = val._inner;
			_hashValue = val._hashValue;

			return *this;
		}

		Hashed & operator = (const T & val)
		{
			_inner = val;
			_hashValue = Rapture::hash(_inner);

			return *this;
		}

		bool operator == (const Hashed & val) const
		{
			return _hashValue == val._hashValue && Rapture::equal_to(_inner, val._inner);
		}

		operator T()
		{
			return _inner;
		}

		operator const T() const
		{
			return _inner;
		}

		size_t hash() const
		{
			return _hashValue;
		}
	};

	template<class T>
	class Hashed<Hashed<T>, false> : public Hashed<T>
	{
	public:
		using Hashed<T>::Hashed;

		template<class A>
		Hashed & operator = (A && val)
		{
			Hashed<T>::operator = (forward<A>(val));
			return *this;
		}
	};

	typedef Hashed<void *> PtrId;

	inline bool equal_to(const char * first, const char * second)
	{
		return strcmp(first, second) == 0;
	}

	inline bool equal_to(const Object & first, const Object & second)
	{
		return first.compare(&second) == 0;
	}

	template<class X, class Y,
		require(
			not_based_on(X, Object) &&
			not_same(X, const char *)
			)>
	inline bool equal_to(const X & first, const Y & second)
	{
		return first == second;
	}

	template<class X, class Y>
	inline size_t mixhash(const X & first, const Y & second)
	{
		return hash(first) ^ hash(first);
	}

	template<class X, class Y>
	inline void mixhash(const X & first, const Y & second, size_t & result)
	{
		result = hash(first) ^ hash(first);
	}

	struct AutoIdentifier
	{
		bool operator == (const AutoIdentifier & val) const
		{
			return this == &val;
		}

		bool operator < (const AutoIdentifier & val) const
		{
			return this < &val;
		}

		bool operator > (const AutoIdentifier & val) const
		{
			return this > &val;
		}

		size_t hash() const
		{
			return reinterpret_cast<size_t>(this);
		}
	};

	class SharedIdentifier : public EmptyHandle
	{
	public:
		SharedIdentifier() : EmptyHandle(emptiness) {}
	};

	template<class T>
	size_t hash(const T & value,
		require_p(
			is_functor<std::hash<T>, const T &>::value
			))
	{
		return std::hash<T>()(value);
	}
	
	template<class T>
	class StaticIdentifier
	{
	public:
		static PtrId & id()
		{
			static PtrId _id(StaticIdentifier::id);
			return _id;
		}
	};
}

namespace std
{
	template<class T>
	struct hash<Rapture::Hashed<T>> : unary_function<Rapture::Hashed<T>, size_t>
	{
		size_t operator()(const Rapture::Hashed<T> & val) const
		{
			return val.hash();
		}
	};

	template<>
	struct hash<Rapture::AutoIdentifier> : unary_function<Rapture::AutoIdentifier, size_t>
	{
		size_t operator()(const Rapture::AutoIdentifier & val) const
		{
			return val.hash();
		}
	};

	template<class T>
	struct hash<Rapture::Handle<T>> : unary_function<Rapture::Handle<T>, size_t>
	{
		template<
			require(
				Rapture::is_functor<hash<T>, const T &>::value
				)>
		size_t operator()(const Rapture::Handle<T> & val) const
		{
			return val == nullptr ? 0 : hash<T>()(*val);
		}
	};
}

//---------------------------------------------------------------------------
#endif