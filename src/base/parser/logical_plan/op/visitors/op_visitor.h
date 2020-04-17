#ifndef BASE__OP_VISITOR_H_
#define BASE__OP_VISITOR_H_

#include <string>

class OpSelect;
class OpMatch;
class OpFilter;
class OpLabel;
class OpProperty;
class OpConnection;
class OpLonelyNode;

class OpVisitor {
public:
    virtual ~OpVisitor() { };
    virtual void visit(OpSelect&) = 0;
    virtual void visit(OpMatch&) = 0;
    virtual void visit(OpFilter&) = 0;
    virtual void visit(OpLabel&) = 0;
    virtual void visit(OpProperty&) = 0;
    virtual void visit(OpConnection&) = 0;
    virtual void visit(OpLonelyNode&) = 0;

};

#endif // BASE__OP_VISITOR_H_
