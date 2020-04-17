#ifndef BASE__BINDING_H_
#define BASE__BINDING_H_

#include <memory>
#include <string>

#include "base/graph/graph_object.h"

// Abstract class
class Binding {
public:
    virtual ~Binding() { };

    virtual std::string to_string() const = 0;
    virtual std::shared_ptr<GraphObject> operator[](const std::string& var) = 0;
    virtual std::shared_ptr<GraphObject> get(const std::string& var, const std::string& key) = 0;
};


#endif // BASE__BINDING_H_
