#include "bpt_merger.h"

#include <cstring>

using namespace std;

BptMerger::BptMerger(OrderedFile& ordered_file, BPlusTree& bpt)
    : ordered_file(ordered_file), bpt(bpt), record_size(bpt.params->total_size) { }


BptMerger::~BptMerger() = default;


void BptMerger::begin() {
    ordered_file.begin();

    auto min_range = vector<uint64_t>(record_size, 0);
    auto max_range = vector<uint64_t>(record_size, UINT64_MAX);
    bpt_iter = bpt.get_range(Record(min_range), Record(max_range));

    ordered_file_record = ordered_file.next_record();
    bpt_record = bpt_iter->next();
}


bool BptMerger::has_more_tuples() {
    return ordered_file_record != nullptr || bpt_record != nullptr;
}


uint_fast32_t BptMerger::next_tuples(uint64_t* output, uint_fast32_t max_tuples) {
    auto current_output = output;
    uint_fast32_t copied_tuples = 0;
    while (copied_tuples < max_tuples && (ordered_file_record != nullptr || bpt_record != nullptr)) {
        copied_tuples++;
        if (ordered_file_record == nullptr) {
            memcpy(
                current_output,
                bpt_record->ids.data(),
                record_size * sizeof(uint64_t)
            );
            bpt_record = bpt_iter->next();
        }
        else if (bpt_record == nullptr) {
            memcpy(
                current_output,
                ordered_file_record->ids.data(),
                record_size * sizeof(uint64_t)
            );
            ordered_file_record = ordered_file.next_record();
        }
        else if (*ordered_file_record < *bpt_record) {
            memcpy(
                current_output,
                ordered_file_record->ids.data(),
                record_size * sizeof(uint64_t)
            );
            ordered_file_record = ordered_file.next_record();
        }
        else if (*bpt_record < *ordered_file_record) {
            memcpy(
                current_output,
                bpt_record->ids.data(),
                record_size * sizeof(uint64_t)
            );
            bpt_record = bpt_iter->next();
        }
        else {
            throw logic_error("duplicated record at merging.");
        }
        current_output += record_size;
    }
    return copied_tuples;
}
