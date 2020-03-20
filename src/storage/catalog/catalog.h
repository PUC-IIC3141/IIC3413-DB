#ifndef STORAGE__CATALOG_H_
#define STORAGE__CATALOG_H_

#include <fstream>
#include <memory>
#include <map>
#include <string>

class Catalog {
public:
    Catalog(const std::string& filename);
    ~Catalog();

    // Create_node and create_edge return the new id without the corresponding mask
    // boths methods must skip id = 0 (reserved for null ids)
    uint64_t create_node();
    uint64_t create_edge();

    uint64_t get_node_count() { return node_count; };
    uint64_t get_edge_count() { return edge_count; };

    // ALL IDS ARE UNMASKED
    void add_node_label(uint64_t label_id);
    void add_edge_label(uint64_t label_id);
    void add_node_key(uint64_t key_id);
    void add_edge_key(uint64_t key_id);

    uint64_t get_node_count_for_label(uint64_t label_id);
    uint64_t get_edge_count_for_label(uint64_t label_id);

    uint64_t get_node_count_for_key(uint64_t key_id);
    uint64_t get_edge_count_for_key(uint64_t key_id);

private:
    std::fstream& file;

    uint64_t node_count;
    uint64_t edge_count;
    uint64_t node_label_count;
    uint64_t edge_label_count;
    uint64_t node_key_count;
    uint64_t edge_key_count;

    std::map<uint64_t, uint64_t> node_label_stats;
    std::map<uint64_t, uint64_t> edge_label_stats;
    std::map<uint64_t, uint64_t> node_key_stats;
    std::map<uint64_t, uint64_t> edge_key_stats;

    void save_changes();
    void add_to_map(std::map<uint64_t, uint64_t>& map, uint64_t key, uint64_t& count);
    uint64_t get_count(std::map<uint64_t, uint64_t>& map, uint64_t key);
    uint64_t read();
    std::pair<uint64_t, uint64_t> read_pair();
};

#endif // STORAGE__CATALOG_H_
