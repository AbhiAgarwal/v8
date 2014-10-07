// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_BASE_FUNCTIONAL_H_
#define V8_BASE_FUNCTIONAL_H_

#include <cstddef>
#include <functional>
#include <utility>

#include "include/v8stdint.h"
#include "src/base/macros.h"

namespace v8 {
namespace base {

// base::hash is an implementation of the hash function object specified by
// C++11. It was designed to be compatible with std::hash (in C++11) and
// boost:hash (which in turn is based on the hash function object specified by
// the Draft Technical Report on C++ Library Extensions (TR1)).
//
// base::hash is implemented by calling the hash_value function. The namespace
// isn't specified so that it can detect overloads via argument dependant
// lookup. So if there is a free function hash_value in the same namespace as a
// custom type, it will get called.
//
// If users are asked to implement a hash function for their own types with no
// guidance, they generally write bad hash functions. Instead, we provide  a
// simple function base::hash_combine to pass hash-relevant member variables
// into, in order to define a decent hash function. base::hash_combine is
// declared as:
//
//   template<typename T, typename... Ts>
//   size_t hash_combine(const T& v, const Ts& ...vs);
//
// Consider the following example:
//
//   namespace v8 {
//   namespace bar {
//     struct Point { int x; int y; };
//     size_t hash_value(Point const& p) {
//       return base::hash_combine(p.x, p.y);
//     }
//   }
//
//   namespace foo {
//     void DoSomeWork(bar::Point const& p) {
//       base::hash<bar::Point> h;
//       ...
//       size_t hash_code = h(p);  // calls bar::hash_value(Point const&)
//       ...
//     }
//   }
//   }
//
// Based on the "Hashing User-Defined Types in C++1y" proposal from Jeffrey
// Yasskin and Chandler Carruth, see
// http://www.open-std.org/Jtc1/sc22/wg21/docs/papers/2012/n3333.html.

template <typename>
struct hash;


inline size_t hash_combine() { return 0u; }
inline size_t hash_combine(size_t seed) { return seed; }
size_t hash_combine(size_t seed, size_t value);
template <typename T, typename... Ts>
inline size_t hash_combine(T const& v, Ts const&... vs) {
  return hash_combine(hash<T>()(v), hash_combine(vs...));
}


#define V8_BASE_HASH_VALUE_TRIVIAL(type) \
  inline size_t hash_value(type v) { return static_cast<size_t>(v); }
V8_BASE_HASH_VALUE_TRIVIAL(bool)
V8_BASE_HASH_VALUE_TRIVIAL(unsigned char)
V8_BASE_HASH_VALUE_TRIVIAL(unsigned short)  // NOLINT(runtime/int)
#undef V8_BASE_HASH_VALUE_TRIVIAL

size_t hash_value(unsigned int);
size_t hash_value(unsigned long);       // NOLINT(runtime/int)
size_t hash_value(unsigned long long);  // NOLINT(runtime/int)

#define V8_BASE_HASH_VALUE_SIGNED(type)            \
  inline size_t hash_value(signed type v) {        \
    return hash_value(bit_cast<unsigned type>(v)); \
  }
V8_BASE_HASH_VALUE_SIGNED(char)
V8_BASE_HASH_VALUE_SIGNED(short)      // NOLINT(runtime/int)
V8_BASE_HASH_VALUE_SIGNED(int)        // NOLINT(runtime/int)
V8_BASE_HASH_VALUE_SIGNED(long)       // NOLINT(runtime/int)
V8_BASE_HASH_VALUE_SIGNED(long long)  // NOLINT(runtime/int)
#undef V8_BASE_HASH_VALUE_SIGNED

size_t hash_value(float v);
size_t hash_value(double v);

template <typename T>
inline size_t hash_value(T* const& v) {
  size_t const x = bit_cast<size_t>(v);
  return (x >> 3) + x;
}

template <typename T1, typename T2>
inline size_t hash_value(std::pair<T1, T2> const& v) {
  return hash_combine(v.first, v.second);
}


template <typename T>
struct hash : public std::unary_function<T, size_t> {
  size_t operator()(T const& v) const { return hash_value(v); }
};

#define V8_BASE_HASH_SPECIALIZE(type)                            \
  template <>                                                    \
  struct hash<type> : public std::unary_function<type, size_t> { \
    size_t operator()(type const v) const {                      \
      return ::v8::base::hash_value(v);                          \
    }                                                            \
  };
V8_BASE_HASH_SPECIALIZE(bool)
V8_BASE_HASH_SPECIALIZE(signed char)
V8_BASE_HASH_SPECIALIZE(unsigned char)
V8_BASE_HASH_SPECIALIZE(short)           // NOLINT(runtime/int)
V8_BASE_HASH_SPECIALIZE(unsigned short)  // NOLINT(runtime/int)
V8_BASE_HASH_SPECIALIZE(int)
V8_BASE_HASH_SPECIALIZE(unsigned int)
V8_BASE_HASH_SPECIALIZE(long)                // NOLINT(runtime/int)
V8_BASE_HASH_SPECIALIZE(unsigned long)       // NOLINT(runtime/int)
V8_BASE_HASH_SPECIALIZE(long long)           // NOLINT(runtime/int)
V8_BASE_HASH_SPECIALIZE(unsigned long long)  // NOLINT(runtime/int)
V8_BASE_HASH_SPECIALIZE(float)
V8_BASE_HASH_SPECIALIZE(double)
#undef V8_BASE_HASH_SPECIALIZE

template <typename T>
struct hash<T*> : public std::unary_function<T*, size_t> {
  size_t operator()(T* const v) const { return ::v8::base::hash_value(v); }
};

template <typename T1, typename T2>
struct hash<std::pair<T1, T2> >
    : public std::unary_function<std::pair<T1, T2>, size_t> {
  size_t operator()(std::pair<T1, T2> const& v) const {
    return ::v8::base::hash_value(v);
  }
};

}  // namespace base
}  // namespace v8

#endif  // V8_BASE_FUNCTIONAL_H_
