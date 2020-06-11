#ifndef RELATIONAL_MODEL__GRAPH_SCAN_H_
#define RELATIONAL_MODEL__GRAPH_SCAN_H_

#include "storage/index/bplus_tree/bplus_tree.h"
#include "relational_model/binding/binding_id_iter.h"
#include "relational_model/execution/binding_id_iter/scan_ranges/scan_range.h"

#include <functional>
#include <list>
#include <memory>
#include <tuple>

class IndexScan : public BindingIdIter {
private:
    int record_size;
    BPlusTree& bpt;
    std::unique_ptr<BPlusTree::Iter> it;

    BindingId* my_input;
    std::unique_ptr<BindingId> my_binding;
    std::vector<std::unique_ptr<ScanRange>> ranges;

public:
    IndexScan(BPlusTree& bpt, std::vector<std::unique_ptr<ScanRange>> ranges);
    ~IndexScan() = default;

    void begin(BindingId& input);
    void reset(BindingId& input);
    BindingId* next();
};

#endif // RELATIONAL_MODEL__GRAPH_SCAN_H_
