#include "filter.h"

#include "base/graph/condition/condition.h"
#include "base/binding/binding.h"
#include "relational_model/binding/binding_filter.h"

#include <iostream>

using namespace std;

Filter::Filter(unique_ptr<BindingIter> iter, unique_ptr<Condition> condition,
    map<string, pair<GraphId, ObjectType>> var_info)
    : iter(move(iter)), condition(move(condition)), var_info(move(var_info)) { }


void Filter::begin() {
    iter->begin();
}


std::unique_ptr<Binding> Filter::next() {
    auto next_binding = iter->next();
    while (next_binding != nullptr) {
        auto binding_filter = BindingFilter(*next_binding, var_info);

        if (condition->eval(binding_filter)) {
            return next_binding;
        }
        else {
            next_binding = iter->next();
        }
    }
    return nullptr;
}
