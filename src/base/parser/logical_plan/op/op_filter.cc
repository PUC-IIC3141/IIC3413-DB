#include "op_filter.h"

using namespace std;


OpFilter::OpFilter(boost::optional<ast::Formula> const& optional_formula, unique_ptr<Op> op)
    : op(move(op))
{
    FormulaVisitor visitor = FormulaVisitor();
    condition = visitor(optional_formula);
}


void OpFilter::accept_visitor(OpVisitor& visitor) {
    visitor.visit(*this);
}
