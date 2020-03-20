#include "binding_project.h"

#include "base/binding/binding.h"
#include "base/graph/value/value.h"

#include <iostream>

using namespace std;

BindingProject::BindingProject(set<string>& projection_vars, unique_ptr<Binding> current_binding)
    : projection_vars(projection_vars), current_binding(move(current_binding)) { }


void BindingProject::print() const {
    cout << "{";
    auto it = projection_vars.begin();
    while (it != projection_vars.end()) {
        auto& var = *it;
        cout << var << ":";
        cout << (*current_binding)[var]->to_string();
        ++it;
        if (it != projection_vars.end())
            cout << ",";
    }
    cout << "}\n";
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
