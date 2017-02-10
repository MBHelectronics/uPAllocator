
#ifndef SMART_PTR_ALLOCATOR_POLICY_H_INCLUDED
#define SMART_PTR_ALLOCATOR_POLICY_H_INCLUDED

#include "uP_allocator.h"

using namespace uP_allocator;

template <template <typename, size_t, typename, typename> class Derived,
          size_t N>
struct uP_smart_ptr_allocator_policy {
 public:
  uP_smart_ptr_allocator_policy() = default;

  template <typename T_obj, size_t N1 = N,
            typename PolicyT1 = uP_allocator_policy<T_obj>,
            typename TraitsT1 = object_traits<T_obj>>
  static inline T_obj* allocate(const T_obj& ptr) {
    return new (_allocator<T_obj, N1, PolicyT1, TraitsT1>()->allocate())
        T_obj(ptr);
  }

  template <typename T_obj, size_t N1 = N,
            typename PolicyT1 = uP_allocator_policy<T_obj>,
            typename TraitsT1 = object_traits<T_obj>>
  static inline void deallocate(T_obj* ptr) {
    _allocator<T_obj, N1, PolicyT1, TraitsT1>()->deallocate(ptr);
  }

 private:
  template <typename T2, size_t N2, typename PolicyT2, typename TraitsT2>
  static allocator<T2, N2, PolicyT2, TraitsT2>* const _allocator() {
    static auto allocator_ptr = new allocator<T2, N2, PolicyT2, TraitsT2>();
    return allocator_ptr;
  }
};

#endif  // SMART_PTR_ALLOCATOR_POLICY_H_INCLUDED
