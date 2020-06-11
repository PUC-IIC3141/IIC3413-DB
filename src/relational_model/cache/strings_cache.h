#ifndef RELATIONAL_MODEL__STRINGS_CACHE_H_
#define RELATIONAL_MODEL__STRINGS_CACHE_H_

#include <map>
#include <list>
#include <utility>
#include <cstdint>
#include <memory>
#include <string>

#include "base/graph/value/value_string.h"

class StringsCache {
private:
    std::map<uint64_t, std::pair<std::string, std::list<uint64_t>::iterator> > m_map;
    std::list<uint64_t> m_list;
    size_t cache_size;

public:
    StringsCache(size_t cache_size)
        : cache_size(cache_size) { }

    ~StringsCache() = default;


    void insert(uint64_t key, const std::string& value) {
        auto i = m_map.find(key);
        if (i == m_map.end()) {
            // insert item into the cache, but first check if it is full
            if (m_map.size() >= cache_size){
                // cache is full, evict the least recently used item
                evict();
            }

            // insert the new item
            m_list.push_front(key);
            m_map[key] = std::make_pair(value, m_list.begin());
        }
    }


    std::shared_ptr<ValueString> get(uint64_t key) {
        // lookup value in the cache
        auto i = m_map.find(key);
        if (i == m_map.end()){
            // value not in cache
            return nullptr;
        }

        // return the value, but first update its place in the most
        // recently used list
        auto j = i->second.second;
        if(j != m_list.begin()){
            // move item to the front of the most recently used list
            m_list.erase(j);
            m_list.push_front(key);

            // update iterator in map
            j = m_list.begin();
            const std::string& value = i->second.first;
            m_map[key] = std::make_pair(value, j);

            // return the value
            return std::make_shared<ValueString>(value);
        }
        else {
            // the item is already at the front of the most recently
            // used list so just return it
            return std::make_shared<ValueString>(i->second.first);
        }
    }


    void clear() {
        m_map.clear();
        m_list.clear();
    }

    void evict() {
        // evict item from the end of most recently used list
        auto i = --m_list.end();
        m_map.erase(*i);
        m_list.erase(i);
    }
};

#endif // RELATIONAL_MODEL__STRINGS_CACHE_H_
