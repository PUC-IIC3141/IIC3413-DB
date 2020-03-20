#ifndef BASE__GRAPH_ID_H_
#define BASE__GRAPH_ID_H_

#include <cstdint>

class GraphId {
public:
    uint64_t id;

    GraphId()
        : id(0) { }
    GraphId(uint64_t id)
        : id(id) { }
    ~GraphId() = default;

    bool operator<(const GraphId other) const {
        return this->id < other.id;
    }
};

#endif //BASE__GRAPH_ID_H_
