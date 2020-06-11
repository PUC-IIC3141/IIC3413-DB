#ifndef BASE__VAR_ID_H_
#define BASE__VAR_ID_H_

#include <cstdint>

class VarId {
public:
    const int_fast32_t id;

    VarId(int_fast32_t id)
        : id(id) { }
    ~VarId() = default;

    bool is_null() const {
        return id == -1;
    }

    static VarId get_null() {
        return VarId(-1);
    }

    operator int_fast32_t() const {
        return id;
    }

    bool operator <(const VarId& rhs) const {
        return id < rhs.id;
    }

    bool operator ==(const VarId& rhs) const {
        return id == rhs.id;
    }

    bool operator !=(const VarId& rhs) const {
        return id != rhs.id;
    }
};

#endif // BASE__VAR_ID_H_
