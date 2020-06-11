#ifndef BASE__VALUE_H_
#define BASE__VALUE_H_

#include <memory>
#include <string>
#include <vector>

#include "base/graph/graph_object.h"

class Value : public GraphObject {
public:
    virtual ~Value() { };
};

#endif // BASE__VALUE_H_
