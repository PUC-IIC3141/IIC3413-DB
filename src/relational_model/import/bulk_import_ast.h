#ifndef RELATIONAL_MODEL__BULK_IMPORT_AST_H_
#define RELATIONAL_MODEL__BULK_IMPORT_AST_H_

#include <iostream>
#include <string>

#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>

namespace bulk_import_ast {
    namespace x3 = boost::spirit::x3;

    typedef boost::variant<std::string, int64_t, float, bool> Value;

    struct Property {
        std::string key;
        Value value;
    };

    struct Node {
        uint64_t id;
        std::vector<std::string> labels;
        std::vector<Property> properties;
    };

    enum class EdgeDirection { right, left };

    struct Edge {
        uint64_t left_id;
        uint64_t right_id;
        std::vector<std::string> labels;
        std::vector<Property> properties;
        EdgeDirection direction;
    };
}

#endif // RELATIONAL_MODEL__BULK_IMPORT_AST_H_
