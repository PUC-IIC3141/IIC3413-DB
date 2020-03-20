#include "catalog.h"

#include <iostream>

#include "storage/file_manager.h"

using namespace std;

Catalog::Catalog(const string& filename)
    : file(file_manager.get_file(file_manager.get_file_id(filename)))
{
    file.seekg (0, file.end);
    if (file.tellg() == 0) {
        // Initialize with zeros
        node_count = 0;
        edge_count = 0;
        node_label_count = 0;
        edge_label_count = 0;
        node_key_count = 0;
        edge_key_count = 0;
    }
    else {
        file.seekg(0, file.beg);

        node_count = read();
        edge_count = read();
        node_label_count = read();
        edge_label_count = read();
        node_key_count = read();
        edge_key_count = read();

        for (uint32_t i = 0; i < node_label_count; i++) {
            node_label_stats.insert(read_pair());
        }
        for (uint32_t i = 0; i < edge_label_count; i++) {
            edge_label_stats.insert(read_pair());
        }
        for (uint32_t i = 0; i < node_key_count; i++) {
            node_key_stats.insert(read_pair());
        }
        for (uint32_t i = 0; i < edge_key_count; i++) {
            edge_key_stats.insert(read_pair());
        }
    }
}


Catalog::~Catalog() {
    save_changes();
}


void Catalog::save_changes(){
    file.seekg(0, file.beg);
    cout << "Saving catalog:" << endl;
    cout << "  node count: " << node_count << endl;
    cout << "  edge count: " << edge_count << endl;
    cout << "  node disinct labels: " << node_label_count << endl;
    cout << "  edge disinct labels: " << edge_label_count << endl;
    cout << "  node disinct keys:   " << node_key_count << endl;
    cout << "  edge disinct keys:   " << edge_key_count << endl;

    file.write((const char *)&node_count, 8);
    file.write((const char *)&edge_count, 8);
    file.write((const char *)&node_label_count, 8);
    file.write((const char *)&edge_label_count, 8);
    file.write((const char *)&node_key_count, 8);
    file.write((const char *)&edge_key_count, 8);

    for (auto&& [id, count] : node_label_stats) {
        file.write((const char *)&id, 8);
        file.write((const char *)&count, 8);
    }
    for (auto&& [id, count] : edge_label_stats) {
        file.write((const char *)&id, 8);
        file.write((const char *)&count, 8);
    }
    for (auto&& [id, count] : node_key_stats) {
        file.write((const char *)&id, 8);
        file.write((const char *)&count, 8);
    }
    for (auto&& [id, count] : edge_key_stats) {
        file.write((const char *)&id, 8);
        file.write((const char *)&count, 8);
    }
}


uint64_t Catalog::read() {
    char buffer[8];
    file.read(buffer, 8);
    return *(uint64_t *)buffer;
}


pair<uint64_t, uint64_t> Catalog::read_pair() {
    char buffer[8];
    file.read(buffer, 8);
    uint64_t first = *(uint64_t *)buffer;
    file.read(buffer, 8);
    uint64_t second = *(uint64_t *)buffer;
    return pair<uint64_t, uint64_t>(first, second);
}


uint64_t Catalog::create_node() {
    return ++node_count;
}


uint64_t Catalog::create_edge() {
    return ++edge_count;
}


void Catalog::add_node_label(uint64_t label_id) {
    add_to_map(node_label_stats, label_id, node_label_count);
}


void Catalog::add_edge_label(uint64_t label_id) {
    add_to_map(edge_label_stats, label_id, edge_label_count);
}


void Catalog::add_node_key(uint64_t key_id) {
    add_to_map(node_key_stats, key_id, node_key_count);
}


void Catalog::add_edge_key(uint64_t key_id) {
    add_to_map(edge_key_stats, key_id, edge_key_count);
}


void Catalog::add_to_map(map<uint64_t, uint64_t>& map, uint64_t key, uint64_t& count) {
    auto it = map.find(key);
    if ( it == map.end() ) {
        count++;
        map.insert(pair<uint64_t, uint64_t>(key, 1));
    } else {
        it->second += 1;
    }
}


uint64_t Catalog::get_count(map<uint64_t, uint64_t>& map, uint64_t key) {
    auto it = map.find(key);
    if ( it == map.end() ) {
        return 0;
    } else {
        return it->second;
    }
}


uint64_t Catalog::get_node_count_for_label(uint64_t label_id) {
    return get_count(node_label_stats, label_id);
}


uint64_t Catalog::get_edge_count_for_label(uint64_t label_id) {
    return get_count(edge_label_stats, label_id);
}


uint64_t Catalog::get_node_count_for_key(uint64_t key_id) {
    return get_count(node_key_stats, key_id);
}


uint64_t Catalog::get_edge_count_for_key(uint64_t key_id){
    return get_count(edge_key_stats, key_id);
}
