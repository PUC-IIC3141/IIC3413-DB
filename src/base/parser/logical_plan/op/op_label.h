#ifndef BASE__OP_LABEL_H_
#define BASE__OP_LABEL_H_

#include "base/ids/graph_id.h"
#include "base/graph/graph_object.h"
#include "base/parser/logical_plan/op/op.h"

#include <string>

class OpLabel : public Op {
public:
    const std::string graph_name;
    const ObjectType type;
    const std::string var;
    const std::string label;

    OpLabel(std::string graph_name, ObjectType type, std::string var, std::string label)
        : graph_name(graph_name), type(type), var(std::move(var)), label(std::move(label)) { }

    ~OpLabel() = default;

    void accept_visitor(OpVisitor& visitor) {
        visitor.visit(*this);
    }
};

#endif // BASE__OP_LABEL_H_
