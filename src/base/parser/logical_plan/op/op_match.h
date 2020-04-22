#ifndef BASE__OP_MATCH_H_
#define BASE__OP_MATCH_H_

#include <set>
#include <map>
#include <memory>
#include <utility>
#include <variant>
#include <vector>

#include "base/parser/logical_plan/exceptions.h"
#include "base/parser/logical_plan/op/op.h"
#include "base/parser/logical_plan/op/op_label.h"
#include "base/parser/logical_plan/op/op_property.h"
#include "base/parser/logical_plan/op/op_connection.h"
#include "base/parser/logical_plan/op/op_lonely_node.h"

class OpMatch : public Op {
public:
    std::vector<std::unique_ptr<OpLabel>> labels;
    std::vector<std::unique_ptr<OpProperty>> properties;
    std::vector<std::unique_ptr<OpConnection>> connections;
    std::vector<std::unique_ptr<OpLonelyNode>> lonely_nodes;

    std::map<std::string, std::string> var_name2graph_name;
    std::map<std::string, ObjectType>  var_name2type;

    int_fast32_t anonymous_var_count = 0;

    OpMatch(const std::vector<ast::LinearPattern>& graph_pattern) {
        for (auto& linear_pattern : graph_pattern) {
            auto graph_name = linear_pattern.graph_name;
            auto last_node_name = process_node(graph_name, linear_pattern.root);

            if (linear_pattern.path.empty()
                && linear_pattern.root.labels.empty()
                && linear_pattern.root.properties.empty())
            {
                lonely_nodes.push_back(std::make_unique<OpLonelyNode>(graph_name, last_node_name));
            }
            for (auto& step_path : linear_pattern.path) {
                auto current_node_name = process_node(graph_name, step_path.node);
                auto edge_name         = process_edge(graph_name, step_path.edge);

                if (step_path.edge.direction == ast::EdgeDirection::right) {
                    connections.push_back(
                        std::make_unique<OpConnection>(graph_name, last_node_name, edge_name, current_node_name)
                    );
                }
                else {
                    connections.push_back(
                        std::make_unique<OpConnection>(graph_name, current_node_name, edge_name, last_node_name)
                    );
                }
                last_node_name = std::move(current_node_name);
            }
        }
    }


    std::string process_node(const std::string& graph_name, const ast::Node& node) {
        std::string var_name;
        if (node.var.empty()) {
            var_name = "_n" + std::to_string(anonymous_var_count++);
            var_name2graph_name.insert({ var_name, graph_name });
            var_name2type.insert({ var_name, ObjectType::node });
        }
        else {
            var_name = node.var;

            auto search = var_name2type.find(var_name);

            if (search != var_name2type.end()) {
                // check is a node
                if ((*search).second != ObjectType::node) {
                    throw QuerySemanticException("\"" + var_name
                        + "\" has already been declared as an Edge and cannot be a Node");
                }
                // check graph name is the same
                if (var_name2graph_name[var_name] != graph_name) {
                    throw QuerySemanticException("\"" + var_name + "\" has already been declared in graph '"
                        + var_name2graph_name[var_name]
                        + "' and cannot be declared in another graph (" + graph_name + ")");
                }
            }
            else { // not found
                var_name2graph_name.insert({ var_name, graph_name });
                var_name2type.insert({ var_name, ObjectType::node });
            }
        }

        for (auto& label : node.labels) {
            labels.push_back(std::make_unique<OpLabel>(graph_name, ObjectType::node, var_name, label));
        }

        for (auto& property : node.properties) {
            properties.push_back(
                std::make_unique<OpProperty>(graph_name, ObjectType::node, var_name, property.key, property.value)
            );
        }

        return var_name;
    }


    std::string process_edge(const std::string& graph_name, const ast::Edge& edge) {
        std::string var_name;
        if (edge.var.empty()) {
            var_name = "_e" + std::to_string(anonymous_var_count++);
            var_name2graph_name.insert({ var_name, graph_name });
            var_name2type.insert({ var_name, ObjectType::edge });
        }
        else {
            var_name = edge.var;

            auto search = var_name2type.find(var_name);

            if (search != var_name2type.end()) {
                // check is an edge
                if ((*search).second != ObjectType::edge) {
                    throw QuerySemanticException("\"" + var_name
                        + "\" has already been declared as a Node and cannot be an Edge");
                }
                // check graph name is the same
                if (var_name2graph_name[var_name] != graph_name) {
                    throw QuerySemanticException("\"" + var_name + "\" has already been declared in graph '"
                        + var_name2graph_name[var_name]
                        + "' and cannot be declared in another graph (" + graph_name + ")");
                }
            }
            else { // not found
                var_name2graph_name.insert({ var_name, graph_name });
                var_name2type.insert({ var_name, ObjectType::edge });
            }
        }

        for (auto& label : edge.labels) {
            labels.push_back(std::make_unique<OpLabel>(graph_name, ObjectType::edge, var_name, label));
        }

        for (auto& property : edge.properties) {
            properties.push_back(
                std::make_unique<OpProperty>(graph_name, ObjectType::edge, var_name, property.key, property.value)
            );
        }

        return var_name;
    }


    void accept_visitor(OpVisitor& visitor) {
        visitor.visit(*this);
    }
};

#endif //BASE__OP_MATCH_H_
