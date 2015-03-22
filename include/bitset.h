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
    /* Number of bits in BITSET_STACK */
    BITSET_STACK_SIZE = sizeof(bitset_stack)*CHAR_BIT,
    /* Number of bits in a word. */
    BITWORD_SIZE = sizeof(bits_ptr_t)*CHAR_BIT,
    BITWORD_ZEROS = 0UL,
    BITWORD_ONES = ~0UL
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
    unsigned num_bytes = bitword_size_bytes(s);
    if (s > BITSET_STACK_SIZE)
    {
      //bits_ptr = (bits_ptr_t*)XCNEW(num_bytes);
      bits_ptr = (bits_ptr_t*)std::malloc(num_bytes);
    }
    else
    {
      bits_ptr = &bitset_stack;
    }
    assert(bits_ptr != NULL);
    bitset_capacity = num_bitwords(s)*BITWORD_SIZE;
    bitset_size = s;
    init_bitset(bits_ptr, v, num_bytes);
  }

  ~bitset()
  {
    if (bits_ptr != &bitset_stack)
      free(bits_ptr);
  }

  /* Number of bitwords required for S bits. */
  unsigned num_bitwords(unsigned s) const
  {
    unsigned t = (s + BITWORD_SIZE - 1)/BITWORD_SIZE;
    // TODO: Remove after testing.
    assert(!(t % CHAR_BIT));
    return t;
  }

  /* Size of bitwords in #bytes.
     FIXME: Can division and mult interchange?
     If yes, then inlining can cause trouble here. */
  unsigned bitword_size_bytes(unsigned s) const
  {
    return num_bitwords(s)*BITWORD_SIZE/CHAR_BIT;
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

  /* Resizes the bitset so that it can hold minumum of CAPACITY bits.
     Does not initialize the values. */
  unsigned resize(unsigned capacity)
  {
    /* When there is enough space then no need to reallocate.
       This however, can be inefficient in cases when the bitset
       has hold on to large memory (this should be quite uncommon). */
    if (bitset_capacity >= capacity)
      return bitset_capacity;

    /* Since bitset_capacity is atleast BITWORD_STACK_SIZE big,
       we would always malloc at this point. */
    if (bits_ptr != &bitset_stack)
      free(bits_ptr);
    bits_ptr = (bits_ptr_t*)std::malloc(bitword_size_bytes(capacity));
    assert(bits_ptr != NULL);
    bitset_capacity = num_bitwords(capacity);
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

    unsigned pro_end = beg / BITWORD_SIZE * BITWORD_SIZE;

    if (pro_end > end)
      pro_end = end;

    for (unsigned r1 = beg; r1 < pro_end; ++r1)
      invert(r1);

    unsigned r2 = beg;
    if (beg % BITWORD_SIZE)
      r2 = beg / BITWORD_SIZE + 1;

    for (; r2 < end / BITWORD_SIZE; ++r2)
      bits_ptr[r2] = ~bits_ptr[r2];

    for (unsigned i = end / BITWORD_SIZE * BITWORD_SIZE; i < end; ++i)
      invert(i);
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

  /* Set a range [beg, end) of bits in the bitset. */
  void set(unsigned beg, unsigned end)
  {
    if (beg % BITWORD_SIZE)
    {
      unsigned pref = BITWORD_ONES >> (beg % BITWORD_SIZE);
      bits_ptr[beg/BITWORD_SIZE] |= pref;
    }

    unsigned i = beg;
    if (beg % BITWORD_SIZE)
      i = beg / BITWORD_SIZE + 1;

    for (; i < end / BITWORD_SIZE; ++i)
      bits_ptr[i] = BITWORD_ONES;

    if (end % BITWORD_SIZE)
    {
      unsigned suff = BITWORD_ONES << (BITWORD_SIZE - end % BITWORD_ONES);
      bits_ptr[end/BITWORD_SIZE] |= suff;
    }
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

  /* Reset a range [beg, end) of bits in the bitset. */
  void reset(unsigned beg, unsigned end)
  {
    if (beg % BITWORD_SIZE)
    {
      unsigned pref = BITWORD_ONES << (BITWORD_SIZE - beg % BITWORD_SIZE);
      bits_ptr[beg/BITWORD_SIZE] &= pref;
    }

    unsigned r2 = beg;
    if (beg % BITWORD_SIZE)
      r2 = beg / BITWORD_SIZE + 1;

    for (; r2 < end / BITWORD_SIZE; ++r2)
      bits_ptr[r2] = BITWORD_ZEROS;

    if (end % BITWORD_SIZE)
    {
      unsigned suff = BITWORD_ONES >> (end % BITWORD_ONES);
      bits_ptr[end/BITWORD_SIZE] &= suff;
    }
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
    bitset_size = b.bitset_size;
    bitset_capacity = resize(b.bitset_capacity);

    assert(bitset_capacity >= b.bitset_capacity);

    memcpy(bits_ptr, b.bits_ptr, bitword_size_bytes(b.bitset_size));
    return *this;
  }

  /* Equality comparison. Strict equality comparison. */
  bool operator==(const bitset& b) const
  {
    if (bitset_size != b.bitset_size)
      return false;
    for (unsigned i = 0; i < bitset_size; ++i)
      if (bits_ptr[i] != b.bits_ptr[i])
        return false;
    return true;
  }

  bool operator!=(const bitset& b) const
  {
    return !(*this == b);
  }

  /* Logical operations. */
  bitset& operator|=(const bitset& b)
  {
    assert(bitset_size == b.bitset_size);
    for (unsigned i = 0; i < num_bitwords(bitset_size); ++i)
      bits_ptr[i] |= b.bits_ptr[i];
    return *this;
  }

  bitset& operator&=(const bitset& b)
  {
    assert(bitset_size == b.bitset_size);
    for (unsigned i = 0; i < num_bitwords(bitset_size); ++i)
      bits_ptr[i] &= b.bits_ptr[i];
    return *this;
  }

  bitset& operator^=(const bitset& b)
  {
    assert(bitset_size == b.bitset_size);
    for (unsigned i = 0; i < num_bitwords(bitset_size); ++i)
      bits_ptr[i] ^= b.bits_ptr[i];
    return *this;
  }

};
