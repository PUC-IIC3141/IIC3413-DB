#ifndef RELATIONAL_MODEL__TOTAL_SCAN_H_
#define RELATIONAL_MODEL__TOTAL_SCAN_H_

#include "base/ids/graph_id.h"
#include "base/ids/object_id.h"
#include "base/ids/var_id.h"
#include "storage/index/bplus_tree/bplus_tree.h"
#include "relational_model/binding/binding_id_iter.h"

#include <functional>
#include <list>
#include <memory>
#include <tuple>

class TotalScan : public BindingIdIter {
private:
    int record_size;
    BPlusTree& bpt;
    std::unique_ptr<BPlusTree::Iter> it;
    std::vector<std::pair<VarId, int>> vars;

    BindingId* my_input;
    std::unique_ptr<BindingId> my_binding;

public:
    TotalScan(BPlusTree& bpt, std::vector<std::pair<VarId, int>> vars);
    ~TotalScan() = default;

    void begin(BindingId& input);
    void reset(BindingId& input);
    BindingId* next();
};

#endif // RELATIONAL_MODEL__TOTAL_SCAN_H_
