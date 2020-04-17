#ifndef BASE__COMPARISON_H_
#define BASE__COMPARISON_H_

#include <memory>

#include "base/binding/binding.h"
#include "base/parser/grammar/ast.h"
#include "base/parser/grammar/value_visitor.h"
#include "base/graph/condition/condition.h"
#include "base/graph/condition/value_assign.h"
#include "base/graph/condition/value_assign_constant.h"
#include "base/graph/condition/value_assign_variable.h"
#include "base/graph/value/value.h"

class Comparison : public Condition {
public:
    std::unique_ptr<ValueAssign> rhs;
    std::unique_ptr<ValueAssign> lhs;

    Comparison(ast::Statement const& statement) {
        lhs = std::make_unique<ValueAssignVariable>(statement.lhs.variable, statement.lhs.key);

        if (statement.rhs.type() == typeid(ast::Element)) {
            auto casted_rhs = boost::get<ast::Element>(statement.rhs);
            rhs = std::make_unique<ValueAssignVariable>(casted_rhs.variable, casted_rhs.key);
        }
        else {
            auto casted_rhs = boost::get<ast::Value>(statement.rhs);
            auto visitor = ValueVisitor();
            auto value = visitor(casted_rhs);
            rhs = std::make_unique<ValueAssignConstant>(move(value));
        }
     }

    virtual ~Comparison() { };

    virtual bool compare(GraphObject& lhs, GraphObject& rhs) = 0;

    bool eval(Binding& binding) {

        auto left_value = lhs->get_value(binding);
        auto right_value = rhs->get_value(binding);

        if (left_value != nullptr) {
            return compare(*left_value, *right_value);
        }
        else {
            return false;
        }
    }

    ConditionType type() {
        return ConditionType::comparison;
    }

    void check_names(std::map<std::string, ObjectType>& m) {
        lhs->check_names(m);
        rhs->check_names(m);
    }
};

#endif // BASE__COMPARISON_H_
