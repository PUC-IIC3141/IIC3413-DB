#ifndef BASE__AST_PRINTER_H_
#define BASE__AST_PRINTER_H_

#include "base/parser/grammar/ast.h"

#include <boost/variant.hpp>
#include <boost/optional.hpp>


// Prints the AST generated at parsing in a JSON-like
// format to a given stream.
class ASTPrinter : public boost::static_visitor<void>
{
private:
    std::ostream& out;
    const int_fast32_t base_indent;
    static const int_fast32_t tab_size = 2;

public:
    ASTPrinter(std::ostream& out, int_fast32_t base_indent);
    ASTPrinter(std::ostream& out);
    void indent() const;
    void indent(std::string str) const;
    void indent(std::string str, int_fast32_t extra_indent) const;

    void operator() (ast::Root const&) const;
    void operator() (std::vector<ast::Element> const&) const;
    void operator() (ast::All const&) const;
    void operator() (std::vector<ast::LinearPattern> const&) const;
    void operator() (ast::LinearPattern const&) const;
    void operator() (ast::Element const&) const;
    void operator() (ast::Node) const;
    void operator() (ast::Edge) const;
    void operator() (ast::StepPath) const;
    void operator() (boost::optional<ast::Formula> const&) const;
    void operator() (ast::Condition const&) const;
    void operator() (ast::Statement const&) const;
    void operator() (ast::StepFormula const&) const;
    void operator() (ast::Value const&) const;

    void operator() (std::string const&) const;
    void operator() (VarId    const&) const;
    void operator() (int64_t  const&) const;
    void operator() (float    const&) const;
    void operator() (bool     const&) const;
    void operator() (ast::And const&) const;
    void operator() (ast::Or  const&) const;
    void operator() (ast::EQ  const&) const;
    void operator() (ast::NE  const&) const;
    void operator() (ast::GT  const&) const;
    void operator() (ast::LT  const&) const;
    void operator() (ast::GE  const&) const;
    void operator() (ast::LE  const&) const;
};

#endif  // BASE__AST_PRINTER_H_
