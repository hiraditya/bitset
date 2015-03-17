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
};

