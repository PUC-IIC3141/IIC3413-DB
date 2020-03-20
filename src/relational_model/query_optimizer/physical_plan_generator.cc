#include "physical_plan_generator.h"

#include <iostream>
#include <set>

#include "base/binding/binding.h"
#include "base/graph/value/value_int.h"
#include "base/graph/value/value_bool.h"
#include "base/graph/value/value_float.h"
#include "base/graph/value/value_string.h"
#include "base/parser/logical_plan/op/op_filter.h"
#include "base/parser/logical_plan/op/op_match.h"
#include "base/parser/logical_plan/op/op_select.h"
#include "relational_model/graph/relational_graph.h"
#include "relational_model/relational_model.h"
#include "relational_model/physical_plan/binding_iter/filter.h"
#include "relational_model/physical_plan/binding_iter/match.h"
#include "relational_model/physical_plan/binding_iter/projection.h"
#include "relational_model/query_optimizer/query_optimizer_connection.h"
#include "relational_model/query_optimizer/query_optimizer_label.h"
#include "relational_model/query_optimizer/query_optimizer_property.h"

using namespace std;

PhysicalPlanGenerator::PhysicalPlanGenerator() { }


unique_ptr<BindingIter> PhysicalPlanGenerator::exec(OpSelect& op_select) {
    op_select.accept_visitor(*this);
    return move(tmp);
}


void PhysicalPlanGenerator::visit (OpSelect& op_select) {
    if (op_select.select_all) {
        op_select.op->accept_visitor(*this);
        tmp = make_unique<Projection>(move(tmp));
    }
    else {
        set<std::string> projection_vars;
        select_items = move(op_select.select_items);
        for (auto&& [key, value] : select_items) {
            projection_vars.insert(key + '.' + value);
        }
        op_select.op->accept_visitor(*this);
        tmp = make_unique<Projection>(move(tmp), move(projection_vars));
    }
}


void PhysicalPlanGenerator::visit(OpMatch& op_match) {
    VarId null_var { -1 };
    vector<unique_ptr<QueryOptimizerElement>> elements;
    var_info = op_match.var_info; // TODO: improve performance using move?

    for (auto& op_label : op_match.labels) {
        ObjectId label_id = RelationalModel::get_string_unmasked_id(op_label->label);
        VarId element_obj_id = get_var_id(op_label->var);
        elements.push_back(make_unique<QueryOptimizerLabel>(op_label->graph_id, element_obj_id, null_var, op_label->type, label_id));
    }

    for (auto& op_property : op_match.properties) {
        VarId element_obj_id = get_var_id(op_property->var);
        ObjectId key_id = RelationalModel::get_string_unmasked_id(op_property->key);
        ObjectId value_id;

        if (op_property->value.type() == typeid(string)) {
            auto val_str = boost::get<string>(op_property->value);
            value_id = RelationalModel::get_value_masked_id(ValueString(val_str));
        }
        else if (op_property->value.type() == typeid(int)) {
            auto val_int = boost::get<int>(op_property->value);
            value_id = RelationalModel::get_value_masked_id(ValueInt(val_int));
        }
        else if (op_property->value.type() == typeid(float)) {
            auto val_float = boost::get<float>(op_property->value);
            value_id = RelationalModel::get_value_masked_id(ValueFloat(val_float));
        }
        else if (op_property->value.type() == typeid(bool)) {
            auto val_bool = boost::get<bool>(op_property->value);
            value_id = RelationalModel::get_value_masked_id(ValueBool(val_bool));
        }
        else {
            throw logic_error("only strings supported for now.");
        }

        elements.push_back(make_unique<QueryOptimizerProperty>(
            op_property->graph_id, element_obj_id, null_var, null_var, op_property->type, key_id, value_id ));
    }

    // Properties from select
    for (auto&& [var, key] : select_items) {
        VarId element_obj_id = get_var_id(var);
        VarId value_var = get_var_id(var + '.' + key);
        ObjectId key_id = RelationalModel::get_string_unmasked_id(key);

        elements.push_back(make_unique<QueryOptimizerProperty>(
            var_info[var].first, element_obj_id, null_var, value_var, var_info[var].second, key_id, ObjectId(NULL_OBJECT_ID) ));
    }

    for (auto& op_connection : op_match.connections) {
        elements.push_back(make_unique<QueryOptimizerConnection>(
            op_connection->graph_id, get_var_id(op_connection->node_from), get_var_id(op_connection->node_to), get_var_id(op_connection->edge) ));
    }

    tmp = make_unique<Match>(move(elements), move(id_map));
}


void PhysicalPlanGenerator::visit(OpFilter& op_filter) {
    op_filter.op->accept_visitor(*this);
    if (op_filter.condition != nullptr) {
        tmp = make_unique<Filter>(move(tmp), move(op_filter.condition), move(var_info));
    }
    // else tmp stays the same
}


VarId PhysicalPlanGenerator::get_var_id(const std::string& var) {
    auto search = id_map.find(var);
    if (id_map.find(var) != id_map.end()) {
        return (*search).second;
    }
    else {
        VarId res = VarId(id_count++);
        id_map.insert({ var, res });
        return res;
    }
}


void PhysicalPlanGenerator::visit (OpLabel&) { }
void PhysicalPlanGenerator::visit (OpProperty&) { }
void PhysicalPlanGenerator::visit (OpConnection&) { }
