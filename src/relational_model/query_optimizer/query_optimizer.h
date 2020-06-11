#ifndef RELATIONAL_MODEL__QUERY_OPTIMIZER_H_
#define RELATIONAL_MODEL__QUERY_OPTIMIZER_H_

#include "base/ids/var_id.h"
#include "base/ids/object_id.h"
#include "base/ids/graph_id.h"
#include "base/graph/graph_object.h"
#include "base/parser/grammar/ast.h"
#include "base/parser/logical_plan/op/visitors/op_visitor.h"

#include <map>
#include <memory>
#include <vector>

class BindingIter;
class BindingIdIter;
class OpMatch;
class OpFilter;
class OpSelect;
class JoinPlan;

class QueryOptimizer : OpVisitor {
private:
    std::unique_ptr<BindingIter> tmp;
    std::map<std::string, VarId> id_map;
    std::map<std::string, GraphId> graph_ids; // graph_name to graph_id
    std::map<std::string, GraphId> var2graph_id;
    std::map<std::string, ObjectType> element_types;
    std::vector<std::pair<std::string, std::string>> select_items;
    int_fast32_t id_count = 0;

    VarId get_var_id(const std::string& var_name);
    ObjectId get_value_id(const ast::Value& value);
    GraphId search_graph_id(const std::string& graph_name);

    std::unique_ptr<BindingIdIter> get_greedy_join_plan(std::vector<std::unique_ptr<JoinPlan>>& base_plans);

public:
    QueryOptimizer();
    ~QueryOptimizer() = default;

    std::unique_ptr<BindingIter> exec(OpSelect&);

    void visit(OpSelect&);
    void visit(OpMatch&);
    void visit(OpFilter&);
    void visit(OpLabel&);
    void visit(OpProperty&);
    void visit(OpConnection&);
    void visit(OpLonelyNode&);
    void visit(OpNodeLoop&);
};

#endif // RELATIONAL_MODEL__QUERY_OPTIMIZER_H_
