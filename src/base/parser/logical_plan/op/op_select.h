#ifndef BASE__OP_SELECT_H_
#define BASE__OP_SELECT_H_

#include <string>
#include <tuple>
#include <vector>
#include <memory>

#include "base/parser/logical_plan/op/op.h"

class OpSelect : public Op {
public:
    uint_fast32_t limit;
    bool select_all;
    // pair <var_name, key_name>
    std::vector< std::pair<std::string, std::string> > select_items;
    std::unique_ptr<Op> op;

    OpSelect(const boost::variant<ast::All, std::vector<ast::Element>>& selection, std::unique_ptr<Op> op,
             uint_fast32_t limit);
    ~OpSelect() = default;
    void accept_visitor(OpVisitor&);
};

#endif // BASE__OP_SELECT_H_
