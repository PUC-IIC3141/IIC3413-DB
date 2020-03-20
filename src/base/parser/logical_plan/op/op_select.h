#ifndef BASE__OP_SELECT_H_
#define BASE__OP_SELECT_H_

#include <string>
#include <tuple>
#include <vector>
#include <memory>

#include "base/parser/logical_plan/op/op.h"

class OpSelect : public Op {
public:
    // pair <var_name, key_name>
    std::vector< std::pair<std::string, std::string> > select_items;
    std::unique_ptr<Op> op;
    bool select_all;

    OpSelect(const boost::variant<ast::All, std::vector<ast::Element>>& selection, std::unique_ptr<Op> op);
    ~OpSelect() = default;
    void accept_visitor(OpVisitor&);
};

#endif //BASE__OP_SELECT_H_
