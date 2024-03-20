#ifndef CircularBuffer_h
#define CircularBuffer_h

//If stdlib is_arithmetic exists, use that. Otherwise, custom roll one.
//Necessary for minimum() and maximum() methods.
#if defined(__has_include) && __has_include(<type_traits>)
#include <type_traits>
#else
namespace std {
  template<typename T>
  struct is_arithmetic { static const bool value = false; };
  template<> struct is_arithmetic<unsigned long long> { static const bool value = true; };
  template<> struct is_arithmetic<long long> { static const bool value = true; };
  template<> struct is_arithmetic<unsigned long> { static const bool value = true; };
  template<> struct is_arithmetic<long> { static const bool value = true; };
  template<> struct is_arithmetic<unsigned int> { static const bool value = true; };
  template<> struct is_arithmetic<int> { static const bool value = true; };
  template<> struct is_arithmetic<unsigned short> { static const bool value = true; };
  template<> struct is_arithmetic<short> { static const bool value = true; };
  template<> struct is_arithmetic<unsigned char> { static const bool value = true; };
  template<> struct is_arithmetic<char> { static const bool value = true; };
  template<> struct is_arithmetic<float> { static const bool value = true; };
  template<> struct is_arithmetic<double> { static const bool value = true; };
}
#endif

/**
 * A circular buffer of fixed size.
 * Appending can be done indefinitely,
 * as the index will just loop back around to the beginning.
 */
template<typename TYPE, unsigned int LEN>
class CircularBuffer {
  TYPE data[LEN];   //The data to store
  int counter = 0;  //The current index. This will always be between 0 and LEN.
  int total = 0;    //The total number of items in the buffer. This will never exceed LEN.

  static_assert(LEN > 0, "Buffer must have more than zero elements.");

  //A custom iterator for this class, to allow C++'s range based for loop syntax
  class iterator {
    const TYPE* ptr;
    int offset;

  public:
    iterator(const TYPE* buffer, int index)
      : ptr(buffer), offset(index) {}

    iterator operator++() {
      ++offset;
      return *this;
    }

    bool operator!=(const iterator& other) const {
      return offset != other.offset;
    }

    const TYPE& operator*() const {
      return ptr[offset % LEN];
    }
  };

public:
  const int length = LEN;

  //Default constructor, don't initialize data.
  CircularBuffer() {}

  //Initialize the buffer with the contents of an array.
  //This allows for brace-enclosed initialization e.g.
  //CircularBuffer<int, 3> buffer = {1, 2, 3};
  template<typename... ARGS>
  CircularBuffer(const ARGS&... args) {
    this->populate(args...);
  }

  //Initialize data to a specific value, e.g. 0
  CircularBuffer(TYPE default_value) {
    for (int i = 0; i < LEN; ++i) {
      data[i] = default_value;
    }
  }

  //Get an element from the buffer. This is used when we're not modifying it.
  TYPE operator[](const int index) const {
    return data[index % LEN];
  }

  //Get a reference to an element from the buffer. This is used when we ARE modifying it.
  //The compiler knows the difference and will use this function only when needed.
  TYPE& operator[](const int index) {
    return data[index % LEN];
  }

  //Add a value to the buffer, and increment the current index
  void append(const TYPE value) {
    data[counter] = value;
    counter = (counter + 1) % LEN;
    if (total < LEN) ++total;
  }

  //Get the current index. Will always be a value from 0 to the length of the buffer
  int index() const {
    return counter;
  }

  //Get a reference to the item at the current index
  TYPE& current() {
    return data[counter];
  }

  //Get the total number of items in the buffer
  int count() const {
    return total;
  }

  //Move to the previous spot in the buffer
  void prev() {
    counter = (counter + LEN - 1) % LEN;
  }

  //Move to the next spot in the buffer
  void next() {
    counter = (counter + 1) % LEN;
  }

  //Helper method for C++ range based for loops
  iterator begin() const {
    return iterator(data, counter + LEN + LEN - total);
  }

  //Helper method for C++ range based for loops
  iterator end() const {
    return iterator(data, counter + LEN + LEN);
  }

  //Fill the buffer with the contents of an array.
  //This allows for brace-enclosed initialization e.g.
  //CircularBuffer<int, 3> buffer = {1, 2, 3};
  template<typename... ARGS>
  void populate(const TYPE& first, const ARGS&... args) {
    append(first);
    populate(args...);
  }

  //Tail end of brace-enclosed initialization.
  void populate(const TYPE& arg) {
    append(arg);
  }

  //Get the minimum value contained in this buffer.
  //This method is only available for arithmetic types.
  TYPE minimum() const {
    static_assert(std::is_arithmetic<TYPE>::value, "CircularBuffer::minimum() is only defined for arithmetic types.");

    TYPE value = data[0];
    for (int i = 1; i < LEN; ++i)
    {
      if (data[i] < value) value = data[i];
    }
    return value;
  }

  //Get the maximum value contained in this buffer.
  //This method is only available for arithmetic types.
  TYPE maximum() const {
    static_assert(std::is_arithmetic<TYPE>::value, "CircularBuffer::maximum() is only defined for arithmetic types.");

    TYPE value = data[0];
    for (int i = 1; i < LEN; ++i)
    {
      if (data[i] > value) value = data[i];
    }
    return value;
  }
};

#endif
