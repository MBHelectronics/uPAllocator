#ifndef UP_STATIC_CLASS_H
#define UP_STATIC_CLASS_H

#include <utility>
#include "uP_allocator.h"

namespace uP_static_class {

template <class TDerived, size_t MaxInstances = 1>
class static_class_wrapper;

template <class TDerived, size_t MaxInstances = 1>
using static_class_t = static_class_wrapper<TDerived, MaxInstances>;

template <class TDerivedWrapper, size_t MaxInstances>
class uP_static_class {
 public:
  virtual ~uP_static_class() {}

 protected:
  template <typename T>
  using class_allocator_t = uP_allocator::allocator<T, MaxInstances>;

  template <typename T>
  static class_allocator_t<T>* const s_allocator() {
    static auto _class_allocator = new class_allocator_t<T>();
    return _class_allocator;
  }
};

template <class TDerived, size_t MaxInstances>
class static_class_wrapper
    : public uP_static_class<static_class_wrapper<TDerived, MaxInstances>,
                             MaxInstances> {
 public:
  template <class T>
  struct _class_ptr_s;

  /* This template is required to ensure calls to the copy
    constructor do not invoke the forwarding constructor */

  template <typename... Args,
            typename = decltype(TDerived(std::declval<Args>()...))>

  explicit static_class_wrapper(Args&&... args)
      : _s_class_ptr(new (allocator<_class_ptr_s<TDerived>>()->allocate())
                         _class_ptr_s<TDerived>(std::forward<Args>(args)...)) {}

  static_class_wrapper(const static_class_wrapper& other)
      : _s_class_ptr(new (allocator<_class_ptr_s<TDerived>>()->allocate())
                         _class_ptr_s<TDerived>(*(other._s_class_ptr))) {}

  friend void swap(static_class_wrapper& first,
                   static_class_wrapper& second) noexcept {
    using std::swap;
    swap(first._s_class_ptr, second._s_class_ptr);
  }

  static_class_wrapper& operator=(static_class_wrapper other) {
    swap(*this, other);
    return *this;
  }

  ~static_class_wrapper() {
    _s_class_ptr->~_class_ptr_s();
    allocator<_class_ptr_s<TDerived>>()->deallocate(_s_class_ptr);
  }

  operator TDerived*() { return _s_class_ptr->_class_ptr; }

  operator TDerived*() const { return _s_class_ptr->_class_ptr; }

  TDerived* operator->() { return _s_class_ptr->_class_ptr; }

  TDerived* operator->() const { return _s_class_ptr->_class_ptr; }

  template <class T>
  struct _class_ptr_s {
   public:
   
     /* This template is required to ensure calls to the copy
    constructor do not invoke the forwarding constructor */
   
    template <typename... Args, typename = decltype(T(std::declval<Args>()...))>

    explicit _class_ptr_s(Args&&... args)
        : _class_ptr(new (_buf) T(std::forward<Args>(args)...)) {}

    _class_ptr_s(const _class_ptr_s<T>& other)
        : _class_ptr(new (_buf) T(*(other._class_ptr))) {}

    _class_ptr_s& operator=(const _class_ptr_s<T>& other) = default;

    ~_class_ptr_s() { _class_ptr->~T(); }

    T* const _class_ptr;

   private:
    char _buf[sizeof(T)];
  };

 private:
  _class_ptr_s<TDerived>* _s_class_ptr;

  template <typename T>
  using class_allocator_t = uP_allocator::allocator<T, MaxInstances>;

  template <typename T>
  class_allocator_t<T>* allocator() {
    return this->template s_allocator<T>();
  }
};
}

#endif  // UP_STATIC_CLASS_H
