/**
 * Catalog saves:
 * - number of graphs
 * - graph names and ids
 *
 **/

#ifndef STORAGE__CATALOG_H_
#define STORAGE__CATALOG_H_

#include <fstream>
#include <memory>
#include <map>
#include <string>
#include <vector>

#include "base/ids/graph_id.h"

class Catalog {
public:
    Catalog(const std::string& filename);
    ~Catalog();

    // Create_node and create_edge return the new id without the corresponding mask
    // boths methods must skip id = 0 (reserved for null ids)
    uint64_t create_node(GraphId);
    uint64_t create_edge(GraphId);

    uint64_t get_node_count(GraphId);
    uint64_t get_edge_count(GraphId);

    // ALL IDS ARE UNMASKED
    void add_node_label(GraphId, uint64_t label_id);
    void add_edge_label(GraphId, uint64_t label_id);
    void add_node_key(GraphId, uint64_t key_id);
    void add_edge_key(GraphId, uint64_t key_id);

    uint64_t get_node_count_for_label(GraphId, uint64_t label_id);
    uint64_t get_edge_count_for_label(GraphId, uint64_t label_id);

    uint64_t get_node_count_for_key(GraphId, uint64_t key_id);
    uint64_t get_edge_count_for_key(GraphId, uint64_t key_id);

    GraphId get_graph(const std::string& graph_name);

    GraphId create_graph(const std::string& graph_name);

    void print();

private:
    std::fstream& file;
    uint32_t graph_count;

    std::vector<std::string> graph_names;
    std::map<std::string, GraphId> graph_ids;

    std::vector<uint64_t> node_count;
    std::vector<uint64_t> edge_count;
    std::vector<uint64_t> node_label_count;
    std::vector<uint64_t> edge_label_count;
    std::vector<uint64_t> node_key_count;
    std::vector<uint64_t> edge_key_count;

    std::vector<std::map<uint64_t, uint64_t>> node_label_stats;
    std::vector<std::map<uint64_t, uint64_t>> edge_label_stats;
    std::vector<std::map<uint64_t, uint64_t>> node_key_stats;
    std::vector<std::map<uint64_t, uint64_t>> edge_key_stats;

    void save_changes();
    void add_to_map(std::map<uint64_t, uint64_t>& map, uint64_t key, uint64_t& count);
    uint64_t get_count(std::map<uint64_t, uint64_t>& map, uint64_t key);
    uint64_t read_uint64();
    uint32_t read_uint32();
};

#endif // STORAGE__CATALOG_H_
