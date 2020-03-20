#ifndef BASE__LESS_THAN_H_
#define BASE__LESS_THAN_H_

#include "base/graph/condition/comparisons/comparison.h"

class LessThan : public Comparison {
public:
    LessThan(ast::Statement const& statement)
        : Comparison(statement) { }

    bool compare(GraphObject& lhs, GraphObject& rhs) override {
        return lhs < rhs;
    }
};

#endif // BASE__LESS_THAN_H_
