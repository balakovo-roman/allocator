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
