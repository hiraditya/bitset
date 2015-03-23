#include<bitset.h>
#include<cassert>

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
  return 0;
}

int main()
{
  test1();
  test2();
  //test3();
  test4();
  return 0;
}
