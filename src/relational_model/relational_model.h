#ifndef RELATIONAL_MODEL__CONFIG_H_
#define RELATIONAL_MODEL__CONFIG_H_

#include "base/ids/graph_id.h"
#include "storage/buffer_manager.h"
#include "storage/catalog/catalog.h"
#include "storage/index/bplus_tree/bplus_tree.h"
#include "storage/index/bplus_tree/bplus_tree_dir.h"
#include "storage/index/bplus_tree/bplus_tree_params.h"
#include "storage/index/object_file/object_file.h"

#include <memory>
#include <map>

const uint64_t NODE_MASK                 = 0x0100000000000000UL;
const uint64_t EDGE_MASK                 = 0x0200000000000000UL;
const uint64_t LABEL_MASK                = 0x0300000000000000UL;
const uint64_t KEY_MASK                  = 0x0400000000000000UL;
const uint64_t VALUE_EXTERNAL_STR_MASK   = 0x0500000000000000UL;
const uint64_t VALUE_INLINE_STR_MASK     = 0x0600000000000000UL;
const uint64_t VALUE_INT_MASK            = 0x0700000000000000UL;
const uint64_t VALUE_FLOAT_MASK          = 0x0800000000000000UL;
const uint64_t VALUE_BOOL_MASK           = 0x0900000000000000UL;
const uint64_t UNMASK                    = 0x00FFFFFFFFFFFFFFUL;
const uint64_t MASK                      = 0xFF00000000000000UL;

class RelationalGraph;

class RelationalModel {

private:
    static std::unique_ptr<RelationalModel> instance;

    std::unique_ptr<ObjectFile> object_file;
    std::unique_ptr<Catalog>    catalog;
    std::unique_ptr<BPlusTree>  hash2id; // ObjectHash|ObjectId.
    std::map<GraphId, std::unique_ptr<RelationalGraph>> graphs;

    static uint64_t get_or_create_external_id(std::unique_ptr< std::vector<unsigned char> > bytes);
    static uint64_t get_external_id(std::unique_ptr< std::vector<unsigned char> > bytes);
    static uint64_t get_value_mask(const Value& value);

public:
    RelationalModel();
    ~RelationalModel();

    static void init();

    static ObjectId get_string_unmasked_id(const std::string& str);
    static ObjectId get_value_masked_id(const Value& value);

    static ObjectId get_or_create_string_unmasked_id(const std::string& str);
    static ObjectId get_or_create_value_masked_id(const Value& value);

    static RelationalGraph& get_graph(GraphId);
    static std::shared_ptr<GraphObject> get_graph_object(ObjectId);

    static ObjectFile& get_object_file();
    static Catalog&    get_catalog();
    static BPlusTree&  get_hash2id_bpt();

    static inline const std::string object_file_name  = "objects.dat";
    static inline const std::string catalog_file_name = "catalog.dat";
    static inline const std::string hash2id_name      = "hash_id";

    // Labels
    static inline const std::string label2node_name = "LN";
    static inline const std::string node2label_name = "NL";
    static inline const std::string label2edge_name = "LE";
    static inline const std::string edge2label_name = "EL";

    // Properties
    static inline const std::string node2prop_name = "NKV";
    static inline const std::string prop2node_name = "KVN";
    static inline const std::string edge2prop_name = "EKV";
    static inline const std::string prop2edge_name = "KVE";

    // Connections
    static inline const std::string from_to_edge_name = "FTE";
    static inline const std::string to_edge_from_name = "TEF";
    static inline const std::string edge_from_to_name = "EFT";
};

#endif //RELATIONAL_MODEL__CONFIG_H_
