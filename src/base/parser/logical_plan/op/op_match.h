#ifndef BASE__OP_MATCH_H_
#define BASE__OP_MATCH_H_

#include "base/parser/logical_plan/exceptions.h"
#include "base/parser/logical_plan/op/op.h"
#include "base/parser/logical_plan/op/op_label.h"
#include "base/parser/logical_plan/op/op_property.h"
#include "base/parser/logical_plan/op/op_connection.h"

#include <set>
#include <memory>
#include <utility>
#include <variant>
#include <vector>

class OpMatch : public Op {
public:
    std::vector<std::unique_ptr<OpLabel>> labels;
    std::vector<std::unique_ptr<OpProperty>> properties;
    std::vector<std::unique_ptr<OpConnection>> connections;

    std::map<std::string, std::pair<GraphId, ObjectType>> var_info;
    int_fast32_t anonymous_var_count = 0;


    OpMatch(const std::vector<ast::LinearPattern>& graph_pattern) {
        for (auto& linear_pattern : graph_pattern) {
            auto graph_id = linear_pattern.graph_id;
            auto last_node_name = process_node(graph_id, linear_pattern.root);

            for (auto& step_path : linear_pattern.path) {
                auto current_node_name = process_node(graph_id, step_path.node);
                auto edge_name         = process_edge(graph_id, step_path.edge);

                if (step_path.edge.direction == ast::EdgeDirection::right) {
                    connections.push_back(std::make_unique<OpConnection>(graph_id, last_node_name, edge_name, current_node_name));
                }
                else {
                    connections.push_back(std::make_unique<OpConnection>(graph_id, current_node_name, edge_name, last_node_name));
                }
                last_node_name = std::move(current_node_name);
            }
        }
    }


    std::string process_node(const GraphId graph_id, const ast::Node& node) {
        std::string var_name;
        if (node.var.empty()) {
            var_name = "_n" + std::to_string(anonymous_var_count++);
        }
        else {
            var_name = node.var;

            auto search = var_info.find(var_name);

            if (search != var_info.end()) {
                // check is a node
                if ((*search).second.second != ObjectType::node) {
                    throw ParsingException();
                }
            }
            else { // not found
                var_info.insert({ var_name, std::make_pair(graph_id , ObjectType::node) });
            }
        }

        for (auto& label : node.labels) {
            labels.push_back(std::make_unique<OpLabel>(graph_id, ObjectType::node, var_name, label));
        }

        for (auto& property : node.properties) {
            properties.push_back(std::make_unique<OpProperty>(graph_id, ObjectType::node, var_name, property.key, property.value));
        }

        return var_name;
    }


    std::string process_edge(const GraphId graph_id, const ast::Edge& edge) {
        std::string var_name;
        if (edge.var.empty()) {
            var_name = "_e" + std::to_string(anonymous_var_count++);
        }
        else {
            var_name = edge.var;

            auto search = var_info.find(var_name);

            if (search != var_info.end()) {
                // check is an edge
                if ((*search).second.second != ObjectType::edge) {
                    throw ParsingException();
                }
            }
            else { // not found
                var_info.insert({ var_name, std::make_pair(graph_id , ObjectType::edge) });
            }
        }

        for (auto& label : edge.labels) {
            labels.push_back(std::make_unique<OpLabel>(graph_id, ObjectType::edge, var_name, label));
        }

        for (auto& property : edge.properties) {
            properties.push_back(std::make_unique<OpProperty>(graph_id, ObjectType::edge, var_name, property.key, property.value));
        }

        return var_name;
    }


    void accept_visitor(OpVisitor& visitor) {
        visitor.visit(*this);
    }
};

#endif //BASE__OP_MATCH_H_
