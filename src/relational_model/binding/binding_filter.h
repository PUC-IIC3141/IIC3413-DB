#ifndef RELATIONAL_MODEL__BINDING_FILTER_H_
#define RELATIONAL_MODEL__BINDING_FILTER_H_

#include "base/binding/binding.h"
#include "base/graph/graph_object.h"
#include "base/ids/graph_id.h"
#include "base/ids/var_id.h"

#include <map>

class BindingFilter : public Binding {

private:
    Binding& binding;
    std::map<std::string, std::shared_ptr<GraphObject>> cache;
    std::map<std::string, GraphId>& var2graph_id;
    std::map<std::string, ObjectType>& element_types;

public:
    BindingFilter(Binding& binding, std::map<std::string, GraphId>& var2graph_id,
                  std::map<std::string, ObjectType>& element_types);
    ~BindingFilter() = default;

    std::string to_string() const;
    std::shared_ptr<GraphObject> operator[](const std::string& var);
    std::shared_ptr<GraphObject> get(const std::string& var, const std::string& key);
};

#endif // RELATIONAL_MODEL__BINDING_FILTER_H_
