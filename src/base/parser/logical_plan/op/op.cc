#include "op.h"

#include "base/parser/grammar/grammar.h"
#include "base/parser/logical_plan/exceptions.h"
#include "base/parser/logical_plan/op/op_filter.h"
#include "base/parser/logical_plan/op/op_match.h"
#include "base/parser/logical_plan/op/op_select.h"
#include "base/parser/logical_plan/op/visitors/check_var_names.h"

using namespace std;

unique_ptr<OpSelect> Op::get_select_plan(ast::Root& ast) {
    unique_ptr<Op> op_match = make_unique<OpMatch>(ast.graph_pattern);
    unique_ptr<Op> op_filter = make_unique<OpFilter>(ast.where, move(op_match));
    int_fast32_t limit = 0;
    if (ast.limit) {
        limit = ast.limit.get();
        if (limit <= 0) {
            throw QuerySemanticException("LIMIT must be a positive number");
        }
    }
    return make_unique<OpSelect>(ast.selection, move(op_filter), limit);
}


unique_ptr<OpSelect> Op::get_select_plan(string query) {
    auto iter = query.begin();
    auto end = query.end();

    ast::Root ast;
    bool r = phrase_parse(iter, end, parser::root, parser::skipper, ast);
    if (r && iter == end) { // parsing succeeded
        if (ast.explain) {
            ASTPrinter printer(cout);
            printer(ast);
        }
        auto res =  Op::get_select_plan(ast);
        check_select_plan(*res);
        return res;
    }
    else {
        throw QueryParsingException();
    }
}


void Op::check_select_plan(OpSelect& op_select) {
    auto check_var_names = CheckVarNames();
    check_var_names.visit(op_select);
}
