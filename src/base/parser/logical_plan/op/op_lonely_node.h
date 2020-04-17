#ifndef BASE__OP_LONELY_NODE_H_
#define BASE__OP_LONELY_NODE_H_

#include "base/ids/graph_id.h"
#include "base/parser/logical_plan/op/op.h"

#include <string>

class OpLonelyNode : public Op {
public:
    const std::string graph_name;
    const std::string var;

    OpLonelyNode(std::string graph_name, std::string var)
        : graph_name(graph_name), var(std::move(var)) { }

    ~OpLonelyNode() = default;

    void accept_visitor(OpVisitor& visitor) {
        visitor.visit(*this);
    }
};

#endif // BASE__OP_LONELY_NODE_H_
