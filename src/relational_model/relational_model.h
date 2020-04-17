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

class RelationalGraph;

class RelationalModel {
public:
    static constexpr auto MAX_INLINED_BYTES = 5; // Ids have 8 bytes, 2 used for graph, 1 foy type and 5 remaining
    static constexpr auto GRAPH_OFFSET = 48;
    static constexpr auto TYPE_OFFSET = 40;

    static constexpr uint64_t NODE_MASK                 = 0x0000'01'0000000000UL;
    static constexpr uint64_t EDGE_MASK                 = 0x0000'02'0000000000UL;
    static constexpr uint64_t LABEL_MASK                = 0x0000'03'0000000000UL;
    static constexpr uint64_t KEY_MASK                  = 0x0000'04'0000000000UL;
    static constexpr uint64_t VALUE_EXTERNAL_STR_MASK   = 0x0000'05'0000000000UL;
    static constexpr uint64_t VALUE_INLINE_STR_MASK     = 0x0000'06'0000000000UL;
    static constexpr uint64_t VALUE_INT_MASK            = 0x0000'07'0000000000UL;
    static constexpr uint64_t VALUE_FLOAT_MASK          = 0x0000'08'0000000000UL;
    static constexpr uint64_t VALUE_BOOL_MASK           = 0x0000'09'0000000000UL;

    static constexpr uint64_t UNMASK                    = 0x0000'00'FFFFFFFFFFUL;
    static constexpr uint64_t TYPE_MASK                 = 0x0000'FF'0000000000UL;
    static constexpr uint64_t GRAPH_MASK                = 0xFFFF'00'0000000000UL;

    RelationalModel();
    ~RelationalModel();

    static void init();

    static ObjectId get_string_unmasked_id(const std::string& str);
    static ObjectId get_value_masked_id(const Value& value);

    static ObjectId get_or_create_string_unmasked_id(const std::string& str);
    static ObjectId get_or_create_value_masked_id(const Value& value);

    static RelationalGraph& create_graph(const std::string& graph_name);
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

private:
    static std::unique_ptr<RelationalModel> instance;

    std::unique_ptr<ObjectFile> object_file;
    std::unique_ptr<Catalog>    catalog;
    std::unique_ptr<BPlusTree>  hash2id; // ObjectHash|ObjectId.
    std::map<GraphId, std::unique_ptr<RelationalGraph>> graphs;

    static uint64_t get_or_create_external_id(std::unique_ptr< std::vector<unsigned char> > bytes);
    static uint64_t get_external_id(std::unique_ptr< std::vector<unsigned char> > bytes);
    static uint64_t get_value_mask(const Value& value);
};

#endif //RELATIONAL_MODEL__CONFIG_H_
