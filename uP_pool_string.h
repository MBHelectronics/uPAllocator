#ifndef UP_POOL_STRING_H_INCLUDED
#define UP_POOL_STRING_H_INCLUDED

#include <cstddef>
#include <string>
#include "uP_allocator.h"

namespace uP_pool_string {

template <size_t M>
class uP_pool_string;

template <size_t M>
using uP_pool_string_t = uP_pool_string<M>;

template <size_t M>
class uP_pool_string {
 public:
  using pool_string = ::std::basic_string<char, ::std::char_traits<char>,
                                          uP_allocator::allocator<char, M>>;

  template <size_t N>
  static uP_pool_string_t<N> make_pool_string(const char* string) {
    return uP_pool_string<N>(string);
  }

  inline operator pool_string() const { return _string; }
  inline operator pool_string() { return _string; }
  inline operator const char*() const { return _string.data(); }

 private:
  static uP_allocator::allocator<char, M> _allocator;
  pool_string _string;
  uP_pool_string(const char* string)
      : _string(pool_string(string, _allocator)) {}
};

template <size_t M>
uP_allocator::allocator<char, M> uP_pool_string<M>::_allocator =
    uP_allocator::allocator<char, M>();
}
#endif  // UP_POOL_STRING_H_INCLUDED
