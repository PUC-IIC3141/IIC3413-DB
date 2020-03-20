#ifndef RELATIONAL_MODEL__GRAPH_SCAN_H_
#define RELATIONAL_MODEL__GRAPH_SCAN_H_

#include "storage/index/bplus_tree/bplus_tree.h"
#include "relational_model/binding/binding_id_iter.h"
#include "base/ids/object_id.h"

#include <functional>
#include <list>
#include <memory>
#include <tuple>

class Record;
class VarId;


class GraphScan : public BindingIdIter {

private:
    int record_size;
    BPlusTree& bpt;
    std::vector<std::pair<ObjectId, int>> terms;
    std::vector<std::pair<VarId, int>> vars;
    std::unique_ptr<BPlusTree::Iter> it;

    BindingId* my_input;
    std::unique_ptr<BindingId> my_binding;

public:
    GraphScan(BPlusTree& bpt, std::vector<std::pair<ObjectId, int>> terms, std::vector<std::pair<VarId, int>> vars);
    ~GraphScan() = default;

    void begin(BindingId& input);
    void reset(BindingId& input);
    BindingId* next();
};

#endif //RELATIONAL_MODEL__GRAPH_SCAN_H_
