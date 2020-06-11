#include "match.h"

#include <cassert>
#include <limits>

#include "storage/index/object_file/object_file.h"
#include "relational_model/binding/binding_match.h"

using namespace std;

Match::Match(std::unique_ptr<BindingIdIter> root, std::map<std::string, VarId> var_pos)
    : root(move(root)), var_pos(move(var_pos)) { }


void Match::begin() {
    binding_id = make_unique<BindingId>(var_pos.size());
    root->begin(*binding_id);
}


unique_ptr<Binding> Match::next() {
    auto binding_id_ptr = root->next();
    if (binding_id_ptr != nullptr) {
        auto binding_id_copy = make_unique<BindingId>(*binding_id_ptr);
        return make_unique<BindingMatch>(var_pos, move(binding_id_copy));
    } else {
        return nullptr;
    }
}
