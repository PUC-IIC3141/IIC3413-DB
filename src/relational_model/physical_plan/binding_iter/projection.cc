#include "projection.h"

#include "storage/index/object_file/object_file.h"
#include "relational_model/binding/binding_project.h"

#include <iostream>

using namespace std;

Projection::Projection(unique_ptr<BindingIter> iter, set<string> projection_vars, uint_fast32_t limit)
    : limit(limit), select_all(false), iter(move(iter)), projection_vars(move(projection_vars)) { }

Projection::Projection(unique_ptr<BindingIter> iter, uint_fast32_t limit)
    : limit(limit), select_all(true), iter(move(iter)) { }

Projection::~Projection() = default;


void Projection::begin() {
    count = 0;
    iter->begin();
}


std::unique_ptr<Binding> Projection::next() {
    if (limit != 0 && count >= limit) {
        return nullptr;
    }
    auto next_binding = iter->next();
    if (next_binding == nullptr) {
        return nullptr;
    }
    else {
        count++;
        if (select_all) {
            return next_binding;
        }
        else {
            return make_unique<BindingProject>(projection_vars, move(next_binding));
        }
    }
}
