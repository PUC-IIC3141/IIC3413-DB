#ifndef BASE__AST_ADAPTED_H_
#define BASE__AST_ADAPTED_H_

#include "base/parser/grammar/ast.h"

#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(ast::Root,
    selection, graph_pattern, where
)

BOOST_FUSION_ADAPT_STRUCT(ast::LinearPattern,
    graph_id, root, path
)

BOOST_FUSION_ADAPT_STRUCT(ast::StepPath,
    edge, node
)

BOOST_FUSION_ADAPT_STRUCT(ast::Node,
    var, labels, properties
)

BOOST_FUSION_ADAPT_STRUCT(ast::Edge,
    var, labels, properties, direction
)

BOOST_FUSION_ADAPT_STRUCT(ast::Property,
    key, value
)

BOOST_FUSION_ADAPT_STRUCT(ast::Element,
    function, variable, key
)

BOOST_FUSION_ADAPT_STRUCT(ast::Statement,
    lhs, comparator, rhs
)

BOOST_FUSION_ADAPT_STRUCT(ast::StepFormula,
    op, condition
)

BOOST_FUSION_ADAPT_STRUCT(ast::Formula,
    root, path
)

BOOST_FUSION_ADAPT_STRUCT(ast::Condition,
    negation, content
)

#endif // BASE__AST_ADAPTED_H_
