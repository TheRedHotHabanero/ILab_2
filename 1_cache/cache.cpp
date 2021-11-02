#include <cassert>
#include <iostream>
#include "cache.h"

int finding_hits(size_t size, size_t el_quantity)
{
  int hits = 0;
  LFU::LFUCache<int> lfu{size};
  for (size_t curr = 0; curr < el_quantity; ++curr)
  {
    int new_key;
    std::cin >> new_key;
    assert(std::cin.good());

    if (lfu.process_elem(new_key) == true)
      hits++;
  }
  return hits;
}

int main()
{
  int     hits        = 0;
  int     el_quantity = 0;
  size_t  cache_size  = 0;
  std::cin >> cache_size;
  std::cin >> el_quantity;
  assert(std::cin.good());

  hits = finding_hits(cache_size, el_quantity);
  std::cout << hits << std::endl;

  return 0;
}