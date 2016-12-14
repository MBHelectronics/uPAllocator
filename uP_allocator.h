#ifndef UP_ALLOCATOR_H_INCLUDED
#define UP_ALLOCATOR_H_INCLUDED

#include <cstddef>
#include <cstdint>
#include "object_traits.h"
#include "uP_allocator_policy.h"

#define FORWARD_ALLOCATOR_TRAITS(C)                  \
typedef typename C::value_type      value_type;      \
typedef typename C::pointer         pointer;         \
typedef typename C::const_pointer   const_pointer;   \
typedef typename C::reference       reference;       \
typedef typename C::const_reference const_reference; \
typedef typename C::size_type       size_type;       \
typedef typename C::difference_type difference_type; \

namespace uP_allocator {

template <typename T, size_t N, typename PolicyT = uP_allocator_policy<T>,
          typename TraitsT = object_traits<T> >
class allocator : public PolicyT, public TraitsT {
 public:
  // Template parameters
  typedef PolicyT Policy;
  typedef TraitsT Traits;

  FORWARD_ALLOCATOR_TRAITS(Policy)

  template <typename U>
  struct rebind {
    typedef allocator<U, N, typename Policy::template rebind<U>::other,
                      typename Traits::template rebind<U>::other> other;
  };

  // Constructor
  allocator() : Policy(N) {}

  // Copy Constructor
  template <typename U, typename PolicyU, typename TraitsU>
  allocator(allocator<U, N, PolicyU, TraitsU> const& other)
      : Policy(other), Traits(other) {}
};

// Two allocators are not equal unless a specialization says so
template <typename T, size_t N, typename PolicyT, typename TraitsT, typename U,
          size_t M, typename PolicyU, typename TraitsU>
bool operator==(allocator<T, N, PolicyT, TraitsT> const& left,
                allocator<U, M, PolicyU, TraitsU> const& right) {
  return false;
}

// Also implement inequality
template <typename T, size_t N, typename PolicyT, typename TraitsT, typename U,
          size_t M, typename PolicyU, typename TraitsU>
bool operator!=(allocator<T, N, PolicyT, TraitsT> const& left,
                allocator<U, M, PolicyU, TraitsU> const& right) {
  return !(left == right);
}

// Comparing an allocator to anything else should not show equality
template <typename T, size_t N, typename PolicyT, typename TraitsT,
          typename OtherAllocator>
bool operator==(allocator<T, N, PolicyT, TraitsT> const& left,
                OtherAllocator const& right) {
  return false;
}

// Also implement inequality
template <typename T, size_t N, typename PolicyT, typename TraitsT,
          typename OtherAllocator>
bool operator!=(allocator<T, N, PolicyT, TraitsT> const& left,
                OtherAllocator const& right) {
  return !(left == right);
}

// Specialize for the uP_allocator_policy
template <typename T, size_t N, typename TraitsT, typename U, size_t M,
          typename TraitsU>
bool operator==(allocator<T, N, uP_allocator_policy<T>, TraitsT> const& left,
                allocator<U, M, uP_allocator_policy<U>, TraitsU> const& right) {
  return true;
}

// Also implement inequality
template <typename T, size_t N, typename TraitsT, typename U, size_t M,
          typename TraitsU>
bool operator!=(allocator<T, N, uP_allocator_policy<T>, TraitsT> const& left,
                allocator<U, M, uP_allocator_policy<U>, TraitsU> const& right) {
  return !(left == right);
}
}
#endif  // UP_ALLOCATOR_H_INCLUDED
