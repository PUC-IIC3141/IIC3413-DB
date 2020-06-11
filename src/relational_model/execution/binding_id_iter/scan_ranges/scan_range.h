#ifndef RELATIONAL_MODEL__SCAN_RANGE_H_
#define RELATIONAL_MODEL__SCAN_RANGE_H_

#include <cstdint>

#include "base/ids/object_id.h"
#include "relational_model/binding/binding_id.h"

class ScanRange {
public:
    virtual ~ScanRange() { };
    virtual uint64_t get_min(BindingId& input) = 0;
    virtual uint64_t get_max(BindingId& input) = 0;
    virtual void try_assign(BindingId& my_binding, ObjectId) = 0;
};

#endif // RELATIONAL_MODEL__SCAN_RANGE_H_
