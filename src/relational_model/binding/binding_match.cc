#include "binding_match.h"

#include "relational_model/relational_model.h"
#include "relational_model/binding/binding_id.h"
#include "relational_model/physical_plan/binding_id_iter/graph_scan.h"

using namespace std;

BindingMatch::BindingMatch(const map<string, VarId>& var_pos, unique_ptr<BindingId> binding_id)
    : var_pos(var_pos), binding_id(move(binding_id)) { }


BindingMatch::~BindingMatch() { }


std::string BindingMatch::to_string() const {
    string result;
    result.reserve(64);
    result += '{';
    bool first = true;
    for (auto&& [var, varid] : var_pos) {
        auto graph = ((*binding_id)[varid] & RelationalModel::GRAPH_MASK) >> RelationalModel::GRAPH_OFFSET;
        auto type = ((*binding_id)[varid] & RelationalModel::TYPE_MASK) >> RelationalModel::TYPE_OFFSET;
        auto unmasked_id = (*binding_id)[varid] & RelationalModel::UNMASK;
        if (first) {
            first = false;
        }
        else {
            result += ", ";
        }
        result += var;
        result += ":Id(";
        result += std::to_string(graph);
        result += ',';
        result += std::to_string(type);
        result += ',';
        result += std::to_string(unmasked_id);
        result += ')';
    }
    result += "}\n";
    return result;
}


shared_ptr<GraphObject> BindingMatch::operator[](const string& var) {
    // search in the cache map
    auto cache_search = cache.find(var);
    if (cache_search != cache.end()) { // Found in the cache
        return (*cache_search).second;
    }
    else {                             // Not found in the cache
        // search in the binding_id
        auto var_pos_search = var_pos.find(var);
        if (var_pos_search != var_pos.end()) {
            auto var_id = (*var_pos_search).second;
            auto object_id = (*binding_id)[var_id];
            auto value = RelationalModel::get_graph_object(object_id);
            cache.insert({ var, value });
            return value;
        }
        else return nullptr;
    }
}


shared_ptr<GraphObject> BindingMatch::get(const string& var, const string& key) {
    return (*this)[var + "." + key];
}
