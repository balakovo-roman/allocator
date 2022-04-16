#ifndef ALLOCATOR_ALLOCATOR_ALLOCATOR_H_
#define ALLOCATOR_ALLOCATOR_ALLOCATOR_H_

#include <iostream>
#include <memory>

class CustomAllocatorObject {
 public:
  using size_type = std::size_t;

 public:
  void *allocate(size_type size) {
	if (auto ptr = malloc(size)) {
	  std::cout << this << " allocate " << ptr << '\n';
	  return ptr;
	}

	throw std::bad_alloc();
  }

  void deallocate(void *ptr) {
	std::cout << this << " deallocate " << ptr << '\n';
	free(ptr);
  }
};

template<typename T>
class CustomAllocator {
  template<typename U>
  friend
  class CustomAllocator;

 public:
  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = const T &;
  using size_type = std::size_t;
  using is_always_equal = std::false_type;
  using propagate_on_container_copy_assignment = std::true_type;
  using propagate_on_container_move_assignment = std::true_type;
  using propagate_on_container_swap = std::true_type;

 public:
  CustomAllocator() : m_allocator_{std::make_shared<CustomAllocatorObject>()} {
	std::cout << this << " constructor, sizeof(T): " << sizeof(T) << '\n';
  }

  template<typename U>
  CustomAllocator(const CustomAllocator<U> &other) noexcept : m_allocator_{other.m_allocator_} {
	std::cout << this << " copy constructor (U), sizeof(T): " << sizeof(T) << '\n';
  }

  CustomAllocator(CustomAllocator &&other) noexcept: m_allocator_{std::move(other.m_allocator_)} {
	std::cout << this << " move constructor, sizeof(T): " << sizeof(T) << '\n';
  }

  CustomAllocator &operator=(CustomAllocator &&other) noexcept {
	std::cout << this << " move assignment, sizeof(T): " << sizeof(T) << '\n';
	m_allocator_ = std::move(other.m_allocator_);
	return *this;
  }

  CustomAllocator(const CustomAllocator &other) noexcept: m_allocator_{other.m_allocator_} {
	std::cout << this << " copy constructor, sizeof(T): " << sizeof(T) << '\n';
  }

  CustomAllocator &operator=(const CustomAllocator &other) noexcept {
	std::cout << this << " copy assignment, sizeof(T): " << sizeof(T) << '\n';
	m_allocator_ = other.m_allocator_;
	return *this;
  }

  pointer allocate(size_type n) const {
	auto ptr = m_allocator_->allocate(sizeof(T) * n);
	return reinterpret_cast<pointer>(ptr);
  }

  void deallocate(pointer ptr, size_type n) const {
	(void)n;
	return m_allocator_->deallocate(ptr);
  }

  template<typename U, typename ...Args>
  void construct(U *ptr, Args &&...args) const {
	std::cout << this << " construct: " << ptr << '\n';

	new(ptr) U(std::forward<Args>(args)...);
  }

  void destroy(pointer ptr) const {
	std::cout << this << " destroy: " << ptr << '\n';

	ptr->~T();
  }

 private:
  template<typename T1, typename T2>
  friend bool operator==(const CustomAllocator<T1> &lhs, const CustomAllocator<T2> &rhs);

  template<typename T1, typename T2>
  friend bool operator!=(const CustomAllocator<T1> &lhs, const CustomAllocator<T2> &rhs);

  std::shared_ptr<CustomAllocatorObject> m_allocator_;
};

template<typename T1, typename T2>
bool operator==(const CustomAllocator<T1> &lhs, const CustomAllocator<T2> &rhs) {
  return lhs.m_allocator_ == rhs.m_allocator_;
}

template<typename T1, typename T2>
bool operator!=(const CustomAllocator<T1> &lhs, const CustomAllocator<T2> &rhs) {
  return lhs.m_allocator_ != rhs.m_allocator_;
}

#endif //ALLOCATOR_ALLOCATOR_ALLOCATOR_H_
