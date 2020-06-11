#include "relational_model.h"

#include <iostream>
#include <new>         // placement new
#include <type_traits> // aligned_storage

#include <openssl/md5.h>

#include "base/graph/edge.h"
#include "base/graph/node.h"
#include "base/graph/value/value_int.h"
#include "base/graph/value/value_bool.h"
#include "base/graph/value/value_float.h"
#include "base/graph/value/value_string.h"
#include "relational_model/graph/relational_graph.h"
#include "storage/catalog/catalog.h"
#include "storage/buffer_manager.h"
#include "storage/file_manager.h"

using namespace std;

// memory for the object
static typename std::aligned_storage<sizeof(RelationalModel), alignof(RelationalModel)>::type relational_model_buf;
// global object
RelationalModel& relational_model = reinterpret_cast<RelationalModel&>(relational_model_buf);

bool RelationalModel::initialized = false;

RelationalModel::RelationalModel() {
    object_file = make_unique<ObjectFile>(object_file_name);
    strings_cache = make_unique<StringsCache>(1000);

    // Create BPT Params
    auto bpt_params_hash2id = make_unique<BPlusTreeParams>(hash2id_name, 3); // Hash:2*64 + Key:64

    auto bpt_params_label2node = make_unique<BPlusTreeParams>(RelationalModel::label2node_name, 2);
    auto bpt_params_label2edge = make_unique<BPlusTreeParams>(RelationalModel::label2edge_name, 2);
    auto bpt_params_node2label = make_unique<BPlusTreeParams>(RelationalModel::node2label_name, 2);
    auto bpt_params_edge2label = make_unique<BPlusTreeParams>(RelationalModel::edge2label_name, 2);

    auto bpt_params_key_value_node = make_unique<BPlusTreeParams>(RelationalModel::key_value_node_name, 3);
    auto bpt_params_node_key_value = make_unique<BPlusTreeParams>(RelationalModel::node_key_value_name, 3);
    auto bpt_params_key_node_value = make_unique<BPlusTreeParams>(RelationalModel::key_node_value_name, 3);

    auto bpt_params_key_value_edge = make_unique<BPlusTreeParams>(RelationalModel::key_value_edge_name, 3);
    auto bpt_params_edge_key_value = make_unique<BPlusTreeParams>(RelationalModel::edge_key_value_name, 3);
    auto bpt_params_key_edge_value = make_unique<BPlusTreeParams>(RelationalModel::key_edge_value_name, 3);

    auto bpt_params_from_to_edge = make_unique<BPlusTreeParams>(RelationalModel::from_to_edge_name, 3);
    auto bpt_params_to_edge_from = make_unique<BPlusTreeParams>(RelationalModel::to_edge_from_name, 3);
    auto bpt_params_edge_from_to = make_unique<BPlusTreeParams>(RelationalModel::edge_from_to_name, 3);

    auto bpt_params_nodeloop_edge = make_unique<BPlusTreeParams>(RelationalModel::nodeloop_edge_name, 2);
    auto bpt_params_edge_nodeloop = make_unique<BPlusTreeParams>(RelationalModel::edge_nodeloop_name, 2);


    // Create BPT
    hash2id = make_unique<BPlusTree>(move(bpt_params_hash2id));

    label2node = make_unique<BPlusTree>(move(bpt_params_label2node));
    label2edge = make_unique<BPlusTree>(move(bpt_params_label2edge));
    node2label = make_unique<BPlusTree>(move(bpt_params_node2label));
    edge2label = make_unique<BPlusTree>(move(bpt_params_edge2label));

    key_value_node = make_unique<BPlusTree>(move(bpt_params_key_value_node));
    node_key_value = make_unique<BPlusTree>(move(bpt_params_node_key_value));
    key_node_value = make_unique<BPlusTree>(move(bpt_params_key_node_value));

    key_value_edge = make_unique<BPlusTree>(move(bpt_params_key_value_edge));
    edge_key_value = make_unique<BPlusTree>(move(bpt_params_edge_key_value));
    key_edge_value = make_unique<BPlusTree>(move(bpt_params_key_edge_value));

    from_to_edge = make_unique<BPlusTree>(move(bpt_params_from_to_edge));
    to_edge_from = make_unique<BPlusTree>(move(bpt_params_to_edge_from));
    edge_from_to = make_unique<BPlusTree>(move(bpt_params_edge_from_to));

    nodeloop_edge = make_unique<BPlusTree>(move(bpt_params_nodeloop_edge));
    edge_nodeloop = make_unique<BPlusTree>(move(bpt_params_edge_nodeloop));

    catalog.print();
}


RelationalModel::~RelationalModel() {
    // delete unique_ptrs
    object_file.reset();
    // hash2id.reset();
    strings_cache.reset();

    catalog.~Catalog();
    buffer_manager.~BufferManager();
    file_manager.~FileManager();
}

void RelationalModel::terminate() {
    if (initialized)
        (&relational_model)->~RelationalModel();
}


void RelationalModel::init(std::string db_folder, int buffer_pool_size) {
    // cout << "initializing RelationalModel:\n";
    // cout << "  folder: " << db_folder << "\n";
    // cout << "  buffer pool size: " << buffer_pool_size << "\n";
    initialized = true;
    // cout << "initializing FileManager...";
    FileManager::init(db_folder);
    // cout << "[done]\n";
    // cout << "initializing BufferManager...";
    BufferManager::init(buffer_pool_size);
    // cout << "[done]\n";
    // cout << "initializing Catalog...";
    Catalog::init();
    // cout << "[done]\n";
    new (&relational_model) RelationalModel(); // placement new
    // cout << "[RelationalModel done]\n";

}


uint64_t RelationalModel::get_external_id(std::unique_ptr< std::vector<unsigned char> > bytes,
                                          bool create_if_not_exists)
{
    static_assert(MD5_DIGEST_LENGTH == 16, "Hash is expected to use 16 bytes.");
    uint64_t hash[2];
    MD5((const unsigned char*)bytes->data(), bytes->size(), (unsigned char *)hash);

    // check if bpt contains object
    auto iter = hash2id->get_range(
        Record(hash[0], hash[1], 0),
        Record(hash[0], hash[1], UINT64_MAX)
    );
    auto next = iter->next();
    if (next == nullptr) {
        // object doesn't exist
        if (create_if_not_exists) {
            // Insert in object file
            uint64_t obj_id = get_object_file().write(*bytes);
            // Insert in bpt
            hash2id->insert( Record(hash[0], hash[1], obj_id) );
            return obj_id;
        } else {
            return ObjectId::OBJECT_ID_NOT_FOUND;
        }
    } else {
        // object already exists
        return next->ids[2];
    }
}


uint64_t RelationalModel::get_string_id(const string& str, bool create_if_not_exists) {
    int string_len = str.length();

    if (string_len > MAX_INLINED_BYTES) {
        auto bytes = make_unique<vector<unsigned char>>(string_len);
        copy(str.begin(), str.end(), bytes->begin());

        auto external_id =  get_external_id(move(bytes), create_if_not_exists);
        if (external_id == ObjectId::OBJECT_ID_NOT_FOUND) {
            return external_id;
        } else {
            return external_id | VALUE_EXTERNAL_STR_MASK;
        }
    } else {
        uint64_t res = 0;
        int shift_size = 0;
        for (uint64_t byte : str) { // MUST convert to 64bits or shift (shift_size >=32) is undefined behaviour
            res |= byte << shift_size;
            shift_size += 8;
        }
        return res | VALUE_INLINE_STR_MASK;;
    }
}


ObjectId RelationalModel::get_value_id(const Value& value, bool create_if_not_exists) {
    switch (value.type()) {
        case ObjectType::value_string : {
            const auto& string_value = static_cast<const ValueString&>(value);
            return ObjectId( get_string_id(string_value.value, create_if_not_exists) );
        }

        case ObjectType::value_int : {
            const auto& int_value = static_cast<const ValueInt&>(value);
            auto value = int_value.value;

            uint64_t mask = VALUE_POSITIVE_INT_MASK;
            if (value < 0) {
                value *= -1;
                mask = VALUE_NEGATIVE_INT_MASK;
            }

            // check if it needs more than 7 bytes
            if ( (value & 0xFF00'0000'0000'0000UL) == 0) {
                return ObjectId(mask | value);
            } else {
                // VALUE_EXTERNAL_INT_MASK
                throw std::logic_error("NOT SUPPORTED YET");
            }
        }

        case ObjectType::value_float : {
            const auto& value_float = static_cast<const ValueFloat&>(value);
            auto bytes = make_unique<vector<unsigned char>>(sizeof(value_float.value));
            memcpy(bytes->data(), &value_float.value, sizeof(value_float.value));

            uint64_t res = 0;
            int shift_size = 0;
            for (uint64_t byte : *bytes) {
                res |= byte << shift_size;
                shift_size += 8;
            }
            return ObjectId(VALUE_FLOAT_MASK | res);
        }

        case ObjectType::value_bool : {
            const auto& value_bool = static_cast<const ValueBool&>(value);
            if (value_bool.value) {
                return ObjectId(VALUE_BOOL_MASK | 0x01);
            } else {
                return ObjectId(VALUE_BOOL_MASK | 0x00);
            }
        }

        default : {
            throw logic_error("Unexpected value type.");
        }
    }
}


shared_ptr<GraphObject> RelationalModel::get_graph_object(ObjectId object_id) {
    auto mask = object_id.id & TYPE_MASK;
    auto unmasked_id = object_id & VALUE_MASK;
    switch (mask) {
        case VALUE_EXTERNAL_STR_MASK : {
            auto cached_string = strings_cache->get(unmasked_id);
            if (cached_string != nullptr) {
                return cached_string;
            } else {
                auto bytes = object_file->read(unmasked_id);
                string value_string(bytes->begin(), bytes->end());
                strings_cache->insert(unmasked_id, value_string);
                return make_shared<ValueString>(move(value_string));
            }
        }

        case VALUE_INLINE_STR_MASK : {
            string value_string = "";
            int shift_size = 0;
            for (int i = 0; i < MAX_INLINED_BYTES; i++) {
                uint8_t byte = (object_id >> shift_size) & 0xFF;
                if (byte == 0x00) {
                    break;
                }
                value_string.push_back(byte);
                shift_size += 8;
            }
            return make_shared<ValueString>(move(value_string));
        }

        case VALUE_POSITIVE_INT_MASK : {
            static_assert(sizeof(int64_t) == 8, "int64_t must be 8 bytes");
            int64_t i = object_id & 0x00FF'FFFF'FFFF'FFFFUL;
            return make_shared<ValueInt>(i);
        }

        case VALUE_NEGATIVE_INT_MASK : {
            static_assert(sizeof(int64_t) == 8, "int64_t must be 8 bytes");
            int64_t i = object_id & 0x00FF'FFFF'FFFF'FFFFUL;
            return make_shared<ValueInt>(i*-1);
        }

        case VALUE_FLOAT_MASK : {
            static_assert(sizeof(float) == 4, "float must be 4 bytes");
            float f;
            uint8_t* dest = (uint8_t*)&f;
            dest[0] = object_id & 0xFF;
            dest[1] = (object_id >> 8) & 0xFF;
            dest[2] = (object_id >> 16) & 0xFF;
            dest[3] = (object_id >> 24) & 0xFF;
            return make_shared<ValueFloat>(f);
        }

        case VALUE_BOOL_MASK : {
            bool b;
            uint8_t* dest = (uint8_t*)&b;
            *dest = object_id & 0xFF;
            return make_shared<ValueBool>(b);
        }

        case NODE_MASK : {
            return make_shared<Node>((object_id >> 40) & 0xFFFF, object_id & 0x0000'00FF'FFFF'FFFFUL);
        }

        case EDGE_MASK : {
            return make_shared<Edge>((object_id >> 40) & 0xFFFF, object_id & 0x0000'00FF'FFFF'FFFFUL);
        }

        default : {
            cout << "wrong value prefix:\n";
            printf("  obj_id: %lX\n", object_id.id);
            printf("  mask: %lX\n", mask);
            string value_string = "";
            return make_shared<ValueString>(move(value_string));
        }
    }
}


RelationalGraph& RelationalModel::create_graph(const std::string& graph_name) {
    auto graph_id = catalog.create_graph(graph_name);
    return get_graph(graph_id);
}


RelationalGraph& RelationalModel::get_graph(GraphId graph_id) {
    auto search = graphs.find(graph_id);
    if (search != graphs.end()) {
        return *search->second.get();
    } else {
        graphs.insert({ graph_id, make_unique<RelationalGraph>(graph_id) });
        return *graphs[graph_id].get();
    }
}


ObjectFile& RelationalModel::get_object_file() { return *object_file; }

BPlusTree& RelationalModel::get_label2node() { return *label2node; }
BPlusTree& RelationalModel::get_label2edge() { return *label2edge; }
BPlusTree& RelationalModel::get_node2label() { return *node2label; }
BPlusTree& RelationalModel::get_edge2label() { return *edge2label; }

BPlusTree& RelationalModel::get_key_value_node() { return *key_value_node; }
BPlusTree& RelationalModel::get_node_key_value() { return *node_key_value; }
BPlusTree& RelationalModel::get_key_node_value() { return *key_node_value; }

BPlusTree& RelationalModel::get_key_value_edge() { return *key_value_edge; }
BPlusTree& RelationalModel::get_edge_key_value() { return *edge_key_value; }
BPlusTree& RelationalModel::get_key_edge_value() { return *key_edge_value; }

BPlusTree& RelationalModel::get_from_to_edge() { return *from_to_edge; }
BPlusTree& RelationalModel::get_to_edge_from() { return *to_edge_from; }
BPlusTree& RelationalModel::get_edge_from_to() { return *edge_from_to; }

BPlusTree& RelationalModel::get_nodeloop_edge() { return *nodeloop_edge; }
BPlusTree& RelationalModel::get_edge_nodeloop() { return *edge_nodeloop; }
