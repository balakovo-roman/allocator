#ifndef ALLOCATOR_ALLOCATOR_LIB_SIMPLE_LIST_H_
#define ALLOCATOR_ALLOCATOR_LIB_SIMPLE_LIST_H_

#include <memory>

template<typename T>
struct Node {
  template<typename... Args>
  Node(Args &&... value) : data_(std::forward<Args>(value)...) {}
  Node *next_{nullptr};
  T data_;
};

template<typename T, typename Allocator>
class SimpleList;

template<typename T, bool Const>
class SimpleListIterator {
 public:
  using difference_type = std::ptrdiff_t;
  using value_type = int;
  using pointer = std::conditional_t<Const, const int *, int *>;
  using reference = std::conditional_t<Const, const int &, int &>;
  using iterator_category = std::forward_iterator_tag;

  reference operator*() const noexcept { return ptr_->data_; }

  auto &operator++() noexcept {
	ptr_ = ptr_->next_;
	return *this;
  }

  auto operator++(int) noexcept {
	auto result = *this;
	++*this;
	return result;
  }

  template<bool R>
  bool operator==(const SimpleListIterator<T, R> &other) const noexcept { return ptr_ == other.ptr_; }

  template<bool R>
  bool operator!=(const SimpleListIterator<T, R> &other) const noexcept { return ptr_ != other.ptr_; }

  operator SimpleListIterator<T, true>() const noexcept { return SimpleListIterator<T, true>{ptr_}; }

 private:
  template<typename, typename Allocator>
  friend
  class SimpleList;

  friend class SimpleListIterator<T, !Const>;

  using NodePointer = std::conditional_t<Const, const Node<T> *, Node<T> *>;

  explicit SimpleListIterator(NodePointer p) noexcept: ptr_(p) {}

  NodePointer ptr_;
};

template<typename T, typename Allocator = std::allocator<T>>
class SimpleList {
 public:
  using value_type = T;
  using size_type = std::size_t;
  using iterator = SimpleListIterator<T, false>;
  using const_iterator = SimpleListIterator<T, true>;
  using reference = value_type &;
  using const_reference = const value_type &;
  using pointer = T *;
  using const_pointer = const T *;

  SimpleList() = default;

  SimpleList(const SimpleList &other)
	  : node_allocator_{NodeTraits::select_on_container_copy_construction(other.node_allocator_)} {
	auto node = other.head_;
	while (node) {
	  EmplaceFront(node->data_);
	  node = node->next_;
	}
  }

  SimpleList(const SimpleList &other, const Allocator &other_alloc)
	  : node_allocator_{NodeAllocator(other_alloc)} {
	auto node = other.head_;
	while (node) {
	  EmplaceFront(node->data_);
	  node = node->next_;
	}
  }

  SimpleList(SimpleList &&other) noexcept
	  : node_allocator_{std::move(other.node_allocator_)},
		head_{std::exchange(other.head_, nullptr)} {
  }

  SimpleList(SimpleList &&other, const Allocator &other_alloc) noexcept
	  : node_allocator_(NodeAllocator(other_alloc)),
		head_(std::exchange(other.head_, nullptr)) {}

  SimpleList &operator=(const SimpleList &other) {
	if (this != std::addressof(other)) {
	  constexpr bool pocca = NodeTraits::propagate_on_container_copy_assignment::value;
	  if constexpr(pocca) {
		if (node_allocator_ != other.node_allocator_) {
		  Clear();
		}
		node_allocator_ = other.node_allocator_;
	  }
	  Clear();
	  auto node = other.head_;
	  while (node) {
		EmplaceFront(node->data_);
		node = node->next_;
	  }
	}
	return *this;
  }

  SimpleList &operator=(SimpleList &&other) noexcept {
	constexpr bool pocma = NodeTraits::propagate_on_container_move_assignment::value;
	if constexpr(pocma) {
	  Clear();
	  node_allocator_ = std::move(other.node_allocator_);
	  head_ = std::exchange(other.head_, nullptr);
	} else if constexpr (NodeTraits::is_always_equal::value || node_allocator_ == other.node_allocator_) {
	  Clear();
	  head_ = std::exchange(other.head_, nullptr);
	} else {
	  auto node = other.head_;
	  while (node) {
		EmplaceFront(std::move(node->data_));
		node = node->next_;
	  }
	}

	return *this;
  }

  void swap(SimpleList &other) noexcept {
	constexpr bool pocs = NodeTraits::propagate_on_container_swap::value;
	if (this != std::addressof(other)) {
	  if constexpr(pocs) {
		std::swap(head_, other.head_);
		std::swap(node_allocator_, other.node_allocator_);
	  } else if constexpr (NodeTraits::is_always_equal::value || node_allocator_ == other.node_allocator_) {
		std::swap(head_, other.head_);
	  } else {
		auto temp = std::move(*this);
		*this = std::move(other);
		other = std::move(temp);
	  }
	}
  }

  ~SimpleList() {
	Clear();
  }

  size_type Size() const {
	return size_;
  }

  reference &At(size_type i) {
	if (i >= size_) throw std::out_of_range("out of range");
	auto p{head_};
	for (size_type j = 0; j < i; ++j) {
	  p = p->next_;
	}
	return p->data_;
  }

  template<typename... Args>
  void EmplaceFront(Args... args) {
	auto node = NodeTraits::allocate(node_allocator_, 1);
	NodeTraits::construct(node_allocator_, node, std::forward<Args>(args)...);

	node->next_ = head_;
	head_ = node;

	++size_;
  }

  const_iterator begin() const noexcept { return const_iterator{head_}; }
  const_iterator end() const noexcept { return const_iterator{nullptr}; }

  iterator begin() noexcept { return iterator{head_}; }
  iterator end() noexcept { return iterator{nullptr}; }

 private:
  using NodeAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Node<T>>;
  using NodeTraits = std::allocator_traits<NodeAllocator>;

  void Clear() {
	while (head_) {
	  auto node = head_;
	  head_ = head_->next_;

	  NodeTraits::destroy(node_allocator_, node);
	  NodeTraits::deallocate(node_allocator_, node, 1);
	}
  }

  NodeAllocator node_allocator_;
  Node<T> *head_{nullptr};
  size_type size_{0};
};

#endif //ALLOCATOR_ALLOCATOR_LIB_SIMPLE_LIST_H_
