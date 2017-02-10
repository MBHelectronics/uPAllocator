
#ifndef SMART_PTR_H_INCLUDED
#define SMART_PTR_H_INCLUDED

#include <cstddef>
#include "smart_ptr_allocator_policy.h"

#ifdef DEBUG
#define DEBUG_MSG(str)             \
  do {                             \
    std::cout << str << std::endl; \
  } while (false)
#else
#define DEBUG_MSG(str) \
  do {                 \
  } while (false)
#endif

namespace smart_ptr {

template <typename T, size_t MaxObjects, typename BasePolicy,
          typename AllocatorPolicy>
class SmartPointer;

template <typename T>
class BaseWrapper;

template <typename T, size_t MaxObjects = 1,
          typename BasePolicy = BaseWrapper<T>,
          typename AllocatorPolicy =
              uP_smart_ptr_allocator_policy<SmartPointer, MaxObjects>>
using smart_ptr_t = SmartPointer<T, MaxObjects, BasePolicy, AllocatorPolicy>;

/* Base class for refcounted objects. */

template <template <typename> class Derived, typename T>
class RefCountedBase {
 public:
  virtual ~RefCountedBase() {}

  RefCountedBase& operator=(const RefCountedBase& b) { return *this; }

  RefCountedBase& operator=(RefCountedBase&& b) { return *this; }

  void capture() {
    static_cast<Derived<T>*>(this)->capture_impl();
    DEBUG_MSG("capture " << _refcount);
  }

  void release() {
    static_cast<Derived<T>*>(this)->release_impl();
    DEBUG_MSG("release " << _refcount);
  }

 protected:
  size_t _refcount;

  RefCountedBase() : _refcount(0) {}
  RefCountedBase(const RefCountedBase<Derived, T>& b)
      : _refcount(b._refcount) {}
};

/* Wrapper class for refcounted objects */

template <typename T>
class BaseWrapper : public RefCountedBase<BaseWrapper, T>

{
 public:
  BaseWrapper() : _p(nullptr) {}
  BaseWrapper(const T* p) : _p(const_cast<T*>(p)) {}

  void capture_impl() { ++this->_refcount; }

  void release_impl() {
    if (--this->_refcount == 0) {
      // destroy the managed object
      _p->~T();
    }
  }

  T* get() { return _p; }

 private:
  T* _p;
};

/* Reference counting smart pointer */

template <typename T, size_t MaxObjects, typename BasePolicy,
          typename AllocatorPolicy>
class SmartPointer : public AllocatorPolicy {
 public:
  SmartPointer() : _counted_base(BasePolicy()) {}
  SmartPointer(std::nullptr_t)
      : _counted_base(BasePolicy()), _counted_base_ptr(&_counted_base) {}

  explicit SmartPointer(const T& p)
      : _counted_base(BasePolicy(this->template allocate<T>(p))),
        _counted_base_ptr(&_counted_base) {
    if (_counted_base_ptr->get()) {
      _counted_base_ptr->capture();
    }
  }

  SmartPointer(const SmartPointer& b)
      : _counted_base(b._counted_base), _counted_base_ptr(&b._counted_base) {
    if (_counted_base_ptr->get()) {
      _counted_base_ptr->capture();
    }
  }

  SmartPointer(SmartPointer&& b)
      : _counted_base(b._counted_base), _counted_base_ptr(&_counted_base) {
    b._counted_base = BasePolicy();
    b._counted_base_ptr = nullptr;
  }

  ~SmartPointer() {
    if (_counted_base_ptr->get()) {
      _counted_base_ptr->release();
    }

    this->template deallocate<T>(_counted_base_ptr->get());
  }

  const SmartPointer& operator=(SmartPointer& b) {
    // Support self assignment
    if (b._counted_base_ptr->get()) {
      b._counted_base_ptr->capture();
    }

    // Support cycle breaking
    auto q = _counted_base_ptr;
    _counted_base_ptr = b._counted_base_ptr;

    if (q->get()) {
      q->release();
    }

    return *this;
  }

  const SmartPointer& operator=(SmartPointer&& b) {
    // Detect self assignment
    if (_counted_base_ptr->get() != b._counted_base_ptr->get()) {
      // Support cycle breaking
      auto q = _counted_base_ptr;
      _counted_base_ptr = b._counted_base_ptr;

      b._counted_base = BasePolicy();
      b._counted_base_ptr = nullptr;

      if (q->get()) {
        q->release();
      }
    }

    return *this;
  }

  T& operator*() const { return _counted_base_ptr->get(); }

  T* operator->() const { return _counted_base_ptr->get(); }

  bool operator<(const SmartPointer& b) const {
    return _counted_base_ptr->get() < b._counted_base_ptr->get();
  }

  explicit operator bool() const { return _counted_base_ptr->get() != nullptr; }

 private:
  BasePolicy _counted_base;
  BasePolicy* _counted_base_ptr;
};
}

#endif  // SMART_PTR_H_INCLUDED
