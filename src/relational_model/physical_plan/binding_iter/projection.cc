#include "projection.h"

#include "storage/index/object_file/object_file.h"
#include "relational_model/binding/binding_project.h"

#include <iostream>

using namespace std;

Projection::Projection(unique_ptr<BindingIter> iter, set<string> projection_vars)
    : iter(move(iter)), projection_vars(move(projection_vars)), select_all(false) { }

Projection::Projection(unique_ptr<BindingIter> iter)
    : iter(move(iter)), select_all(true) { }

Projection::~Projection() = default;


void Projection::begin() {
    iter->begin();
}


std::unique_ptr<Binding> Projection::next() {
    auto next_binding = iter->next();
    if (next_binding == nullptr) {
        return nullptr;
    }
    else {
        if (select_all) {
            return move(next_binding);
        }
        else {
            return make_unique<BindingProject>(projection_vars, move(next_binding));
        }
    }
}
