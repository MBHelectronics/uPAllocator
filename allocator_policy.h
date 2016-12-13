#ifndef ALLOCATOR_POLICY_H_INCLUDED
#define ALLOCATOR_POLICY_H_INCLUDED

#include <cstddef>
#include <cstdint>
#include <new>

#define ALLOCATOR_TRAITS(T)                \
typedef T                 type;            \
typedef type              value_type;      \
typedef value_type*       pointer;         \
typedef value_type const* const_pointer;   \
typedef value_type&       reference;       \
typedef value_type const& const_reference; \
typedef std::size_t       size_type;       \
typedef std::ptrdiff_t    difference_type; \

// space for only one object per request can
// be reserved with this allocator implementation

template <typename T>
struct max_allocations {
  enum { value = static_cast<std::size_t>(sizeof(T) / sizeof(T)) };
};

template <typename T>
class arduino_allocator_policy {
 public:
  ALLOCATOR_TRAITS(T)

  template <typename U>
  struct rebind {
    typedef arduino_allocator_policy<U> other;
  };

  // ctr
  arduino_allocator_policy(size_type num_blocks,
                           size_type size_of_each_block = sizeof(T))
      : _m_num_blocks(num_blocks),
        _m_size_of_each_block(0),
        _m_num_free_blocks(0),
        _m_num_initialized(0),
        _m_mem_start(reinterpret_cast<uintptr_t>(
            new uint8_t[_m_size_of_each_block * _m_num_blocks]))

  {}

  // dtr
  ~arduino_allocator_policy() {
    delete[] reinterpret_cast<uint8_t*>(_m_mem_start);
    _m_mem_start = reinterpret_cast<uintptr_t>(nullptr);
  }

  // copy ctr
  template <typename U>
  arduino_allocator_policy(arduino_allocator_policy<U> const& other) {}

  // Allocate memory
  pointer allocate(size_type count = 1, const_pointer /* hint */ = 0) {
    if (count > max_size()) {
      throw std::bad_alloc();
    }

    if (_m_num_initialized < _m_num_blocks) {
      size_t* p =
          reinterpret_cast<size_t*>(_addr_from_index(_m_num_initialized));
      *p = _m_num_initialized + 1;
      _m_num_initialized++;
    }

    auto ret = static_cast<value_type*>(nullptr);

    if (_m_num_free_blocks > 0) {
      ret = reinterpret_cast<value_type*>(_m_next);
      --_m_num_free_blocks;

      if (_m_num_free_blocks != 0) {
        _m_next = _addr_from_index(*reinterpret_cast<size_t*>(_m_next));
      } else {
        _m_next = reinterpret_cast<uintptr_t>(static_cast<T*>(nullptr));
      }
    }
    return ret;

    return static_cast<pointer>(
        ::operator new(count * sizeof(type), ::std::nothrow));
  }

  // Delete memory
  void deallocate(pointer p, size_type /* count */) {
    if (reinterpret_cast<size_t*>(_m_next) != nullptr) {
      auto tmp = reinterpret_cast<size_t*>(&(*p));
      *tmp = _index_from_addr(_m_next);
      _m_next = reinterpret_cast<uintptr_t>(&(*p));
    } else {
      auto tmp = reinterpret_cast<size_t*>(&(*p));
      *tmp = _m_num_blocks;
      _m_next = reinterpret_cast<uintptr_t>(&(*p));
    }
    ++_m_num_free_blocks;
  }

 private:
  size_t _m_num_blocks;          // Num of blocks
  size_t _m_size_of_each_block;  // Size of each block
  size_t _m_num_free_blocks;     // Num of remaining blocks
  size_t _m_num_initialized;     // Num of initialized blocks
  uintptr_t _m_mem_start;        // Beginning of memory pool
  uintptr_t _m_next;             // Num of next free block

  uintptr_t _addr_from_index(size_type i) const {
    return _m_mem_start + (i * _m_size_of_each_block);
  }

  size_type _index_from_addr(uintptr_t p) const {
    return static_cast<size_type>(
        static_cast<std::ptrdiff_t>(p - _m_mem_start) / _m_size_of_each_block);
  }

  // Max number of objects that can be allocated in one call
  size_type max_size() const { return max_allocations<T>::value; }
};

#endif  // ALLOCATOR_POLICY_H_INCLUDED
