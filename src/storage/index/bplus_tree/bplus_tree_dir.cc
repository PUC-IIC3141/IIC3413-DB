#include "bplus_tree_dir.h"

#include "storage/buffer_manager.h"
#include "storage/index/record.h"
#include "storage/index/bplus_tree/bplus_tree.h"
#include "storage/index/bplus_tree/bplus_tree_leaf.h"
#include "storage/index/bplus_tree/bplus_tree_params.h"

#include <iostream>
#include <utility>
#include <cstring>

using namespace std;

BPlusTreeDir::BPlusTreeDir(const BPlusTreeParams& params, Page& page)
    : params(params), page(page)
{
    key_count = reinterpret_cast<int*>(page.get_bytes());
    keys      = reinterpret_cast<uint64_t*>(page.get_bytes()
                                            + sizeof(int));
    children  = reinterpret_cast<int*>(page.get_bytes()
                                       + sizeof(int)
                                       + (sizeof(uint64_t) * params.dir_max_records * params.key_size));
}


BPlusTreeDir::~BPlusTreeDir() {
    buffer_manager.unpin(page);
}


std::unique_ptr<Record> BPlusTreeDir::get(const Record& key){
    int index = search_child_index(0, *key_count, key);
    int page_pointer = children[index];

    if (page_pointer < 0) { // negative number: pointer to dir
        Page& child_page = buffer_manager.get_page(params.dir_file_id, page_pointer*-1);
        auto child =  BPlusTreeDir(params, child_page);
        return child.get(key);
    }
    else { // positive number: pointer to leaf
        Page& child_page = buffer_manager.get_page(params.leaf_file_id, page_pointer);
        auto child =  BPlusTreeLeaf(params, child_page);
        return child.get(key);
    }
}

// requieres first insert manually
std::unique_ptr<BPlusTreeSplit> BPlusTreeDir::bulk_insert(BPlusTreeLeaf& leaf) {
    int page_pointer = children[*key_count];
    std::unique_ptr<BPlusTreeSplit> split;

    if (page_pointer < 0) { // negative number: pointer to dir
        auto& child_page = buffer_manager.get_page(params.dir_file_id, page_pointer*-1);
        auto child =  BPlusTreeDir(params, child_page);
        split = child.bulk_insert(leaf);
    }
    else { // positive number: pointer to leaf
        split = make_unique<BPlusTreeSplit>(*leaf.get_record(0), leaf.page.get_page_number());
    }

    if (split != nullptr) {
        // Case 1: no need to split this node
        if (*key_count < params.dir_max_records) {
            update_key(*key_count, split->record);
            (*key_count)++;
            update_child(*key_count, split->encoded_page_number);
            page.make_dirty();
            return nullptr;
        }
        // Case 2: we need to split this node and this node is the root
        else if (page.get_page_number() == 0) {
            auto& new_left_page = buffer_manager.append_page(params.dir_file_id);
            auto& new_right_page = buffer_manager.append_page(params.dir_file_id);

            auto new_left_dir = BPlusTreeDir(params, new_left_page);
            auto new_right_dir = BPlusTreeDir(params, new_right_page);

            // write left keys from 0 to (*key_count-1)
            std::memcpy(
                new_left_dir.keys,
                keys,
                (*key_count) * params.key_size * sizeof(uint64_t)
            );
            // write left children-pointers from 0 to (*key_count)
            std::memcpy(
                new_left_dir.children,
                children,
                (*key_count + 1) * sizeof(int)
            );

            // write right dirs
            new_right_dir.children[0] = split->encoded_page_number;

            // update counts
            *new_left_dir.key_count = *key_count;
            *this->key_count = 1;
            *new_right_dir.key_count = 0;

            std::memcpy(
                keys,
                split->record.ids.data(),
                params.key_size * sizeof(uint64_t)
            );
            children[0] = new_left_dir.page.get_page_number() * -1;
            children[1] = new_right_dir.page.get_page_number() * -1;
            new_left_page.make_dirty();
            new_right_page.make_dirty();
            this->page.make_dirty();
            return nullptr;
        }
        // Case 3: no-root split
        else {
            auto& new_page = buffer_manager.append_page(params.dir_file_id);
            auto new_dir = BPlusTreeDir(params, new_page);
            new_dir.children[0] = split->encoded_page_number;
            *new_dir.key_count = 0;
            // *this->key_count does not change
            new_page.make_dirty();
            this->page.make_dirty();
            return std::make_unique<BPlusTreeSplit>(split->record, new_page.get_page_number()*-1);
        }
    }
    return nullptr;
}


std::unique_ptr<BPlusTreeSplit> BPlusTreeDir::insert(const Record& key, const Record& value) {
    int index = (*key_count > 0)
        ? search_child_index(0, *key_count, key)
        : 0;

    int page_pointer = children[index];
    std::unique_ptr<BPlusTreeSplit> split = nullptr;

    if (page_pointer < 0) { // negative number: pointer to dir
        auto& child_page = buffer_manager.get_page(params.dir_file_id, page_pointer*-1);
        auto child =  BPlusTreeDir(params, child_page);
        split = child.insert(key, value);
    }
    else { // positive number: pointer to leaf
        auto& child_page = buffer_manager.get_page(params.leaf_file_id, page_pointer);
        auto child =  BPlusTreeLeaf(params, child_page);
        split = child.insert(key, value);
    }

    if (split != nullptr) {
        int splitted_index = search_child_index(0, *key_count, split->record);
        // Case 1: no need to split this node
        if (*key_count < params.dir_max_records) {
            shift_right_keys(splitted_index, *key_count-1);
            shift_right_children(splitted_index+1, *key_count);
            update_key(splitted_index, split->record);
            update_child(splitted_index+1, split->encoded_page_number);
            (*key_count)++;
            this->page.make_dirty();
            return nullptr;
        }
        // Case 2: we need to split this node and this node is the root
        else if (page.get_page_number() == 0) {
            // poner nuevo record/dir y guardar el ultimo (que no cabe)
            auto last_key = vector<uint64_t>(params.key_size);
            int last_dir;
            if (splitted_index == *key_count) { // splitted key is the last key
                std::memcpy(
                    last_key.data(),
                    split->record.ids.data(),
                    params.key_size * sizeof(uint64_t)
                );
                last_dir = split->encoded_page_number;
            }
            else {
                std::memcpy(
                    last_key.data(),
                    &keys[(*key_count-1) * params.key_size],
                    params.key_size * sizeof(uint64_t)
                );
                last_dir = children[*key_count];
                shift_right_keys(splitted_index, *key_count-2);
                shift_right_children(splitted_index+1, *key_count-1);
                update_key(splitted_index, split->record);
                update_child(splitted_index+1, split->encoded_page_number);
            }
            int middle_index = (*key_count+1)/2;
            auto& new_left_page = buffer_manager.append_page(params.dir_file_id);
            auto& new_right_page = buffer_manager.append_page(params.dir_file_id);

            auto new_left_dir = BPlusTreeDir(params, new_left_page);
            auto new_right_dir = BPlusTreeDir(params, new_right_page);

            // write left keys from 0 to (middle_index-1)
            std::memcpy(
                new_left_dir.keys,
                keys,
                middle_index * params.key_size * sizeof(uint64_t)
            );
            // write right keys from (middle_index+1) to (*count-1) plus the last key saved before
            std::memcpy(
                new_right_dir.keys,
                &keys[(middle_index + 1) * params.key_size],
                (params.dir_max_records-(middle_index + 1)) * params.key_size * sizeof(uint64_t)
            );

            std::memcpy(
                &new_right_dir.keys[(params.dir_max_records - (middle_index + 1)) * params.key_size],
                last_key.data(),
                params.key_size * sizeof(uint64_t)
            );

            // write left children from 0 to middle_index
            std::memcpy(
                new_left_dir.children,
                children,
                (middle_index+1) * sizeof(int)
            );

            // write right dirs from middle_index + 1 to *count plus the last dir saved before
            std::memcpy(
                new_right_dir.children,
                &children[middle_index + 1],
                (*key_count - middle_index) * sizeof(int)
            );
            new_right_dir.children[*key_count - middle_index] = last_dir;
            // update counts
            *key_count = 1;
            *new_left_dir.key_count = middle_index;
            *new_right_dir.key_count = params.dir_max_records - middle_index;

            // record at middle_index becomes the first and only record of the root
            std::memcpy(
                keys,
                &keys[middle_index*params.key_size],
                params.key_size * sizeof(uint64_t)
            );
            children[0] = new_left_dir.page.get_page_number() * -1;
            children[1] = new_right_dir.page.get_page_number() * -1;
            new_left_page.make_dirty();
            new_right_page.make_dirty();
            this->page.make_dirty();
            return nullptr;
        }
        // Case 3: normal split (this node is not the root)
        else {
            // poner nuevo record/dir y guardar el ultimo (que no cabe)
            auto last_key = vector<uint64_t>(params.key_size);
            int last_dir;
            if (splitted_index == *key_count) { // splitted key is the last key
                std::memcpy(
                    last_key.data(),
                    split->record.ids.data(),
                    params.key_size * sizeof(uint64_t)
                );
                last_dir = split->encoded_page_number;
            }
            else {
                std::memcpy(
                    last_key.data(),
                    &keys[(*key_count-1) * params.key_size],
                    params.key_size * sizeof(uint64_t)
                );
                last_dir = children[*key_count];
                shift_right_keys(splitted_index, *key_count-2);
                shift_right_children(splitted_index+1, *key_count-1);
                update_key(splitted_index, split->record);
                update_child(splitted_index+1, split->encoded_page_number);
            }
            int middle_index = (*key_count+1)/2;

            auto& new_page = buffer_manager.append_page(params.dir_file_id);
            auto new_dir = BPlusTreeDir(params, new_page);

            // write records from (middle_index+1) to (*key_count-1) and the last record saved before
            std::memcpy(
                new_dir.keys,
                &keys[(middle_index+1)*params.key_size],
                (params.dir_max_records - (middle_index+1))*params.key_size * sizeof(uint64_t)
            );
            std::memcpy(
                &new_dir.keys[(params.dir_max_records - (middle_index+1))*params.key_size],
                last_key.data(),
                params.key_size * sizeof(uint64_t)
            );
            // write children from middle_index + 1 to *key_count and the last dir saved before
            std::memcpy(
                new_dir.children,
                &children[middle_index + 1],
                (*key_count - middle_index) * sizeof(int)
            );
            new_dir.children[*key_count - middle_index] = last_dir;
            // update counts
            *key_count = middle_index;
            *new_dir.key_count = params.dir_max_records - middle_index;

            // key at middle_index is returned
            std::vector<uint64_t> split_key(params.key_size);
            std::memcpy(
                split_key.data(),
                &keys[middle_index*params.key_size],
                params.key_size * sizeof(uint64_t)
            );
            new_page.make_dirty();
            this->page.make_dirty();
            return std::make_unique<BPlusTreeSplit>(Record(split_key), new_page.get_page_number()*-1);
        }
    }
    return nullptr;
}


void BPlusTreeDir::update_key(int index, const Record& record) {
    std::memcpy(
        &keys[index*params.key_size],
        record.ids.data(),
        params.key_size * sizeof(uint64_t)
    );
}


void BPlusTreeDir::update_child(int index, int dir) {
    children[index] = dir;
}


void BPlusTreeDir::shift_right_keys(int from, int to) {
    for (int i = to; i >= from; i--) {
        for (int j = 0; j < params.key_size; j++) {
            keys[(i+1)*params.key_size + j] = keys[i*params.key_size + j];
        }
    }
}


void BPlusTreeDir::shift_right_children(int from, int to) {
    for (int i = to; i >= from; i--) {
        children[i+1] = children[i];
    }
}


SearchLeafResult BPlusTreeDir::search_leaf(const Record& min) {
    int dir_index = search_child_index(0, *key_count, min);
    int page_pointer = children[dir_index];

    if (page_pointer < 0) { // negative number: pointer to dir
        Page& child_page = buffer_manager.get_page(params.dir_file_id, page_pointer*-1);
        BPlusTreeDir child = BPlusTreeDir(params, child_page);
        return child.search_leaf(min);
    }
    else { // positive number: pointer to leaf
        Page& child_page = buffer_manager.get_page(params.leaf_file_id, page_pointer);
        BPlusTreeLeaf child =  BPlusTreeLeaf(params, child_page);
        return child.search_leaf(min);
    }
}


int BPlusTreeDir::search_child_index(int dir_from, int dir_to, const Record& record) {
    if (dir_from == dir_to) {
        return dir_from;
    }
    int middle_dir = (dir_from + dir_to + 1) / 2;
    int middle_record = middle_dir-1;

    for (int i = 0; i < params.key_size; i++) {
        auto id = keys[middle_record*params.key_size + i];
        if (record.ids[i] < id) {
            return search_child_index(dir_from, middle_record, record);
        }
        else if (record.ids[i] > id) {
            return search_child_index(middle_record+1, dir_to, record);
        }
        // continue if they were equal
    }
    return search_child_index(middle_record+1, dir_to, record);
}


bool BPlusTreeDir::check() const {
    if (*key_count < 0) {
        std::cout << "  key_count shouldn't be less than 0\n";
    }
    if (*key_count == 0 && page.get_page_number() != 0) {
        std::cout << "  key_count shouldn't be 0, except for one node (at most)"
                  << " at the right-most branch if bulk import was used\n";
    }

    if (*key_count > 1) {
        // check keys are ordered
        auto x = Record(vector<uint64_t>(params.key_size));
        auto y = Record(vector<uint64_t>(params.key_size));

        int current_pos = 0;
        while (current_pos < params.key_size) {
            x.ids[current_pos] = keys[current_pos];
            current_pos++;
        }

        for (int k = 1; k < *key_count; k++) {
            for (int i = 0; i < params.key_size; i++) {
                y.ids[i] = keys[current_pos++];
            }
            if (y <= x) {
                std::cout << "bad key order at BPlusTreeDir:\n";
                std::cout << "  " << x << "\n";
                std::cout << "  " << y << "\n";
                return false;
            }
            x = y;
        }
    }

    // check children and keys are consistent
    int current_pos = 0;
    for (int i = 0; i < *key_count; i++) {
        auto key = Record(vector<uint64_t>(params.total_size));
        auto greatest_left_key = Record(vector<uint64_t>(params.total_size));
        auto smallest_right_key = Record(vector<uint64_t>(params.total_size));

        for (int j = 0; j < params.key_size; j++) {
            key.ids[j] = keys[current_pos++];
        }

        // Set greatest_left_key
        int left_pointer = children[i];
        if (left_pointer < 0) { // negative number: pointer to dir
            Page& left_page = buffer_manager.get_page(params.dir_file_id, left_pointer*-1);
            auto left_child =  BPlusTreeDir(params, left_page);
            for (int j = 0; j < params.key_size; j++) {
                greatest_left_key.ids[j] = left_child.keys[((*left_child.key_count-1) * params.key_size) + j];
            }
        }
        else { // positive number: pointer to leaf
            Page& left_page = buffer_manager.get_page(params.leaf_file_id, left_pointer);
            auto left_child =  BPlusTreeLeaf(params, left_page);
            for (int j = 0; j < params.key_size; j++) {
                greatest_left_key.ids[j] = left_child.records[((*left_child.value_count-1) * params.total_size) + j];
            }
        }

        // Set smallest_right_key
        int right_pointer = children[i+1];
        bool right_empty = false; // for skipping empty dirs
        if (right_pointer < 0) { // negative number: pointer to dir
            Page& right_page = buffer_manager.get_page(params.dir_file_id, right_pointer*-1);
            auto right_child =  BPlusTreeDir(params, right_page);
            for (int j = 0; j < params.key_size; j++) {
                smallest_right_key.ids[j] = right_child.keys[j];
            }
            if (*right_child.key_count == 0) {
                right_empty = true;
            }
        }
        else { // positive number: pointer to leaf
            Page& right_page = buffer_manager.get_page(params.leaf_file_id, right_pointer);
            auto right_child =  BPlusTreeLeaf(params, right_page);
            for (int j = 0; j < params.key_size; j++) {
                smallest_right_key.ids[j] = right_child.records[j];
            }
            if (*right_child.value_count == 0) {
                right_empty = true;
            }
        }

        if (!(greatest_left_key < key)) {
            std::cout << "inconsistency between key and left-child key at BPlusTreeDir.\n";
            std::cout << greatest_left_key << "\n";
            std::cout << key << "\n";
            return false;
        }
        if (!right_empty && !(key <= smallest_right_key)) {
            std::cout << "inconsistency between key and right-child key at BPlusTreeDir\n";
            std::cout << key << "\n";
            std::cout << smallest_right_key << "\n";
            return false;
        }
    }

    // propagate checking to children
    for (int i = 0; i <= *key_count; i++) {
        int page_pointer = children[i];

        if (page_pointer < 0) { // negative number: pointer to dir
            auto& child_page = buffer_manager.get_page(params.dir_file_id, page_pointer*-1);
            auto child =  BPlusTreeDir(params, child_page);
            if (!child.check())
                return false;
        }
        else { // positive number: pointer to leaf
            auto& child_page = buffer_manager.get_page(params.leaf_file_id, page_pointer);
            auto child =  BPlusTreeLeaf(params, child_page);
            if (!child.check())
                return false;
        }
    }
    return true;
}
