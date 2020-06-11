#ifndef BASE__GRAPH_ID_H_
#define BASE__GRAPH_ID_H_

#include <cstdint>

class GraphId {
public:
    static constexpr uint64_t DEFAULT_GRAPH_ID = 0;
    uint64_t id;

    GraphId()
        : id(0) { }
    GraphId(uint64_t id)
        : id(id) { }
    ~GraphId() = default;

    operator uint64_t() const {
        return id;
    }

    bool operator<(const GraphId other) const {
        return this->id < other.id;
    }

    bool is_default() {
        return id == DEFAULT_GRAPH_ID;
    }
};

#endif // BASE__GRAPH_ID_H_
