#ifndef RELATIONAL_MODEL__BULK_IMPORT_GRAMMAR_H_
#define RELATIONAL_MODEL__BULK_IMPORT_GRAMMAR_H_

#include "bulk_import_ast.h"
#include "bulk_import_ast_adapted.h"

#include <boost/spirit/home/x3.hpp>

namespace bulk_import_parser
{
    namespace x3 = boost::spirit::x3;
    namespace ascii = boost::spirit::x3::ascii;

    // using x3::lit;
    using x3::lexeme;
    using x3::no_case;
    using x3::alnum;
    // using x3::alpha;
    // using x3::graph;
    using x3::int32;
    // using x3::int_;
    // using x3::float_;
    x3::real_parser<float, x3::strict_real_policies<float> > const float_ = {};
    using x3::attr;
    using x3::omit;
    // using x3::space;
    using x3::blank;
    // using x3::skip;
    using x3::no_skip;
    using x3::eol;
    using x3::eoi;
    // using x3::string;

    using ascii::char_;

    auto const skipper =
        blank;//"//" >> *(char_ - eol) >> (eol | eoi);

    // Declare rules
    x3::rule<class node, bulk_import_ast::Node>
        node = "node";
    x3::rule<class edge, bulk_import_ast::Edge>
        edge = "edge";
    x3::rule<class property, bulk_import_ast::Property>
        property = "property";
    x3::rule<class value, bulk_import_ast::Value>
        value = "value";


    ///////////////////////////////////////////////////////////
    //   GRAMMAR
    ///////////////////////////////////////////////////////////
    auto const key =
        // lexeme[+alnum];
        lexeme[+char_("A-Za-zÁÉÍÓÚáéíóúÑñèç0-9#'")];

    auto const label =
        // lexeme[':' >> +alnum];
        lexeme[':' >> +char_("A-Za-zÁÉÍÓÚáéíóúÑñèç0-9#'")];


    auto const boolean = lexeme[
        (no_case["true"]  >> attr(true)) |
        (no_case["false"] >> attr(false))
    ];

    auto const string =
        ('"'  >> *(char_ - '"')  >> '"') |
        ('\'' >> *(char_ - '\'') >> '\'');

    auto const value_def =
        string | float_ | int32 | boolean;

    auto const property_def =
        key >> ':' >> value;

    auto const node_def =
        '(' >> int32 >> ')'
        >> *label
        >> *property
        >> (eol|eoi);

    auto const edge_dir = lexeme[
        ("->" >> attr(bulk_import_ast::EdgeDirection::right)) |
        ("<-" >> attr(bulk_import_ast::EdgeDirection::left))
    ];

    auto const edge_def =
        '(' >> int32 >> ')' >> edge_dir >> '(' >> int32 >> ')'
        >> *label
        >> *property
        >> (eol|eoi);

    BOOST_SPIRIT_DEFINE(
        node,
        edge,
        property,
        value
    );
}

#endif // RELATIONAL_MODEL__BULK_IMPORT_GRAMMAR_H_
