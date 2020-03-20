#ifndef RELATIONAL_MODEL__BULK_IMPORT_AST_ADAPTED_H_
#define RELATIONAL_MODEL__BULK_IMPORT_AST_ADAPTED_H_

#include "bulk_import_ast.h"

#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(bulk_import_ast::Node,
    id, labels, properties
)

BOOST_FUSION_ADAPT_STRUCT(bulk_import_ast::Edge,
    left_id, direction, right_id, labels, properties
)

BOOST_FUSION_ADAPT_STRUCT(bulk_import_ast::Property,
    key, value
)

#endif // RELATIONAL_MODEL__BULK_IMPORT_AST_ADAPTED_H_
