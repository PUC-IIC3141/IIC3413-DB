#include "storage/index/bplus_tree/bplus_tree.h"

#include "storage/buffer_manager.h"
#include "storage/index/record.h"
#include "storage/index/ordered_file/ordered_file.h"
#include "storage/index/bplus_tree/bplus_tree_dir.h"
#include "storage/index/bplus_tree/bplus_tree_leaf.h"
#include "storage/index/bplus_tree/bplus_tree_params.h"

#include <iostream>
#include <memory>

using namespace std;

BPlusTree::BPlusTree(unique_ptr<BPlusTreeParams> _params)
    : params(move(_params))
{
    root = make_unique<BPlusTreeDir>(*params, buffer_manager.get_page(params->dir_file_id, 0));
    BPlusTreeLeaf first_leaf(*params, buffer_manager.get_page(params->leaf_file_id, 0)); // just to see if BPT is empty
    is_empty = *first_leaf.value_count == 0;
}


void BPlusTree::bulk_import(OrderedFile& ordered_file) {
    ordered_file.begin_iter();

    if (is_empty) {
        // first leaf
        auto first_leaf = BPlusTreeLeaf(*params, buffer_manager.get_page(params->leaf_file_id, 0));
        *first_leaf.value_count = ordered_file.next_tuples(first_leaf.records, params->leaf_max_records);
        // root.dirs[0] = 0;
        if (ordered_file.has_more_tuples()) {
            *first_leaf.next_leaf = first_leaf.page.get_page_number() + 1;
        }
        first_leaf.page.make_dirty();
        is_empty = false;
    }
    else {
        // fill last page
        auto last_leaf = BPlusTreeLeaf(*params, buffer_manager.get_last_page(params->leaf_file_id));
        if (*last_leaf.value_count < params->leaf_max_records) {
            auto new_tuples = ordered_file.next_tuples(&last_leaf.records[*last_leaf.value_count * params->total_size],
                                                       params->leaf_max_records - *last_leaf.value_count);
            *last_leaf.value_count += new_tuples;
        }
        if (ordered_file.has_more_tuples()) {
            *last_leaf.next_leaf = last_leaf.page.get_page_number() + 1;
        }
        last_leaf.page.make_dirty();
    }

    while (ordered_file.has_more_tuples()) {
        auto new_leaf = BPlusTreeLeaf(*params, buffer_manager.append_page(params->leaf_file_id));
        *new_leaf.value_count = ordered_file.next_tuples(new_leaf.records, params->leaf_max_records);

        if (*new_leaf.value_count <= 0) {
            cout << "Wrong *new_leaf.value_count: " << *new_leaf.value_count << "\n";
        }

        if (ordered_file.has_more_tuples()) {
            *new_leaf.next_leaf = new_leaf.page.get_page_number() + 1;
        }
        root->bulk_insert(new_leaf);
        new_leaf.page.make_dirty();
    }
}


unique_ptr<BPlusTree::Iter> BPlusTree::get_range(const Record& min, const Record& max) {
    auto page_number_and_pos = root->search_leaf(min);
    return make_unique<Iter>(*params, page_number_and_pos.page_number, page_number_and_pos.result_index, max);
}


void BPlusTree::insert(const Record& record) {
    if (is_empty) {
        create_new(record, Record());
        is_empty = false;
        return;
    }
    root->insert(record, Record());
}


void BPlusTree::insert(const Record& key, const Record& value) {
    if (is_empty) {
        create_new(key, value);
        is_empty = false;
        return;
    }
    root->insert(key, value);
}


// Insert first key at root, create leaf
void BPlusTree::create_new(const Record& key, const Record& value) {
    Page& leaf_page = buffer_manager.get_page(params->leaf_file_id, 0);
    BPlusTreeLeaf first_leaf = BPlusTreeLeaf(*params, leaf_page);
    first_leaf.create_new(key, value);
}


unique_ptr<Record> BPlusTree::get(const Record& key) {
    return root->get(key);
}


bool BPlusTree::check() const {
    return root->check();
}

/******************************* Iter ********************************/
BPlusTree::Iter::Iter(const BPlusTreeParams& params, int leaf_page_number, int current_pos, const Record& max)
    : params(params), max(max)
{
    current_leaf = make_unique<BPlusTreeLeaf>(params, buffer_manager.get_page(params.leaf_file_id, leaf_page_number));
    this->current_pos = current_pos;
}


unique_ptr<Record> BPlusTree::Iter::next() {
    if (current_pos < current_leaf->get_value_count()) {
        unique_ptr<Record> res = current_leaf->get_record(current_pos);
        // check if res is less than max
        for (int i = 0; i < params.key_size; i++) {
            if (res->ids[i] < max.ids[i]) {
                current_pos++;
                return res;
            }
            else if (res->ids[i] > max.ids[i]) {
                return nullptr;
            }
            // continue if they were equal
        }
        current_pos++;
        return res; // res == max
    }
    else if (current_leaf->has_next()) {
        current_leaf = current_leaf->get_next_leaf();
        current_pos = 0;
        return next();
    }
    else {
        return nullptr;
    }
}
