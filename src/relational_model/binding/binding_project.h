#ifndef RELATIONAL_MODEL__BINDING_PROJECT_H_
#define RELATIONAL_MODEL__BINDING_PROJECT_H_

#include "base/binding/binding.h"
#include "base/binding/binding_iter.h"

#include <memory>
#include <set>
#include <string>

class BindingProject : public Binding {

private:
    // std::unique_ptr<BindingIter> iter;
    std::set<std::string>& projection_vars;
    std::unique_ptr<Binding> current_binding;

public:
    BindingProject(std::set<std::string>& projection_vars, std::unique_ptr<Binding> current_binding);
    ~BindingProject() = default;

    // void update_binding(); ?
    void print() const;
    std::shared_ptr<GraphObject> operator[](const std::string& var);
    std::shared_ptr<GraphObject> get(const std::string& var, const std::string& key);
};

#endif //RELATIONAL_MODEL__BINDING_PROJECT_H_
