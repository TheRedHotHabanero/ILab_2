#pragma once

#include <iostream>
#include <map>
#include <iterator>
#include <cassert>
#include <limits>

using std::map;
using std::cin;
using std::cout;
using std::endl;

namespace caches_lfu 
{
  class CacheLFU 
  {
    using KeyT  = int;
    using FreqT = unsigned long long;
    using MapIt = typename map<KeyT, FreqT>::iterator;

    private:
      size_t size_;
      map<KeyT, FreqT> cache_;
      int hits_;

    public:
      CacheLFU(size_t size) : size_(size), hits_(0) {}

      void delete_min_freq()
      {
        FreqT min_freq = std::numeric_limits<FreqT>::max();
        MapIt min_it = cache_.end();

        for (MapIt curr = cache_.begin(); curr != cache_.end(); ++curr)
        {
          if (curr->second < min_freq)
            {
              min_freq = curr->second;
              min_it = curr;
            }  
        }
        cache_.erase(min_it->first);
      }

      bool put_new(KeyT key)
      {
        if (cache_.count(key) == 0)
          {
            if (cache_.size() == size_)
              delete_min_freq();
              
            cache_.insert({key, 0});
            // if (cache_.size() > size_)
            //   return false;  
          }
        else
          {
            cache_[key]++;
            return true;
          }
        return false;
      }

      int finding_hits(size_t size_)
      {
        for (int curr = 0; curr < size_; ++curr) 
        {
          int new_key;
          cin >> new_key;
          assert(cin.good());

          if (put_new(new_key) == true)
            hits_ += 1;
        }
        return hits_;
      }     
  };
}