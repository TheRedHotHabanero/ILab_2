#pragma once

#include <unordered_map>
#include <list>
#include <cassert>

namespace LFU
{
  using std::unordered_map;
  using std::list;

  template <typename KeyT>
  struct FreqElem;

  template <typename KeyT>
  using FreqIt  = typename list<FreqElem<KeyT>>::iterator;
  using FreqT   = unsigned long long;

  template <typename KeyT>
  struct NodeElem
  {
    KeyT value_;
    FreqIt<KeyT> head_;
    NodeElem(KeyT value, FreqIt<KeyT> head): value_(value), head_(head) {}
  };

  template<typename KeyT>
  struct FreqElem
  {
    int hits_;
    list<NodeElem<KeyT>> NodeList;
    FreqElem(int hits): hits_(hits), NodeList() {}
  };

  template <typename KeyT>
  class LFUCache
  {
    private:
      using NodeIt    = typename list<NodeElem<KeyT>>::iterator;
      using HashTable = unordered_map<KeyT, NodeIt>;
      using HashIt    = typename HashTable::iterator;

      size_t                size_;
      HashTable             hash_table_;
      list<FreqElem<KeyT>>  freq_list_;

      void funding_hits(NodeIt new_node_it)
      {
        FreqIt<KeyT> node_head(new_node_it->head_);
        FreqIt<KeyT> node_head_p(++node_head);
        node_head--;

        if (node_head_p != freq_list_.end() && node_head->hits_ + 1 == node_head_p->hits_)
        {
          node_head_p->NodeList.push_front(NodeElem<KeyT>(new_node_it->value_, node_head_p));
          tying_node(new_node_it, node_head, node_head_p);
          return;
        }

        FreqIt<KeyT> new_head = freq_list_.insert(node_head_p, FreqElem<KeyT>(node_head->hits_ + 1));
        new_head->NodeList.push_front(NodeElem<KeyT>(new_node_it->value_, new_head));
        tying_node(new_node_it, node_head, new_head);
      }

      void tying_node(NodeIt &node, FreqIt<KeyT> &current, FreqIt<KeyT> &destination)
      {
        hash_table_.erase(node->value_);
        hash_table_[node->value_] = destination->NodeList.begin();
        current->NodeList.erase(node);
        if (current->NodeList.empty())
          freq_list_.erase(current);
      }

      void adding_value(KeyT value)
      {
        if (hash_table_.size() >= size_)
          delete_min_freq();
        adding_freq(value);
        hash_table_[value] = freq_list_.front().NodeList.begin();
      }

      void adding_freq(KeyT value)
      {
        if (freq_list_.empty() || freq_list_.front().hits_ != 0)
          freq_list_.push_front(FreqElem<KeyT>(0));
        freq_list_.front().NodeList.push_front(NodeElem<KeyT>(value, freq_list_.begin()));
      }

      void delete_min_freq()
      {
        hash_table_.erase(freq_list_.front().NodeList.front().value_);
        freq_list_.front().NodeList.pop_front();

        if (freq_list_.front().NodeList.empty())
          freq_list_.pop_front();
      }

    public:
      LFUCache(size_t size): size_(size) {}

      bool process_elem(const KeyT &value)
      {
        auto key_list_it  = hash_table_.find(value);
        if (key_list_it != hash_table_.end())
        {
          funding_hits(key_list_it->second);
          return true;
        }
        adding_value(value);
        return false;
      }
  };
}; //namespace LFU
