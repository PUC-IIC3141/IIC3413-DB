#ifndef BASE__OP_CONNECTION_H_
#define BASE__OP_CONNECTION_H_

#include "base/ids/graph_id.h"
#include "base/parser/logical_plan/op/op.h"

#include <string>

class OpConnection : public Op {
public:
    const GraphId graph_id;
    const std::string node_from;
    const std::string node_to;
    const std::string edge;

    OpConnection(GraphId graph_id, std::string node_from, std::string edge, std::string node_to)
        : graph_id(graph_id), node_from(std::move(node_from)), node_to(std::move(node_to)), edge(std::move(edge)) { }

    void accept_visitor(OpVisitor& visitor) {
        visitor.visit(*this);
    }

};

#endif //BASE__OP_CONNECTION_H_
