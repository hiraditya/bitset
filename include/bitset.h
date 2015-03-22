/*
  Bitset implementation.
*/

#include<climits>
#include<cassert>
#include<cstdlib>
#include<cstring>

class bitset {
public:
  typedef unsigned long bits_ptr_t;

private:
  /* Pointer to the actual bits.  */
  bits_ptr_t *bits_ptr;
  /* For small bitsets, the size is fixed.
     TODO: make bits an array and the size can be passed as template param.  */
  bits_ptr_t bitset_stack;
  /* Total number of bits. */
  unsigned bitset_size;
  /* Capacity of storage in #bits. */
  unsigned bitset_capacity;

public:

  enum
  {
    BITSET_STACK_SIZE = sizeof(bitset_stack)*CHAR_BIT,
    BITWORD_SIZE = sizeof(bits_ptr_t)*CHAR_BIT
  };

  /* Reference to a single bit.
     This is useful for indexing a bit in the bitset. */
  class ref
  {
    friend class bitset;

    /* Pointer to one unit. */
    bits_ptr_t *bitword_ptr;
    /* Index of bit in the bitword. */
    unsigned idx;

    /* Cannot use the default constructor. */
    ref();

  public:
    /* Reference to the i-th bit in the bitset B. */
    ref(bitset *b, unsigned i)
      : bitword_ptr(&b->bits_ptr[i/BITWORD_SIZE]),
        idx(i%BITWORD_SIZE)
    { }

    ref(const ref&) = default;

    /* Get a reference to the I-th bit in the bitset B. */
    ref& get(bitset *b, unsigned i)
    {
      bits_ptr_t *bp = &b->bits_ptr[i/BITWORD_SIZE];
      unsigned ip = i%BITWORD_SIZE;
      // If pointing to the same bitword.
      if (bitword_ptr == bp)
      {
        /* If pointing to the same bit. */
        if (idx == ip)
          return *this;
        idx = ip;
        return *this;
      }
      bitword_ptr = bp;
      idx = ip;
      return *this;
    }

    /* Get a reference to the I-th bit in the bitset B. */
    bool test(const bitset *b, unsigned i) const
    {
      const bits_ptr_t *bp = &b->bits_ptr[i/BITWORD_SIZE];
      unsigned idx = i%BITWORD_SIZE;
      return (*bitword_ptr) & (bits_ptr_t(1) << idx);
    }

    ref &operator=(ref &t)
    {
      *this = bool(t);
      return *this;
    }

    ref &operator=(bool t)
    {
      bits_ptr_t &bitword = *bitword_ptr;
      bits_ptr_t mask = bits_ptr_t(1) << idx;
      if (t)
        bitword |= mask;
      else
        bitword &= ~mask;
      return *this;
    }

    operator bool() const
    {
      return (*bitword_ptr) & (bits_ptr_t(1) << idx);
    }
  };

private:
  /* Reference to a bit. */
  ref bits_ref;

public:

  bitset()
    : bits_ptr(&bitset_stack), bitset_stack(0),
      bitset_size(BITSET_STACK_SIZE), bitset_capacity(bitset_size),
      bits_ref(this, 0)
  { }

  bitset(unsigned s, bool v = false)
    : bits_ref(this, 0)
  {
    bitset_size = calc_bitset_size(s);
    bitset_capacity = bitset_size;
    unsigned num_bytes = calc_bitset_size(bitset_size)/sizeof(bits_ptr_t);
    if (bitset_size > calc_bitset_size(BITSET_STACK_SIZE*CHAR_BIT))
    {
      //bits_ptr = (bits_ptr_t*)XCNEW(bitset_size);
      bits_ptr = (bits_ptr_t*)std::malloc(num_bytes);
    }
    else
    {
      bits_ptr = &bitset_stack;
    }
    init_bitset(bits_ptr, v, num_bytes);
  }

  ~bitset()
  {
    if (bits_ptr != &bitset_stack)
      free(bits_ptr);
  }

  /* Number of bitwords required for S bits. */
  unsigned calc_bitset_size(unsigned s) const
  {
    return (s + BITWORD_SIZE - 1)/BITWORD_SIZE;
  }

  void init_bitset(bits_ptr_t *p, bool v, unsigned num)
  {
    ::memset(p, 0 - (int)v, num);
  }

  /* Returns size in #bits */
  unsigned size() const
  {
    return bitset_size;
  }

  /* Returns capacity in #bits */
  unsigned capacity() const
  {
    return bitset_capacity;
  }

  /* Invert one bit in the bitset. */
  void invert(unsigned i)
  {
    bits_ptr[i/BITWORD_SIZE] ^= bits_ptr_t(1) << (i % BITWORD_SIZE);
  }

  /* Invert a range of bits in the bitset.
     End is one past the end element we want to invert. */
  void invert(unsigned beg, unsigned end)
  {
    assert(beg < end);

    unsigned pro_end = beg + (BITWORD_SIZE - beg % BITWORD_SIZE) % BITWORD_SIZE;
    unsigned epi_end = end % BITWORD_SIZE;

    for (unsigned i = beg; i < pro_end; ++i)
      invert(i);

    assert(!(pro_end % BITWORD_SIZE));

    for (unsigned i = 0; i < (end-beg)/BITWORD_SIZE; ++i)
      bits_ptr[i + beg/BITWORD_SIZE + 1] = ~bits_ptr[i + beg/BITWORD_SIZE + 1];

    for (unsigned i = 0; i < epi_end; ++i)
      invert(end-i-1);
  }

  /* Invert all the bits in the bitset. */
  void invert()
  {
    invert(0, bitset_size);
  }

  /* Set one bit in the bitset. */
  void set(unsigned i)
  {
    (*this)[i] = true;
  }

  /* Set a range of bits in the bitset. */
  void set(unsigned i, unsigned j)
  {
  }

  /* Set all the bits in the bitset. */
  void set()
  {
    set(0, bitset_size);
  }

  /* Reset one bit in the bitset. */
  void reset(unsigned i)
  {
    (*this)[i] = false;
  }

  /* Reset a range of bits in the bitset. */
  void reset(unsigned i, unsigned j)
  {
  }

  /* Reset all the bits (only the used ones) in the bitset. */
  void reset()
  {
    reset(0, bitset_size);
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
  ref& operator[](unsigned i)
  {
    return bits_ref.get(this, i);
  }

  bool operator[](unsigned i) const
  {
    return bits_ref.test(this, i);
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
    assert(bitset_size == b.bitset_size);
    for (unsigned i = 0; i < calc_bitset_size(bitset_size); ++i)
      bits_ptr[i] |= b.bits_ptr[i];
    return *this;
  }

  bitset& operator&=(const bitset& b)
  {
    assert(bitset_size == b.bitset_size);
    for (unsigned i = 0; i < calc_bitset_size(bitset_size); ++i)
      bits_ptr[i] &= b.bits_ptr[i];
    return *this;
  }

  bitset& operator^=(const bitset& b)
  {
    assert(bitset_size == b.bitset_size);
    for (unsigned i = 0; i < calc_bitset_size(bitset_size); ++i)
      bits_ptr[i] ^= b.bits_ptr[i];
    return *this;
  }

};
