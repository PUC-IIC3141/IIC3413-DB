#ifndef BASE__GREATER_OR_EQUALS_H_
#define BASE__GREATER_OR_EQUALS_H_

#include "base/graph/condition/comparisons/comparison.h"

class GreaterOrEquals : public Comparison {
public:
    GreaterOrEquals(ast::Statement const& statement)
        : Comparison(statement) { }

    bool compare(GraphObject& lhs, GraphObject& rhs) override {
        return lhs >= rhs;
    }
};

#endif // BASE__GREATER_OR_EQUALS_H_
