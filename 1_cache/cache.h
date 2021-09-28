#pragma once

#include <iostream>
#include <unordered_map>
#include <list>
#include <iterator>
#include <cassert>
#include <limits>


namespace caches_lfu 
{
  using std::unordered_map;
  using std::list;
  using std::cin;
  using std::cout;
  using std::endl;

  template <typename KeyT = int>
  class CacheLFU 
  {
    using FreqT = unsigned long long;
    using MapIt = typename unordered_map<KeyT, FreqT>::iterator;
    using ListIt = typename list<KeyT>::iterator;

    private:
      size_t size_;
      unordered_map<KeyT, FreqT> cache_;
      list<KeyT> list_;

    public:
      CacheLFU(size_t size) : size_(size){}

      void delete_min_freq()
      {
        FreqT min_freq = std::numeric_limits<FreqT>::max();
        MapIt min_it_map = cache_.end();
        ListIt min_it_list = list_.end();

        for (ListIt curr_list = list_.begin(); curr_list != list_.end(); ++curr_list)
        {
          MapIt find_res = cache_.find(*curr_list);
          assert(find_res != cache_.end());

          if (find_res->second < min_freq)
          {
            min_freq = find_res->second;
            min_it_map = find_res;
            min_it_list = curr_list;
          }
        } 
        cache_.erase(min_it_map);
        list_.erase(min_it_list);
      }

      bool process_elem(KeyT key) 
      {
        if (cache_.count(key) != 0)
        {
          cache_[key]++;
          return true;
        }
        if (cache_.size() == size_)
          delete_min_freq();
        cache_.insert({key, 0});
        list_.push_front(key);
        return false;
      }
  };
}