#include "binding_project.h"

#include "base/binding/binding.h"
#include "base/graph/value/value.h"

#include <iostream>

using namespace std;

BindingProject::BindingProject(set<string>& projection_vars, unique_ptr<Binding> current_binding)
    : projection_vars(projection_vars), current_binding(move(current_binding)) { }


std::string BindingProject::to_string() const {
    std::string result;
    result += '{';
    auto it = projection_vars.begin();

iter_begin:
    auto& var = *it;
    result += var;
    result += ':';
    result += (*current_binding)[var]->to_string();
    ++it;
    if (it != projection_vars.end()) {
        result += ',';
        goto iter_begin;
    }

    result +=  "}\n";
    return result;
}


shared_ptr<GraphObject> BindingProject::operator[](const string& var) {
    if (projection_vars.find(var) != projection_vars.end()) {
        return (*current_binding)[var];
    }
    else return nullptr;
}


shared_ptr<GraphObject> BindingProject::get(const string& var, const string& key) {
    return (*this)[var + "." + key];
}
