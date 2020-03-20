// Uncomment for parser debugging:
// #define BOOST_SPIRIT_X3_DEBUG

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/spirit/home/x3.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <vector>

#include "base/parser/grammar/ast.h"
#include "base/parser/grammar/grammar.h"
#include "base/parser/grammar/ast_printer.h"

using namespace std;

int main(int argc, char **argv)
{
    char const* filename;
    if (argc > 1) {
        filename = argv[1];
    }
    else {
        cerr << "Error: No input file provided." << endl;
        return 1;
    }

    ifstream in(filename, ios_base::in);

    if (!in) {
        cerr << "Error: Could not open input file: "
            << filename << endl;
        return 1;
    }

    string storage; // We will read the contents here.
    in.unsetf(ios::skipws); // No white space skipping!
    copy(
        istream_iterator<char>(in),
        istream_iterator<char>(),
        back_inserter(storage));

    ast::Root ast; // Our tree

    using boost::spirit::x3::ascii::space;
    string::const_iterator iter = storage.begin();
    string::const_iterator end = storage.end();
    bool r = phrase_parse(iter, end, parser::root, parser::skipper, ast);

    if (r && iter == end) {
        cout << "-------------------------\n";
        cout << " Parsing succeeded\n";
        cout << "-------------------------\n";
        // Get AST printer
        ASTPrinter printer(cout);
        printer(ast);

        // // Get first visitor
        // visitors::assignVarIDs varIDs_visitor;

        // try
        // {
        //     map<string, uint_fast32_t> idMap = varIDs_visitor.visit(ast);

        //     // Print map obtained
        //     cout << "\nMap obtained at first step:\n";
        //     for(auto const& t: idMap) {
        //         cout << "Variable(" << t.first << ") -> " << "VarId(" << t.second << "),\n";
        //     }

        //     // Get following visitors
        //     visitors::assignEntities entities_visitor(idMap);
        //     visitors::assignLabels labels_visitor(idMap);
        //     visitors::asssignProperties properties_visitor(idMap);
        //     visitors::assignConnections connections_visitor(idMap);

        //     entities_visitor(ast);
        //     auto id2type = entities_visitor.get_id2type();

        //     cout << "\nMap obtained at second step:\n";
        //     string s;
        //     for(auto const& t: id2type) {
        //         switch (t.second)
        //         {
        //         case ObjectType::NODE:
        //             s = "NODE";
        //             break;
        //         case ObjectType::EDGE:
        //             s = "EDGE";
        //             break;
        //         default:
        //             break;
        //         }
        //         cout << "Entity(" << t.first << ", " << s << "),\n";
        //     }

        //     // 3rd Visitor
        //     labels_visitor(ast);
        //     map<uint_fast32_t, vector<string>> labelMap = labels_visitor.get_labelMap();

        //     // Print map obtained
        //     cout << "\nMap obtained at third step:\n";
        //     for(auto const& t: labelMap) {
        //         for(auto const& d: t.second) {
        //             cout << "Label(" << t.first << ", " <<  d << "),\n";
        //         }

        //     }

        //     // 4th Visitor
        //     properties_visitor(ast);
        //     map<uint_fast32_t, map<string, ast::value>> propertyMap =
        //                                     properties_visitor.get_property_map();

        //     cout << "\nMap obtained at fourth step:\n";

        //     for(auto const& t: propertyMap) {
        //         for(auto const &s: t.second) {
        //             cout << "Property(" << t.first << ", " << s.first << ", ";
        //             printer(s.second);
        //             cout << ")\n";
        //         }
        //     }

        //     // 5th Visitor
        //     connections_visitor(ast);
        //     vector<array<uint_fast32_t, 3>> connections = connections_visitor.get_connections();

        //     // Print vector obtained
        //     cout << "\nVector obtained at fifth step:\n";
        //     for(auto const& t: connections) {
        //         cout << "Connection(" << t[0] << ", " << t[1] << ", " << t[2] << "),\n";
        //     }

        // } // try

        // catch (const std::exception& e) {
        //     cerr << "Error while processing query:\n" << e.what() << endl;
        //     return 1;
        // }
        return 0;
    }
    else
    {
        string::const_iterator some = iter + 30;
        string context(iter, (some > end) ? end : some);
        cout << "----------------------------\n";
        cerr << " Parsing failed\n";
        cerr << " stopped at: \": " << context << "...\"\n";
        cout << "----------------------------\n";
        return 1;
    }
}