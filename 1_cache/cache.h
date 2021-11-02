#pragma once

#include <unordered_map>
#include <list>

namespace LFU
{
  using std::list;
  using std::unordered_map;

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
    list<NodeElem<KeyT>> node_list_;
    FreqElem(int hits): hits_(hits), node_list_() {}
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

      void adding_freq(const KeyT &key)
      {
        if (freq_list_.empty() || freq_list_.front().hits_ != 0)
          freq_list_.push_front(FreqElem<KeyT>(0));
        freq_list_.front().node_list_.push_front(NodeElem<KeyT>(key, freq_list_.begin()));
      }

      void delete_min_freq()
      {
        hash_table_.erase(freq_list_.front().node_list_.front().key_);
        freq_list_.front().node_list_.pop_front();

        if (freq_list_.front().node_list_.empty())
          freq_list_.pop_front();
      }

      void adding_key(const KeyT &key)
      {
        if (hash_table_.size() >= size_)
          delete_min_freq();
        adding_freq(key);
        hash_table_[key] = freq_list_.front().node_list_.begin();
      }

      void tying_node(const NodeIt &node, FreqIt<KeyT> &current, const FreqIt<KeyT> &destination)
      {
        hash_table_[node->key_] = destination->node_list_.begin();
        current->node_list_.erase(node);
        if (current->node_list_.empty())
          freq_list_.erase(current);
      }

      void request(NodeIt new_node_it)
      {
        FreqIt<KeyT> node_head{new_node_it->head_};
        FreqIt<KeyT> node_head_p{++node_head};
        node_head--;

        if (node_head_p != freq_list_.end() && (node_head->hits_ + 1) == node_head_p->hits_)
        {
          node_head_p->node_list_.push_front(NodeElem<KeyT>(new_node_it->key_, node_head_p));
          tying_node(new_node_it, node_head, node_head_p);
          return;
        }

        FreqIt<KeyT> new_head = freq_list_.insert(node_head_p, FreqElem<KeyT>(node_head->hits_ + 1));
        new_head->node_list_.push_front(NodeElem<KeyT>(new_node_it->key_, new_head));
        tying_node(new_node_it, node_head, new_head);
      }

    public:
      LFUCache(size_t size): size_(size) {}

      bool process_elem(const KeyT &key)
      {
        auto key_list_it  = hash_table_.find(key);
        if (key_list_it != hash_table_.end())
        {
          request(key_list_it->second);
          return true;
        }
        adding_key(key);
        return false;
      }
  };  // class LFUCache
};    //namespace LFU
