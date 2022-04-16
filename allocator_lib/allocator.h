#ifndef ALLOCATOR_ALLOCATOR_ALLOCATOR_H_
#define ALLOCATOR_ALLOCATOR_ALLOCATOR_H_

#include <iostream>

template<typename T>
class CustomAllocator {
 public:

  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = const T &;
  using size_type = std::size_t;
  using is_always_equal = std::true_type;

  CustomAllocator() {
	std::cout << this << " constructor sizeof(T): " << sizeof(T) << '\n';
  }

  template<typename U>
  CustomAllocator(const CustomAllocator<U> &other) noexcept {
	(void)other;
  }

  CustomAllocator(CustomAllocator &&other) noexcept {
	(void)other;
	std::cout << this << " move constructor, sizeof(T): " << sizeof(T) << '\n';
  }

  CustomAllocator &operator=(CustomAllocator &&other) noexcept {
	(void)other;
	std::cout << this << " move assignment, sizeof(T): " << sizeof(T) << '\n';
	return *this;
  }

  CustomAllocator(const CustomAllocator &other) noexcept {
	(void)other;
	std::cout << this << " copy constructor, sizeof(T): " << sizeof(T) << '\n';
  }

  CustomAllocator &operator=(const CustomAllocator &other) noexcept {
	(void)other;
	std::cout << this << " copy assignment, sizeof(T): " << sizeof(T) << '\n';
	return *this;
  }

//  template<typename U>
//  struct rebind {
//	using other = CustomAllocator<U>;
//  };

  pointer allocate(size_type n) const {
	if (auto ptr = std::malloc(n * sizeof(T))) {
	  std::cout << this << " allocate [" << n << "]: " << ptr << '\n';
	  return reinterpret_cast<pointer>(ptr);
	}

	throw std::bad_alloc();
  }

  void deallocate(pointer ptr, size_type n) const {
	std::cout << this << " deallocate [" << n << "]: " << ptr << '\n';

	(void)n;

	return std::free(ptr);
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
};

template<typename T1, typename T2>
bool operator==(const CustomAllocator<T1> &, const CustomAllocator<T2> &) {
  return true;
}

template<typename T1, typename T2>
bool operator!=(const CustomAllocator<T1> &, const CustomAllocator<T2> &) {
  return false;
}

#endif //ALLOCATOR_ALLOCATOR_ALLOCATOR_H_
