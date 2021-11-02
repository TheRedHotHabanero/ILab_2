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
    KeyT key_;
    FreqIt<KeyT> head_;
    NodeElem(KeyT key, FreqIt<KeyT> head): key_(key), head_(head) {}
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

      void adding_key(KeyT key)
      {
        if (hash_table_.size() >= size_)
          delete_min_freq();
        adding_freq(key);
        hash_table_[key] = freq_list_.front().NodeList.begin();
      }

      void adding_freq(KeyT key)
      {
        if (freq_list_.empty() || freq_list_.front().hits_ != 0)
          freq_list_.push_front(FreqElem<KeyT>(0));
        freq_list_.front().NodeList.push_front(NodeElem<KeyT>(key, freq_list_.begin()));
      }

      void delete_min_freq()
      {
        hash_table_.erase(freq_list_.front().NodeList.front().key_);
        freq_list_.front().NodeList.pop_front();

        if (freq_list_.front().NodeList.empty())
          freq_list_.pop_front();
      }

      void tying_node(NodeIt &node, FreqIt<KeyT> &current, FreqIt<KeyT> &destination)
      {
        hash_table_.erase(node->key_);
        hash_table_[node->key_] = destination->NodeList.begin();
        current->NodeList.erase(node);
        if (current->NodeList.empty())
          freq_list_.erase(current);
      }

      void funding_hits(NodeIt new_node_it)
      {
        FreqIt<KeyT> node_head(new_node_it->head_);
        FreqIt<KeyT> node_head_p(++node_head);
        node_head--;

        if (node_head_p != freq_list_.end() && node_head->hits_ + 1 == node_head_p->hits_)
        {
          node_head_p->NodeList.push_front(NodeElem<KeyT>(new_node_it->key_, node_head_p));
          tying_node(new_node_it, node_head, node_head_p);
          return;
        }

        FreqIt<KeyT> new_head = freq_list_.insert(node_head_p, FreqElem<KeyT>(node_head->hits_ + 1));
        new_head->NodeList.push_front(NodeElem<KeyT>(new_node_it->key_, new_head));
        tying_node(new_node_it, node_head, new_head);
      }

    public:
      LFUCache(size_t size): size_(size) {}

      bool process_elem(const KeyT &key)
      {
        auto key_list_it  = hash_table_.find(key);
        if (key_list_it != hash_table_.end())
        {
          funding_hits(key_list_it->second);
          return true;
        }
        adding_key(key);
        return false;
      }
  };
}; //namespace LFU
