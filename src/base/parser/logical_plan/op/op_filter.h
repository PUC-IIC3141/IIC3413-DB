#ifndef BASE__OP_FILTER_H_
#define BASE__OP_FILTER_H_

#include "base/parser/logical_plan/op/op.h"
#include "base/graph/condition/constant_condition.h"
#include "base/graph/condition/conjunction.h"
#include "base/graph/condition/comparisons/equals.h"
#include "base/graph/condition/comparisons/not_equals.h"
#include "base/graph/condition/comparisons/less_than.h"
#include "base/graph/condition/comparisons/greater_than.h"
#include "base/graph/condition/comparisons/less_or_equals.h"
#include "base/graph/condition/comparisons/greater_or_equals.h"
#include "base/graph/condition/disjunction.h"
#include "base/graph/condition/negation.h"

#include <map>
#include <memory>
#include <vector>

class OpFilter : public Op {
public:
    std::unique_ptr<Condition> condition;
    std::unique_ptr<Op> op;

    OpFilter(boost::optional<ast::Formula> const& optional_formula, std::unique_ptr<Op> op);
    void accept_visitor(OpVisitor&);
};

class FormulaVisitor : public boost::static_visitor<std::unique_ptr<Condition>> {

public:
    // std::vector<std::pair<std::string, std::string>> properties; // to save properties used only on Filter
    // std::map<std::string, VarId>& id_map;

    FormulaVisitor() { }


    std::unique_ptr<Condition> operator()(boost::optional<ast::Formula> const& optional_formula) const {
        if (optional_formula) {
            ast::Formula formula = static_cast<ast::Formula>(optional_formula.get());
            return (*this)(formula);
        }
        else {
            return nullptr;
        }
    }


    std::unique_ptr<Condition> operator()(ast::Formula const& formula) const {
        std::vector<std::unique_ptr<Condition>> tmp_disjunction;
        auto tmp = (*this)(formula.root);

        for (auto const& step_formula : formula.path) {
            auto step = (*this)(step_formula.condition);
            if (step_formula.op.type() == typeid(ast::Or)) { // OR
                tmp_disjunction.push_back(std::move(tmp));
                tmp = std::move(step);
            }
            else { // AND
                if (tmp->type() == ConditionType::conjunction) { // already AND
                    Conjunction* conjuction = static_cast<Conjunction*>(tmp.get());
                    conjuction->add(std::move(step));
                }
                else {
                    auto tmp2 = std::make_unique<Conjunction>();
                    tmp2->add(std::move(tmp));
                    tmp2->add(std::move(step));
                    tmp = std::move(tmp2);
                }
            }
        }
        if (tmp_disjunction.size() > 0) {
            auto res = std::make_unique<Disjunction>(std::move(tmp_disjunction));
            res->add(std::move(tmp));
            return res;
        }
        else return tmp;

    }


    std::unique_ptr<Condition> operator()(ast::Condition const& condition) const {
        if (condition.negation) {
            std::unique_ptr<Condition> cont = boost::apply_visitor(*this, condition.content);
            return std::make_unique<Negation>(std::move(cont));
        }
        else {
            return boost::apply_visitor(*this, condition.content);
        }
    }


    std::unique_ptr<Condition> operator()(ast::Statement const& statement) const {
        if (statement.comparator.type() == typeid(ast::EQ))
            return std::make_unique<Equals>(statement);

        else if (statement.comparator.type() == typeid(ast::NE))
            return std::make_unique<NotEquals>(statement);

        else if (statement.comparator.type() == typeid(ast::LE))
            return std::make_unique<LessOrEquals>(statement);

        else if (statement.comparator.type() == typeid(ast::GE))
            return std::make_unique<GreaterOrEquals>(statement);

        else if (statement.comparator.type() == typeid(ast::GT))
            return std::make_unique<GreaterThan>(statement);

        else // if (statement.comparator.type() == typeid(ast::LT))
            return std::make_unique<LessThan>(statement);
    }
};

#endif // BASE__OP_FILTER_H_
