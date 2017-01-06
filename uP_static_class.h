#ifndef UP_STATIC_CLASS_H
#define UP_STATIC_CLASS_H

#include <cstddef>
#include <utility>
#include <new>

namespace uP_static_class {

template <class TDerivedWrapper, class TDerived>
class static_class {
 public:
  operator TDerived() const {
    return &static_cast<static_class*>(this)->get_ref();
  }

  operator TDerived*() { return &static_cast<static_class*>(this)->get_ref(); }

 protected:
  template <typename... Args1>
  static_class(Args1&&... args)
      : _derived(make_ref(std::forward<Args1>(args)...)) {}

  TDerived& get_ref() { return _derived; }

 private:
  TDerived& _derived;

  static char* const buf_init() {
    static char* _buf = new char[sizeof(TDerived)];
    return _buf;
  }

  template <typename... Args2>
  static TDerived& make_ref(Args2&&... args) {
    struct init {
     public:
      init(Args2&&... args)
          : _ref(new (buf_init()) TDerived(std::forward<Args2>(args)...)) {}

      operator TDerived*() const { return _ref; }

     private:
      TDerived* const _ref;
    };

    static init _init(std::forward<Args2>(args)...);
    return *_init;
  }
};

template <class TDerived>
class static_class_wrapper;

template <class TDerived>
using static_class_t = static_class_wrapper<TDerived>;

template <class TDerived>
class static_class_wrapper
    : public static_class<static_class_wrapper<TDerived>, TDerived> {
 public:
  template <typename... Args>
  static_class_wrapper(Args&&... args)
      : static_class<static_class_wrapper<TDerived>, TDerived>(
            std::forward<Args>(args)...) {}
};

}
#endif  // UP_STATIC_CLASS_H
