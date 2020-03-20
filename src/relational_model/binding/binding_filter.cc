#include "binding_filter.h"

#include "relational_model/relational_model.h"
#include "relational_model//graph/relational_graph.h"

using namespace std;

BindingFilter::BindingFilter(Binding& binding, map<string, pair<GraphId, ObjectType>>& var_info)
    : binding(binding), var_info(var_info) { }


void BindingFilter::print() const {
    throw std::logic_error("Binding filter only intended to be used by get()");
}


std::shared_ptr<GraphObject> BindingFilter::operator[](const std::string&) {
    throw std::logic_error("Binding filter only intended to be used by get()");
}


std::shared_ptr<GraphObject> BindingFilter::get(const std::string& var, const std::string& key) {
    auto search_var = var + "." + key;
    auto value = binding[search_var];
    if (value != nullptr) {
        return value;
    }
    auto search = cache.find(search_var);
    if (search != cache.end()) {
        return (*search).second;
    }
    else { // no esta en el cache ni el el binding original
        auto info = var_info[var];
        auto key_object_id = RelationalModel::get_string_unmasked_id(key);
        auto var_value = binding[var];

        unique_ptr<BPlusTree::Iter> it = nullptr;
        if (info.second == ObjectType::node) {
            Node node = static_cast<const Node&>(*var_value);
            auto& graph = RelationalModel::get_graph(info.first);
            it = graph.node2prop->get_range(
                Record(node.id | NODE_MASK, key_object_id, 0),
                Record(node.id | NODE_MASK, key_object_id, UINT64_MAX)
            );
        }
        else {
            Edge edge = static_cast<const Edge&>(*var_value);
            auto& graph = RelationalModel::get_graph(info.first);
            it = graph.edge2prop->get_range(
                Record(edge.id | EDGE_MASK, key_object_id, 0),
                Record(edge.id | EDGE_MASK, key_object_id, UINT64_MAX)
            );
        }

        auto res = it->next();
        if (res != nullptr) {
            auto value_obj_id = ObjectId(res->ids[2]);
            auto res = RelationalModel::get_graph_object(value_obj_id);
            cache.insert({ search_var, res });
            return res;
        }
        else {
            cache.insert({ search_var, nullptr });
            return nullptr;
        }
    }
}
