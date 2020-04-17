#ifndef RELATIONAL_MODEL__BINDING_ID_ITER_H_
#define RELATIONAL_MODEL__BINDING_ID_ITER_H_

class BindingId;

// Abstract class
class BindingIdIter {
public:
    virtual ~BindingIdIter() { };
    virtual void begin(BindingId& input) = 0;
    virtual void reset(BindingId& input) = 0;
    virtual BindingId* next() = 0; // next returning nullptr means there are not more bindings
};

#endif //RELATIONAL_MODEL__BINDING_ID_ITER_H_
