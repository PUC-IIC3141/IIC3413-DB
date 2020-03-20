#ifndef BASE__GREATER_THAN_H_
#define BASE__GREATER_THAN_H_

#include "base/graph/condition/comparisons/comparison.h"

class GreaterThan : public Comparison {
public:
    GreaterThan(ast::Statement const& statement)
        : Comparison(statement) { }

    bool compare(GraphObject& lhs, GraphObject& rhs) override {
        return lhs > rhs;
    }
};

#endif // BASE__GREATER_THAN_H_
