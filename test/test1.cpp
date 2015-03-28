#include<bitset.h>
#include<cassert>
#include<iostream>

void print_bit(const bitset &b, unsigned i)
{
  std::cout << "\nTesting bit:" << i << ": ";
  if (b[i])
    std::cout<<"True";
  else
    std::cout<<"False";
}

int test1()
{
  bitset b;
  assert(b.size() == sizeof(unsigned long)*CHAR_BIT);
  return b[0];
}

int test2()
{
  bitset b(64, true);
  assert(b.size() == sizeof(unsigned long)*CHAR_BIT);
  return b[0];
}

int test3()
{
  const int sz = 65;
  bitset b(sz, false);

  for (int i = 0; i < sz; ++i)
    assert(b[i] == false);

  for (int i = 0; i < sz; ++i)
    b[i] = true;

  for (int i = 0; i < sz; ++i)
    assert(b[i] == true);

  b.reset();
  for (int i = 0; i < sz; ++i)
    assert(b[i] == false);

  b.set();
  for (int i = 0; i < sz; ++i)
    assert(b[i] == true);
  return 0;
}

int test4()
{
  const int sz = 100;
  bitset b(sz, false);

  b.reset(5, 70);
  for (int i = 5; i < 70; ++i)
    assert(b[i] == false);

  b.set(10, 80);
  for (int i = 10; i < 80; ++i)
    assert(b[i] == true);

  for (int i = 0; i < 9; ++i)
    assert(b[i] == false);

  for (int i = 81; i < 99; ++i)
    assert(b[i] == false);

  return 0;
}

int test5()
{
  const int sz = 100;
  bitset b1(sz, false);
  bitset b2(sz, true);

  b2 &= b1;
  assert(b1 == b2);

  b2.set(10, 50);
  bitset temp1(b2);
  b2 |= b1;
  assert(b1 != b2);
  assert(temp1 == b2);

  b2.set();
  bitset temp2 = temp1;
  temp1 ^= b2;
  temp2.invert();
  assert(temp2 == temp1);
  return 0;
}

int main()
{
  test1();
  test2();
  test3();
  test4();
  test5();
  return 0;
}
