/* Bitset implementation.

  Copyright [2015-] Aditya Kumar.
  This file is distributed under the GPL v3 License. See LICENSE for details.
*/

#include <climits>
#include <cassert>
#include <cstdlib>
#include <cstring>

/* Only to make it compatible with llvm/bitset
   so that llvm/ADT google tests can work.
#define invert flip  */

class bitset {
public:
  typedef unsigned long bits_ptr_t;

private:
  /* Pointer to the actual bits.  */
  bits_ptr_t *bits_ptr;

  /* For small bitsets, the size is fixed.
     TODO: make bits an array and the size can be passed as template param.  */
  bits_ptr_t small_bitset;

  /* Storage (#bits) exposed to the user. */
  unsigned bitset_size;

  /* Capacity of storage in #bits. */
  unsigned bitset_capacity;

public:

  enum
  {
    /* Number of bits in SMALL_BITSET */
    SMALL_BITSET_SIZE = sizeof(small_bitset) * CHAR_BIT,
    /* Number of bits in a word. */
    BITWORD_SIZE = sizeof(bits_ptr_t) * CHAR_BIT,
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
      : bitword_ptr(&b->bits_ptr[i / BITWORD_SIZE]),
        idx(i % BITWORD_SIZE)
    { }

    ref(const ref&) = default;

    ref &operator=(ref &t)
    {
      //*this = bool(t);

      //second method: bitword_ptr = t.bitword_ptr;
      //idx = t.idx;

      bits_ptr_t *bp = t.bitword_ptr;
      unsigned ip = t.idx;
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

    ref &operator=(bool t)
    {
      bits_ptr_t &bitword = *bitword_ptr;
      bits_ptr_t mask = bits_ptr_t(1) << (BITWORD_SIZE - idx - 1);
      if (t)
        bitword |= mask;
      else
        bitword &= ~mask;
      return *this;
    }

    operator bool() const
    {
      return (*bitword_ptr) & (bits_ptr_t(1) << (BITWORD_SIZE - idx - 1));
    }
  };

  bitset()
    : bits_ptr(&small_bitset), small_bitset(0),
      bitset_size(0), bitset_capacity(SMALL_BITSET_SIZE)
  { }

  bitset(unsigned s, bool v = false)
  {
    unsigned num_bytes = bitword_size_bytes(s);
    if (s > SMALL_BITSET_SIZE)
    {
      //bits_ptr = (bits_ptr_t*)XCNEW(num_bytes);
      bits_ptr = (bits_ptr_t *)std::malloc(num_bytes);
    }
    else
    {
      bits_ptr = &small_bitset;
    }

    assert(bits_ptr != NULL);

    bitset_capacity = num_bitwords(s) * BITWORD_SIZE;
    bitset_size = s;
    init_bitset(bits_ptr, v, num_bytes);
  }

  bitset(const bitset& b)
    : bitset_size(b.bitset_size)
  {
    unsigned num_bytes = bitword_size_bytes(bitset_size);
    if (bitset_size > SMALL_BITSET_SIZE)
    {
      //bits_ptr = (bits_ptr_t*)XCNEW(num_bytes);
      bits_ptr = (bits_ptr_t *)std::malloc(num_bytes);
    }
    else
    {
      bits_ptr = &small_bitset;
    }

    assert(bits_ptr != NULL);

    bitset_capacity = num_bitwords(bitset_size) * BITWORD_SIZE;
    *this = b;
  }

  ~bitset()
  {
    if (bits_ptr != &small_bitset)
      free(bits_ptr);
  }

  /* Number of bitwords required for S bits. */
  unsigned num_bitwords(unsigned s) const
  {
    unsigned t = (s + BITWORD_SIZE - 1) / BITWORD_SIZE;
    return t;
  }

  /* Size of bitwords in #bytes.
     FIXME: Can division and mult interchange?
     If yes, then inlining can cause trouble here. */
  unsigned bitword_size_bytes(unsigned s) const
  {
    return num_bitwords(s) * BITWORD_SIZE / CHAR_BIT;
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

  bool empty() const
  {
    return !size();
  }

  /* Resizes the bitset so that it can hold minumum of CAPACITY bits.
     Does not initialize the values.
     TODO: Make this function private.  */
  unsigned resize_capacity(unsigned capacity)
  {
    /* When there is enough space then no need to reallocate.
       This however, can be inefficient in cases when the bitset
       has hold on to large memory (this should be quite uncommon). */
    if (bitset_capacity >= capacity)
      return bitset_capacity;

    /* Since bitset_capacity is atleast BITWORD_STACK_SIZE big,
       we would always malloc at this point. */
    if (bits_ptr != &small_bitset)
    {
      assert(bits_ptr != NULL);

      free(bits_ptr);
    }
    bits_ptr = (bits_ptr_t *)std::malloc(bitword_size_bytes(capacity));

    assert(bits_ptr != NULL);

    bitset_capacity = num_bitwords(capacity) * BITWORD_SIZE;
    return bitset_capacity;
  }

  // TODO: Check this.
  unsigned resize(unsigned N, bool init = false)
  {
    unsigned old_capacity = bitset_capacity;
    if (old_capacity < num_bitwords(N) * BITWORD_SIZE)
    {
      resize_capacity(num_bitwords(N));
      init_bitset(bits_ptr + old_capacity, init,
                  bitword_size_bytes(bitset_capacity - old_capacity));
    }

    return bitset_capacity;
  }

  /* Invert one bit in the bitset. */
  bitset &invert(unsigned i)
  {
    bits_ptr[i/BITWORD_SIZE] ^= bits_ptr_t(1)
            << (BITWORD_SIZE - (i % BITWORD_SIZE) - 1);
    return *this;
  }

  /* Invert a range of bits in the bitset.
     End is one past the end element we want to invert. */
  bitset &invert(unsigned beg, unsigned end)
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
    return *this;
  }

  /* Invert all the bits in the bitset. */
  bitset &invert()
  {
    invert(0, bitset_size);
    return *this;
  }

  /* Set the reference to the I-th bit in the bitset B. */
  static ref& fast_set(ref &r, bitset *b, unsigned i)
  {
    bits_ptr_t *bp = &b->bits_ptr[i / BITWORD_SIZE];
    unsigned ip = i % BITWORD_SIZE;
    // If pointing to the same bitword.
    if (r.bitword_ptr == bp)
    {
      /* If pointing to the same bit. */
      if (r.idx == ip)
        return r;
      r.idx = ip;
      return r;
    }
    r.bitword_ptr = bp;
    r.idx = ip;
    return r;
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
      bits_ptr_t pref = BITWORD_ONES >> (beg % BITWORD_SIZE);
      bits_ptr[beg / BITWORD_SIZE] |= pref;
    }

    unsigned i = beg;
    if (beg % BITWORD_SIZE)
      i = beg / BITWORD_SIZE + 1;

    for (; i < end / BITWORD_SIZE; ++i)
      bits_ptr[i] = BITWORD_ONES;

    if (end % BITWORD_SIZE)
    {
      bits_ptr_t suff = BITWORD_ONES << (BITWORD_SIZE - end % BITWORD_ONES);
      bits_ptr[end / BITWORD_SIZE] |= suff;
    }
  }

  /* Set all the bits in the bitset. */
  void set()
  {
    set(0, bitset_size);
  }

  /* Reset one bit in the bitset. */
  bitset &reset(unsigned i)
  {
    (*this)[i] = false;
    return *this;
  }

  /* Reset a range [beg, end) of bits in the bitset. */
  bitset &reset(unsigned beg, unsigned end)
  {
    if (beg % BITWORD_SIZE)
    {
      bits_ptr_t pref = BITWORD_ONES << (BITWORD_SIZE - beg % BITWORD_SIZE);
      bits_ptr[beg / BITWORD_SIZE] &= pref;
    }

    unsigned r2 = beg;
    if (beg % BITWORD_SIZE)
      r2 = beg / BITWORD_SIZE + 1;

    for (; r2 < end / BITWORD_SIZE; ++r2)
      bits_ptr[r2] = BITWORD_ZEROS;

    if (end % BITWORD_SIZE)
    {
      bits_ptr_t suff = BITWORD_ONES >> (end % BITWORD_ONES);
      bits_ptr[end / BITWORD_SIZE] &= suff;
    }
    return *this;
  }

  /* Reset all the bits (only the used ones) in the bitset. */
  bitset &reset()
  {
    reset(0, bitset_size);
    return *this;
  }

  void clear()
  {
    bitset_size = 0;
  }

  /* Return true if all bits in the bitset are set (=1). */
  bool all() const
  {
    for (unsigned i = 0; i < num_bitwords(size()); ++i)
      if (bits_ptr[i] != BITWORD_ONES)
        return false;
    return true;
  }

  /* Return true if any bit in the bitset is set (=1). */
  bool any() const
  {
    for (unsigned i = 0; i < num_bitwords(size()); ++i)
      if (bits_ptr[i] != BITWORD_ZEROS)
        return true;
    return false;
  }

  bool none() const
  {
    return !any();
  }

  unsigned count() const
  {
    unsigned n = 0;
    for (unsigned i = 0; i < size(); ++i)
      if ((*this)[i])
        ++n;
    return n;
    //for (unsigned i = 0; i < num_bitwords(size()); ++i)
    //return __builtin_popcount()
  }

  bool test(unsigned i) const
  {
    const bits_ptr_t *bp = &bits_ptr[i / BITWORD_SIZE];
    unsigned idx = i % BITWORD_SIZE;
    return (*bp) & (bits_ptr_t(1) << (BITWORD_SIZE - idx - 1));
  }

  bool operator[](unsigned i) const
  {
    return test(i);
  }

  /* Indexing operator defined in terms of class ref. */
  ref operator[](unsigned i)
  {
    return ref(this, i);
  }

  const bitset& operator=(const bitset& b)
  {
    bitset_size = b.bitset_size;
    bitset_capacity = resize_capacity(b.bitset_capacity);

    assert(bitset_capacity >= b.bitset_capacity);

    memcpy(bits_ptr, b.bits_ptr, bitword_size_bytes(b.bitset_size));
    return *this;
  }

  /* Equality comparison. Strict equality comparison.
     FIXME: This is highly inefficient. Improve this by initializing
     unused bits in the bitset to zero.  */
  bool operator==(const bitset& b) const
  {
    if (bitset_size != b.bitset_size)
      return false;
    for (unsigned i = 0; i < bitset_size; ++i)
      if ((*this)[i] != b[i])
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
