#pragma once

#include <unordered_map>
#include <list>
#include <cassert>
#include <limits>

namespace caches_lfu 
{
  using std::unordered_map;
  using std::list;
  
  template <typename ValueT>
  struct FreqElem;

  template <typename ValueT>
  using FreqIt = typename list<FreqElem<ValueT>>::iterator;

  template <typename ValueT>
  struct NodeElem
  {
    ValueT value_;
    FreqIt<ValueT> head_;
    NodeElem(ValueT value, FreqIt<ValueT> head) : value_(value), head_(head) {}
  };
  
  template<typename ValueT>
  struct FreqElem
  {
    std::list<NodeElem<ValueT>> NodeList;
    const int hits_;
    FreqElem(int hits): NodeList(), hits_(hits) {}
  };

  using FreqT = unsigned long long;
  template <typename KeyT>
  class LFUCache
  {
    private:
    
      size_t capacity_;
      list<FreqElem<KeyT>> FreqList;

      using NodeIt    = typename list<NodeElem<KeyT>>::iterator;
      using HashTable = unordered_map<KeyT, NodeIt>;
      using HashIt    = typename HashTable::iterator;

      HashTable hash_table;
    public:

      LFUCache(size_t capacity): capacity_(capacity), hash_table(), FreqList() {}
  
      bool process_elem(const KeyT &value)
      {
        KeyT value_hash = Hash(value);
        auto value_list_it = hash_table.find(value_hash);

        if (value_list_it != hash_table.end())
        {
          funding_hits(value_list_it->second);
          return true;
        }
        AddCache_(value);
        return false;
      }

    private:
      template <typename ValueT>
      static ValueT Hash( ValueT value )
      { return value; }

      //Increment amount of hits in value function
      void funding_hits(NodeIt new_node_it)
      {
        FreqIt<KeyT> n_head(new_node_it->head_); ////////////////////////////////////////
        FreqIt<KeyT> n_head_p1(++n_head);       ///////////////////////////////////////
        --n_head;                               ///////////////////////////////////////

        if (n_head_p1 != FreqList.end() && n_head->hits_ + 1 == n_head_p1->hits_)
        {
          n_head_p1->NodeList.push_front(NodeElem<KeyT>(new_node_it->value_, n_head_p1));
          ReTie_(new_node_it, n_head, n_head_p1);
          return;
        }

        // Add list with new hits amount
        FreqIt<KeyT> new_head = FreqList.insert(n_head_p1, FreqElem<KeyT>(n_head->hits_ + 1));
        // push node to new freq list
        new_head->NodeList.push_front(NodeElem<KeyT>(new_node_it->value_, new_head));
        ReTie_(new_node_it, n_head, new_head);
      }

      // Retie a node function
      // param node reference to node iterator for retie
      // param current reference to iterator to element of Freq_list, where node is located
      // param destination reference to iterator to element of Freq_list, where node will be located
      void ReTie_(NodeIt &node, FreqIt<KeyT> &current, FreqIt<KeyT> &destination)
      {
        hash_table.erase(node->value_);
        hash_table[node->value_] = destination->NodeList.begin();
        current->NodeList.erase(node);
        if (current->NodeList.empty())
          FreqList.erase(current);
      }

      //Add new value to cache function
      void AddCache_(KeyT value)
      {
        if (hash_table.size() >= capacity_)
          DelMin_();
        AddFreq_(value);
        hash_table[value] = FreqList.front().NodeList.begin();
      }

      void DelMin_(void)
      {
        hash_table.erase(FreqList.front().NodeList.front().value_);
        FreqList.front().NodeList.pop_front();

        if (FreqList.front().NodeList.empty())
          FreqList.pop_front();
      }

      //Add new Frequency to Freq_list function
      void AddFreq_(KeyT value)
      {
        if (FreqList.empty() || FreqList.front().hits_ != 0)
          FreqList.push_front(FreqElem<KeyT>(0));
        FreqList.front().NodeList.push_front(NodeElem<KeyT>(value, FreqList.begin()));
      }
  };    
};  