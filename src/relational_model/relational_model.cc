#include "relational_model.h"

#include <iostream>
#include <openssl/md5.h>

#include "base/graph/edge.h"
#include "base/graph/node.h"
#include "base/graph/value/value_int.h"
#include "base/graph/value/value_bool.h"
#include "base/graph/value/value_float.h"
#include "base/graph/value/value_string.h"
#include "relational_model/graph/relational_graph.h"

using namespace std;

unique_ptr<RelationalModel> RelationalModel::instance = nullptr; // can't use static object because dependency with BufferManager

RelationalModel::RelationalModel() {
    object_file = make_unique<ObjectFile>(object_file_name);
    catalog = make_unique<Catalog>(catalog_file_name);
    auto bpt_params_hash2id = make_unique<BPlusTreeParams>(hash2id_name, 3); // Hash:2*64 + Key:64
    hash2id = make_unique<BPlusTree>(move(bpt_params_hash2id));
}

RelationalModel::~RelationalModel() = default;


void RelationalModel::init() {
    instance = make_unique<RelationalModel>();
}


uint64_t RelationalModel::get_external_id(std::unique_ptr< std::vector<unsigned char> > bytes) {
    static_assert(MD5_DIGEST_LENGTH == 16, "Hash is expected to use 16 bytes.");
    uint64_t hash[2];
    MD5((const unsigned char*)bytes->data(), bytes->size(), (unsigned char *)hash);

    // check if bpt contains object
    BPlusTree& bpt = get_hash2id_bpt();
    auto iter = bpt.get_range(
        Record(hash[0], hash[1], 0),
        Record(hash[0], hash[1], UINT64_MAX)
    );
    auto next = iter->next();
    if (next == nullptr) { // object doesn't exists
        return NOT_FOUND_OBJECT_ID;
    }
    else {                 // object already exists
        return next->ids[2];
    }
}


uint64_t RelationalModel::get_or_create_external_id(std::unique_ptr< std::vector<unsigned char> > bytes) {
    static_assert(MD5_DIGEST_LENGTH == 16, "Hash is expected to use 16 bytes.");
    uint64_t hash[2];
    MD5((const unsigned char*)bytes->data(), bytes->size(), (unsigned char *)hash);

    // check if bpt contains object
    BPlusTree& hash2id = RelationalModel::get_hash2id_bpt();
    auto iter = hash2id.get_range(
        Record(hash[0], hash[1], 0),
        Record(hash[0], hash[1], UINT64_MAX)
    );
    auto next = iter->next();
    if (next == nullptr) { // obj doesn't exist
        // Insert in object file
        uint64_t obj_id = RelationalModel::get_object_file().write(*bytes);
        // Insert in bpt
        hash2id.insert( Record(hash[0], hash[1], obj_id) );
        return obj_id;
    }
    else { // object already exists
        return next->ids[2];
    }
}


ObjectId RelationalModel::get_string_unmasked_id(const string& str) {
    int string_len = str.length();

    if (string_len > MAX_INLINED_BYTES) {
        auto bytes = make_unique<vector<unsigned char>>(string_len);
        copy(str.begin(), str.end(), bytes->begin());

        return ObjectId( get_external_id(move(bytes)) );
    }
    else {
        uint64_t res = 0;
        int shift_size = 0;
        for (uint64_t byte : str) { // MUST convert to 64bits or shift (shift_size >=32) is undefined behaviour
            res |= byte << shift_size;
            shift_size += 8;
        }
        return ObjectId(res);
    }
}


ObjectId RelationalModel::get_value_masked_id(const Value& value) {
    auto obj_bytes = value.get_bytes();
    if (obj_bytes->size() > MAX_INLINED_BYTES) {
        return get_external_id(move(obj_bytes)) | get_value_mask(value);
    }
    else {
        uint64_t res = 0;
        int shift_size = 0;
        for (uint64_t byte : *obj_bytes) { // MUST convert to 64bits or shift (shift_size >=32) is undefined behaviour
            res |= byte << shift_size;
            shift_size += 8;
        }
        return ObjectId( res | get_value_mask(value) );
    }
}


ObjectId RelationalModel::get_or_create_string_unmasked_id(const std::string& str) {
    int string_len = str.length();

    if (string_len > MAX_INLINED_BYTES) {
        auto bytes = make_unique<vector<unsigned char>>(string_len);
        copy(str.begin(), str.end(), bytes->begin());

        return ObjectId( get_or_create_external_id(move(bytes)) );
    }
    else {
        uint64_t res = 0;
        int shift_size = 0;
        for (uint64_t byte : str) { // MUST convert to 64bits or shift (shift_size >=32) is undefined behaviour
            res |= byte << shift_size;
            shift_size += 8;
        }
        return ObjectId(res);
    }
}


ObjectId RelationalModel::get_or_create_value_masked_id(const Value& value) {
    auto obj_bytes = value.get_bytes();
    if (obj_bytes->size() > MAX_INLINED_BYTES) {
        return get_or_create_external_id(move(obj_bytes)) | get_value_mask(value);
    }
    else {
        uint64_t res = 0;
        int shift_size = 0;
        for (uint64_t byte : *obj_bytes) { // MUST convert to 64bits or shift (shift_size >=32) is undefined behaviour
            res |= byte << shift_size;
            shift_size += 8;
        }
        return ObjectId( res | get_value_mask(value) );
    }
}


shared_ptr<GraphObject> RelationalModel::get_graph_object(ObjectId object_id) {
    auto mask = object_id.id & TYPE_MASK;

    if (mask == VALUE_EXTERNAL_STR_MASK) {
        auto bytes = instance->object_file->read(object_id & UNMASK);
        string value_string(bytes->begin(), bytes->end());
        return make_shared<ValueString>(move(value_string));
    }
    else if (mask == VALUE_INLINE_STR_MASK) {
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
    else if (mask == VALUE_INT_MASK) {
        static_assert(sizeof(int) == 4, "int must be 4 bytes");
        int i;
        uint8_t* dest = (uint8_t*)&i;
        dest[0] = object_id & 0xFF;
        dest[1] = (object_id >> 8) & 0xFF;
        dest[2] = (object_id >> 16) & 0xFF;
        dest[3] = (object_id >> 24) & 0xFF;
        return make_shared<ValueInt>(i);
    }
    else if (mask == VALUE_FLOAT_MASK) {
        static_assert(sizeof(float) == 4, "float must be 4 bytes");
        float f;
        uint8_t* dest = (uint8_t*)&f;
        dest[0] = object_id & 0xFF;
        dest[1] = (object_id >> 8) & 0xFF;
        dest[2] = (object_id >> 16) & 0xFF;
        dest[3] = (object_id >> 24) & 0xFF;
        return make_shared<ValueFloat>(f);
    }
    else if (mask == VALUE_BOOL_MASK) {
        bool b;
        uint8_t* dest = (uint8_t*)&b;
        *dest = object_id & 0xFF;
        return make_shared<ValueBool>(b);
    }
    else if (mask == NODE_MASK) {
        return make_shared<Node>(object_id);
    }
    else if (mask == EDGE_MASK) {
        return make_shared<Edge>(object_id);
    }
    else {
        cout << "wrong value prefix:\n";
        printf("  obj_id: %lX\n", object_id.id);
        printf("  mask: %lX\n", mask);
        string value_string = "";
        return make_shared<ValueString>(move(value_string));
    }
}


RelationalGraph& RelationalModel::create_graph(const std::string& graph_name) {
    auto graph_id = get_catalog().create_graph(graph_name);
    return get_graph(graph_id);
}


RelationalGraph& RelationalModel::get_graph(GraphId graph_id) {
    auto search = instance->graphs.find(graph_id);
    if (search != instance->graphs.end()) {
        return *search->second.get();
    }
    else {
        instance->graphs.insert({ graph_id, make_unique<RelationalGraph>(graph_id) });
        return *instance->graphs[graph_id].get();
    }
}


uint64_t RelationalModel::get_value_mask(const Value& value) {
    auto type = value.type();
    if (type == ObjectType::value_string) {
        const auto& string_value = static_cast<const ValueString&>(value);
        if (string_value.value.size() <= MAX_INLINED_BYTES) {
            return VALUE_INLINE_STR_MASK;
        }
        else return VALUE_EXTERNAL_STR_MASK;
    }
    else if (type == ObjectType::value_int) {
        return VALUE_INT_MASK;
    }
    else if (type == ObjectType::value_float) {
        return VALUE_FLOAT_MASK;
    }
    else if (type == ObjectType::value_bool) {
        return VALUE_BOOL_MASK;
    }
    else {
        throw logic_error("Unexpected value type.");
    }
}


ObjectFile& RelationalModel::get_object_file() { return *instance->object_file; }
Catalog&    RelationalModel::get_catalog()     { return *instance->catalog; }
BPlusTree&  RelationalModel::get_hash2id_bpt() { return *instance->hash2id; }
