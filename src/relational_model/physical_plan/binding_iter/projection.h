#ifndef RELATIONAL_MODEL__PROJECTION_H_
#define RELATIONAL_MODEL__PROJECTION_H_

#include "base/binding/binding_iter.h"
#include "relational_model/binding/binding_id_iter.h"

#include <set>
#include <memory>

class Projection : public BindingIter {

private:
    uint_fast32_t limit;
    uint_fast32_t count;
    const bool select_all;
    std::unique_ptr<BindingIter> iter;
    std::set<std::string> projection_vars;

public:
    Projection(std::unique_ptr<BindingIter> iter, uint_fast32_t limit); // constructor for select *
    Projection(std::unique_ptr<BindingIter> iter, std::set<std::string> projection_vars, uint_fast32_t limit);
    ~Projection();

    void begin();
    std::unique_ptr<Binding> next();
};

#endif //RELATIONAL_MODEL__PROJECTION_H_
