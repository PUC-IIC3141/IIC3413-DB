/**
 * Will throw an exception if unexisting nodes/edges ares used inside SELECT or WHERE
 */
#ifndef BASE__CHECK_VAR_NAMES_EXISTS_H_
#define BASE__CHECK_VAR_NAMES_EXISTS_H_

#include <map>
#include <string>

#include "base/graph/graph_object.h"
#include "base/parser/logical_plan/op/visitors/op_visitor.h"

class CheckVarNames : public OpVisitor {
private:
    std::map<std::string, ObjectType> var_name2type;

public:
    void visit(OpSelect&) override;
    void visit(OpMatch&) override;
    void visit(OpFilter&) override;
    void visit(OpLabel&) override;
    void visit(OpProperty&) override;
    void visit(OpConnection&) override;
    void visit(OpLonelyNode&) override;
    void visit(OpNodeLoop&) override;
};

#endif // BASE__CHECK_VAR_NAMES_EXISTS_H_
