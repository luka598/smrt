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

// --------------------------
// | Class Base declaration |
// --------------------------
class Base {
public:
  // Constructor & Destructor
  virtual ~Base();
  Base(){};
  Base(const Base &other) = delete;
  Base &operator=(const Base &other) = delete;

  // Pointers
  template <typename T> Ptr<T> sptr();

  // Ref managing
  void acquire();
  void release();
  int refCount();

private:
  // Ref managing
  int _refCount = 0;
  bool acquired = false;
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
  Ptr(const Ptr &other);
  Ptr &operator=(const Ptr &other) = delete; // TODO

  // Pointers
  template <typename U> Ptr<U> as();
  Ptr<T> cpy();
  T *rptr() const;

  // Operators
  T *operator->() const;

private:
  T *_ptr;
};

// --------------------------
// | Class Base definitions |
// --------------------------
inline Base::~Base() {}
template <typename T> inline Ptr<T> Base::sptr() { return Ptr<T>(this); }
inline void Base::acquire() {
  if (_refCount == 0 && acquired)
    throw std::runtime_error("Trying to acquire invalid Base");
  _refCount++;
  acquired = true;
}
inline void Base::release() {
  if (_refCount == 0)
    throw std::runtime_error("Refcount decremented bellow 0");
  if (!acquired)
    throw std::runtime_error("Trying to release unacquired Base");
  _refCount--;
}
inline int Base::refCount() { return _refCount; }

// ----------------------------
// | Class Ptr<T> definitions |
// ----------------------------
template <typename T> Ptr<T>::Ptr(T *ptr) {
  if (ptr == nullptr)
    throw std::runtime_error("Got nullptr");
  ptr->acquire();
  _ptr = ptr;
}
template <typename T> Ptr<T>::Ptr(const Ptr &other) {
  T *ptr = other.rptr();
  if (ptr == nullptr)
    throw std::runtime_error("Got nullptr");
  ptr->acquire();
  _ptr = ptr;
};

template <typename T> Ptr<T>::~Ptr() {
  _ptr->release();

  if (_ptr->refCount() == 0)
    delete _ptr;
}
template <typename T> template <typename U> Ptr<U> Ptr<T>::as() {
  static_assert(is_derived_from<Base, U>::value, "Invalid type provided");
  U *p = dynamic_cast<U *>(_ptr);
  if (p == nullptr)
    throw std::runtime_error("Can't cast to that type");
  return Ptr<U>(p);
}
template <typename T> Ptr<T> Ptr<T>::cpy() { return Ptr<T>(_ptr); }
template <typename T> T *Ptr<T>::rptr() const { return _ptr; }
template <typename T> T *Ptr<T>::operator->() const { return _ptr; }

// -----------
// | Helpers |
// -----------
template <typename T, typename... Args> Ptr<T> make(Args... args) {
  static_assert(is_derived_from<Base, T>::value, "Invalid type provided");
  return Ptr<T>(new T(args...));
}

template <typename T, typename... Args> basePtr make_base(Args... args) {
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
