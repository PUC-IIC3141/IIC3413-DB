#ifndef BASE__OP_NODE_LOOP_H_
#define BASE__OP_NODE_LOOP_H_

#include "base/ids/graph_id.h"
#include "base/parser/logical_plan/op/op.h"

#include <string>

class OpNodeLoop : public Op {
public:
    const std::string graph_name;
    const std::string node;
    const std::string edge;

    OpNodeLoop(std::string graph_name, std::string node, std::string edge)
        : graph_name(graph_name), node(std::move(node)), edge(std::move(edge)) { }

    void accept_visitor(OpVisitor& visitor) {
        visitor.visit(*this);
    }
};

#endif // BASE__OP_NODE_LOOP_H_
