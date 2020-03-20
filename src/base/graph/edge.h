#ifndef BASE__EDGE_H_
#define BASE__EDGE_H_

#include "base/graph/graph_object.h"

class Edge : public GraphObject {
public:
    const uint64_t id;

    Edge(uint64_t id)
        : id(id) { }
    ~Edge() = default;

    uint64_t get_id() { return id; }

    std::string to_string() const override {
        return "Edge(" + std::to_string(id) + ")";
    }

    ObjectType type() const override {
        return ObjectType::edge;
    }

    bool operator==(const GraphObject& rhs) const override {
        if (rhs.type() == ObjectType::edge) {
            const auto& casted_rhs = static_cast<const Edge&>(rhs);
            return this->id == casted_rhs.id;
        }
        else return false;
    }

    bool operator!=(const GraphObject& rhs) const override {
        if (rhs.type() == ObjectType::edge) {
            const auto& casted_rhs = static_cast<const Edge&>(rhs);
            return this->id != casted_rhs.id;
        }
        else return true;
    }

    bool operator<=(const GraphObject&) const override {
        return false;
    }

    bool operator>=(const GraphObject&) const override {
        return false;
    }

    bool operator<(const GraphObject&) const override {
        return false;
    }

    bool operator>(const GraphObject&) const override {
        return false;
    }
};

#endif // BASE__EDGE_H_
