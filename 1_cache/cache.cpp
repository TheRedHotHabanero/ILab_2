#include <cassert>

#include "cache.h"

int finding_hits(size_t size_)
{
  int hits = 0;
  caches_lfu::CacheLFU<int> lfu{size_};
  for (int curr = 0; curr < size_; ++curr) 
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
  size_t cache_size = 0;
  int el_quantity = 0;
  int hits = 0;
  std::cin >> cache_size;
  std::cin >> el_quantity;
  assert(std::cin.good());

  caches_lfu::CacheLFU<int> lfu{cache_size};
  hits = finding_hits(el_quantity);
  std::cout << hits << std::endl;

  return 0;
}