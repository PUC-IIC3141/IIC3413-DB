#ifndef BASE__OP_PROPERTY_H_
#define BASE__OP_PROPERTY_H_

#include "base/ids/graph_id.h"
#include "base/graph/graph_object.h"
#include "base/parser/logical_plan/op/op.h"

#include <string>

class OpProperty : public Op {
public:
    const GraphId graph_id;
    const ObjectType type;
    const std::string var;
    const std::string key;
    const ast::Value value;

    OpProperty(GraphId graph_id, ObjectType type, std::string var, std::string key, ast::Value value)
        : graph_id(graph_id), type(type), var(std::move(var)), key(std::move(key)), value(std::move(value)) { }

    void accept_visitor(OpVisitor& visitor) {
        visitor.visit(*this);
    }
};

#endif //BASE__OP_PROPERTY_H_
