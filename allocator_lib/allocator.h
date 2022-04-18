#ifndef ALLOCATOR_ALLOCATOR_ALLOCATOR_H_
#define ALLOCATOR_ALLOCATOR_ALLOCATOR_H_

#include <iostream>
#include <memory>
#include <stack>
#include <span>

// Allocator object for using in unequal allocator manager
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

// Pool of the memory
class Pool {
 public:
  using size_type = std::size_t;

 public:
  Pool(size_type size) : m_size_(size) {}

  void *allocate() {
	if (m_addrs_.empty()) {
	  std::cout << this << " allocate: address pool is empty" << '\n';
	  add_addrs();
	}

	std::cout << this << " allocate: address pool isn't empty" << '\n';

	auto ptr = m_addrs_.top();
	m_addrs_.pop();

	std::cout << this << " allocate: return free pointer: " << ptr << '\n';

	std::cout << this << " allocate: remain [" << m_addrs_.size() << "] bytes" << '\n';

	return ptr;
  }

  void deallocate(void *ptr) {
	std::cout << this << " deallocate: push unnecessary pointer back to memory pool: " << ptr << '\n';
	m_addrs_.push(ptr);
  }

  void rebind(size_type size) {
	if (!m_addrs_.empty() || !m_blocks_.empty()) {
	  std::cerr << "rebind after alloc unsupported\n";
	  abort();
	}

	std::cout << this << " rebind: old size: " << m_size_ << " now equal new size: " << size << '\n';

	m_size_ = size;
  }

  void add_addrs() {
	constexpr const auto block_size = 0x1000;

	auto block = std::make_unique<uint8_t[]>(block_size);

	auto v = std::span<uint8_t>(block.get(), block_size);

	std::cout << this << " add_addrs: allocate new memory blocks, block_size: " << v.size() << '\n';

	std::cout << this << " add_addrs: pointer to block of the memory: " << (void*)v.data() << '\n';

	auto total_size = v.size() % m_size_ == 0 ? v.size() : v.size() - m_size_;

	std::cout << this << " add_addrs: total bytes allocated: " << total_size << '\n';

	for (auto i = 0ul; i < total_size; i += m_size_) {
	  m_addrs_.push(&v[i]);
	}

	m_blocks_.push(std::move(block));

	std::cout << this << " add_addrs: move block to memory pool " << (void*)&m_blocks_.top() << '\n';
  }

 private:
  std::stack<void *> m_addrs_{};
  std::stack<std::unique_ptr<uint8_t[]>> m_blocks_{};
  size_type m_size_;
};

template<typename T>
class CustomAllocator {
 public:
  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = const T &;
  using size_type = std::size_t;

  using is_always_equal = std::false_type;

  using propagate_on_container_copy_assignment = std::false_type;
  using propagate_on_container_move_assignment = std::true_type;
  using propagate_on_container_swap = std::true_type;

 public:
  CustomAllocator() : m_pool_{std::make_shared<Pool>(sizeof(T))} {
	std::cout << this << " constructor, sizeof(T): " << sizeof(T) << '\n';
  }

  template<typename U>
  CustomAllocator(const CustomAllocator<U> &other) noexcept : m_pool_{other.m_pool_} {
	std::cout << this << " copy constructor (U), sizeof(T): " << sizeof(T) << '\n';
	m_pool_->rebind(sizeof(T));
  }

  CustomAllocator(CustomAllocator &&other) noexcept: m_pool_{std::move(other.m_pool_)} {
	std::cout << this << " move constructor, sizeof(T): " << sizeof(T) << '\n';
  }

  CustomAllocator &operator=(CustomAllocator &&other) noexcept {
	std::cout << this << " move assignment, sizeof(T): " << sizeof(T) << '\n';
	m_pool_ = std::move(other.m_pool_);
	return *this;
  }

  CustomAllocator(const CustomAllocator &other) noexcept: m_pool_{other.m_pool_} {
	std::cout << this << " copy constructor, sizeof(T): " << sizeof(T) << '\n';
  }

  CustomAllocator &operator=(const CustomAllocator &other) noexcept {
	if (this != &other) {
	  std::cout << this << " copy assignment, sizeof(T): " << sizeof(T) << '\n';
	  m_pool_ = other.m_pool_;
	}
	return *this;
  }

  pointer allocate(size_type n) {
	std::cout << this << " allocate: n: " << n << '\n';
	if (n != 1) {
	  std::cout << this << " allocate: n != 1, allocate sizeof(T) * n: " << sizeof(T) * n << '\n';
	  return static_cast<pointer>(malloc(sizeof(T) * n));
	}
	std::cout << this << " allocate: n == 1, use memory pool" << '\n';
	return static_cast<pointer>(m_pool_->allocate());
  }

  void deallocate(pointer ptr, size_type n) {
	std::cout << this << " deallocate: ptr: " << ptr << ", n:" << n << '\n';
	if (n != 1) {
	  std::cout << this << " deallocate: n != 1, use free" << '\n';
	  free(ptr);
	}
	std::cout << this << " deallocate: n == 1, use memory pool" << '\n';
	m_pool_->deallocate(ptr);
  }

 private:
  template<typename U>
  friend
  class CustomAllocator;

  template<typename T1, typename T2>
  friend bool operator==(const CustomAllocator<T1> &lhs, const CustomAllocator<T2> &rhs);

  template<typename T1, typename T2>
  friend bool operator!=(const CustomAllocator<T1> &lhs, const CustomAllocator<T2> &rhs);

  std::shared_ptr<Pool> m_pool_;
};

template<typename T1, typename T2>
bool operator==(const CustomAllocator<T1> &lhs, const CustomAllocator<T2> &rhs) {
  return lhs.m_pool_ == rhs.m_pool_;
}

template<typename T1, typename T2>
bool operator!=(const CustomAllocator<T1> &lhs, const CustomAllocator<T2> &rhs) {
  return lhs.m_pool_ != rhs.m_pool_;
}

#endif //ALLOCATOR_ALLOCATOR_ALLOCATOR_H_
