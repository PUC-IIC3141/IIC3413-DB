#include "check_var_names.h"

#include "base/parser/logical_plan/exceptions.h"
#include "base/parser/logical_plan/op/op_filter.h"
#include "base/parser/logical_plan/op/op_label.h"
#include "base/parser/logical_plan/op/op_property.h"
#include "base/parser/logical_plan/op/op_select.h"
#include "base/parser/logical_plan/op/op_match.h"
#include "base/parser/logical_plan/op/op_connection.h"
#include "base/parser/logical_plan/op/op_lonely_node.h"

void CheckVarNames::visit(OpSelect& op_select) {
    op_select.op->accept_visitor(*this);
    for (auto& select_item : op_select.select_items) {
        auto search = var_name2type.find(select_item.first);
        if (search == var_name2type.end()) {
            throw QuerySemanticException("Variable \"" + select_item.first + "\" used in SELECT is not declared in MATCH");
        }
    }
}


void CheckVarNames::visit(OpMatch& op_match) {
    var_name2type = op_match.var_name2type;
}


void CheckVarNames::visit(OpFilter& op_filter) {
    op_filter.op->accept_visitor(*this);
    if (op_filter.condition != nullptr)
        op_filter.condition->check_names(var_name2type);
}


void CheckVarNames::visit(OpLabel&) { };
void CheckVarNames::visit(OpProperty&) { };
void CheckVarNames::visit(OpConnection&) { };
void CheckVarNames::visit(OpLonelyNode&) { };
