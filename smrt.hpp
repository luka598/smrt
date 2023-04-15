/*
 * Copyright 2023 luka598 <luka598@tuta.io>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <stdexcept>
#include <type_traits>

namespace smrt {

	// ------------------------------------------------------
	// | Incomplete class declarations and type definitions |
	// ------------------------------------------------------
	class Base;
	template <typename T> class Ptr;
	typedef Ptr<Base> basePtr;
	template <typename T, typename... Args> Ptr<T> make(Args... args);
	template <typename T, typename... Args> basePtr make_base(Args... args);
	template <typename T, typename U, typename... Args>
		Ptr<T> make_as(Args... args);
	template <typename BaseT, typename DerivedT> struct is_derived_from;
	template <template <typename...> class Template, typename T>
		struct is_specialization_of;
	template <template <typename...> class Template, typename... Args>
		struct is_specialization_of<Template, Template<Args...>>;

	// ------------------------------------
	// | Struct is_derived_from definition |
	// -------------------------------------
	template <typename BaseT, typename DerivedT> struct is_derived_from {
		private:
			static constexpr bool check(const BaseT *) { return true; }
			static constexpr bool check(...) { return false; }

		public:
			static constexpr bool value = check(static_cast<const DerivedT *>(nullptr));
	};

	// -------------------------------
	// | Struct is_sepicalization_of |
	// -------------------------------
	template <template <typename...> class Template, typename T>
		struct is_specialization_of : std::false_type {};

	template <template <typename...> class Template, typename... Args>
		struct is_specialization_of<Template, Template<Args...>> : std::true_type {};

	// --------------------------
	// | Class Base declaration |
	// --------------------------
	class Base {
		public:
			// Constructor & Destructor
			virtual ~Base();

			// Pointers
			template <typename T> Ptr<T> sptr();
			Ptr<Base> bptr();
			Base *rptr();

			// Ref managing
			void acquire();
			void release();
			int refCount();

		private:
			// Ref managing
			int _refCount = 0;
	};

	// --------------------------
	// | Class Ptr<T> declaration |
	// --------------------------
	template <typename T> class Ptr {
		static_assert(is_derived_from<Base, T>::value, "Invalid type provided");

		public:
		// Constructor & Destructor
		Ptr(T *_ptr);
		~Ptr();

		// Pointers
		template <typename U, std::enable_if_t<is_derived_from<Base, U>::value, bool> = true>
			Ptr<U> as();
		template <typename U, std::enable_if_t<is_specialization_of<Ptr, U>::value, bool> = true>
			U as();
		Ptr<T> cpy();
		T *rptr();

		// ???
		T *operator->();
		typedef T Type;

		private:
		T *_ptr;
	};

	// --------------------------
	// | Class Base definitions |
	// --------------------------
	inline Base::~Base() {}
	template <typename T> inline Ptr<T> Base::sptr() { return Ptr<T>(this); }
	inline Ptr<Base> Base::bptr() { return Ptr<Base>(this); }
	inline Base *Base::rptr() { return this; }
	inline void Base::acquire() { _refCount++; }
	inline void Base::release() { _refCount--; }
	inline int Base::refCount() { return _refCount; }

	// ----------------------------
	// | Class Ptr<T> definitions |
	// ----------------------------
	template <typename T>
		Ptr<T>::Ptr(T *ptr) {
			if (ptr == nullptr)
				throw std::runtime_error("Got nullptr");
			ptr->acquire();
			_ptr = ptr;
		}
	template <typename T>
		Ptr<T>::~Ptr() {
			_ptr->release();
			if (_ptr->refCount() == 0)
				delete _ptr;
		}
	template <typename T>
		template <typename U, std::enable_if_t<is_derived_from<Base, U>::value, bool>>
		Ptr<U> Ptr<T>::as() {
			static_assert(is_derived_from<Base, U>::value, "Invalid type provided");
			U *p = dynamic_cast<U *>(_ptr);
			if (p == nullptr)
				throw std::runtime_error("Can't cast to that type");
			return Ptr<U>(p);
		}

	template <typename T>
		template <typename U, std::enable_if_t<is_specialization_of<Ptr, U>::value, bool>>
		U Ptr<T>::as() {
			typename U::Type *p = dynamic_cast<typename U::Type *>(_ptr);
			if (p == nullptr)
				throw std::runtime_error("Can't cast to that type");
			return U(p);
		}
	template <typename T> Ptr<T> Ptr<T>::cpy() { return Ptr<T>(_ptr); }
	template <typename T> T *Ptr<T>::rptr() { return _ptr; }
	template <typename T> T *Ptr<T>::operator->() { return _ptr; }

	// -----------
	// | Helpers |
	// -----------
	template <typename T, typename... Args>
		Ptr<T> make(Args... args) {
			static_assert(is_derived_from<Base, T>::value, "Invalid type provided");
			return Ptr<T>(new T(args...));
		}

	template <typename T, typename... Args>
		basePtr make_base(Args... args) {
			static_assert(is_derived_from<Base, T>::value, "Invalid type provided");
			return Ptr<Base>(new T(args...));
		}

	template <typename T, typename U, typename... Args>
		Ptr<T> make_as(Args... args) {
			static_assert(is_derived_from<Base, T>::value, "Invalid type provided");
			static_assert(is_derived_from<Base, U>::value, "Invalid type provided");
			return Ptr<T>(new U(args...));
		}
} // namespace smrt
#define SMRT_CLASS(name)                                                       \
	class name##Class;                                                           \
	typedef smrt::Ptr<name##Class> name;                                         \
	class name##Class : public smrt::Base

#define SMRT_MAKE(name) smrt::make<name##Class>
