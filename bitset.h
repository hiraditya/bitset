/*
  Bitset implementation.
*/

class bitset {
  typedef unsigned long bits_ptr_t;

  enum
  {
    BITS_STACK_SIZE = sizeof(bits_stack),
    BITWORD_SIZE = sizeof(bits_ptr_t)
  };

  /* Pointer to the actual bits.  */
  bits_ptr_t *bits_ptr;
  /* For small bitsets, the size is fixed.
     TODO: make bits an array and the size can be passed as template param.  */
  bits_ptr_t bits_stack;
  /* Size of storage in bytes. */
  unsigned bitset_size;
  /* Capacity of storage in bytes. */
  unsigned bitset_capacity;

public:

  /* Reference to a single bit.
     This is useful for indexing a bit in the bitset. */
  class ref
  {
    friend class bitset;
    unsigned idx;
    bits_ptr_t *bitword_ptr;

    /* Cannot use the default constructor. */
    ref();
  
  public:
    /* Reference to the i-th bit in the bitset. */
    ref(bitset *b, unsigned i)
    {
    }

    ref(const ref&) = default;

    ref &operator=(ref t)
    {
    }

    ref &operator=(bool t)
    {
    }

    operator bool() const
    {
    }
  };

  bitset()
      : bits_ptr(&bits_stack), bits_stack(0),
        bitset_size(BITS_STACK_SIZE), bitset_capacity(bitset_size)
  { }

  bitset(unsigned s, bool v = false)
  {
    bitset_size = calc_bitset_size(s);
    bitset_capacity = bitset_size;
    if (bitset_size > calc_bitset_size(BITS_STACK_SIZE*CHAR_BIT))
    {
      bits_ptr = (bits_ptr_t*)XCNEW(bitset_size);
    }
    else
    {
      bits_ptr = &bits_stack;
    }
    init_bitset(bits_ptr, v, bitset_capacity);
  }

  unsigned calc_bitset_size(unsigned s)
  {
    return (s + BITWORD_SIZE - 1)/BITWORD_SIZE;
  }

  void init_bitset(bitset_ptr_t *p, bool v, unsigned capacity)
  {
    memset(p, 0 - (int)v, capacity);
  }

  /* Returns size in bytes */
  unsigned size() const
  {
    return bitset_size;
  }

  /* Returns capacity in bytes */
  unsigned capacity() const
  {
    return bitset_capacity;
  }

  /* Invert one bit in the bitset. */
  void invert(unsigned i)
  {
  }

  /* Invert a range of bits in the bitset. */
  void invert(unsigned i, unsigned j)
  {
  }

  /* Invert all the bits in the bitset. */
  void invert()
  {
  }

  /* Set one bit in the bitset. */
  void set(unsigned i)
  {
  }

  /* Set a range of bits in the bitset. */
  void set(unsigned i, unsigned j)
  {
  }

  /* Set all the bits in the bitset. */
  void set()
  {
  }

  /* Reset one bit in the bitset. */
  void reset(unsigned i)
  {
  }

  /* Reset a range of bits in the bitset. */
  void reset(unsigned i, unsigned j)
  {
  }

  /* Reset all the bits in the bitset. */
  void reset()
  {
  }

  /* Return true if all bits in the bitset are set (=1). */
  bool all() const
  {
  }

  /* Return true if any bit in the bitset is set (=1). */
  bool any() const
  {
  }

  /* Indexing operator defined in terms of class ref. */
  ref operator[](unsigned i)
  {
  }

  bool operator[](unsigned i) const
  {
  }

  const bitset& operator=(const bitset& b)
  {
  }

  /* Equality comparison. */
  bool operator==(const bitset& b) const
  {
  }

  bool operator!=(const bitset& b) const
  {
    return !(*this == b);
  }

  /* Logical operations. */
  bitset& operator|=(const bitset& b)
  {
  }

  bitset& operator&=(const bitset& b)
  {
  }

  bitset& operator^=(const bitset& b)
  {
  }

};

