#include "op.h"

#include "base/parser/grammar/grammar.h"
#include "base/parser/logical_plan/exceptions.h"
#include "base/parser/logical_plan/op/op_filter.h"
#include "base/parser/logical_plan/op/op_match.h"
#include "base/parser/logical_plan/op/op_select.h"

using namespace std;

unique_ptr<OpSelect> Op::get_select_plan(ast::Root& ast) {
    unique_ptr<Op> op_match = make_unique<OpMatch>(ast.graph_pattern);
    unique_ptr<Op> op_filter = make_unique<OpFilter>(ast.where, move(op_match));
    return make_unique<OpSelect>(ast.selection, move(op_filter));
}


unique_ptr<OpSelect> Op::get_select_plan(string query) {
    auto iter = query.begin();
    auto end = query.end();

    ast::Root ast;
    bool r = phrase_parse(iter, end, parser::root, parser::skipper, ast);
    if (r && iter == end) { // parsing succeeded
        ASTPrinter printer(cout);
        printer(ast);
        return Op::get_select_plan(ast);
    }
    else {
        throw ParsingException();
    }
}