#include "ast_printer.h"

/*
 * Each operator() overload should not begin nor end with whitespaces
 * the caller has the responsability to indent before calling and insert
 * new line (if necesary) after the call.
 */

ASTPrinter::ASTPrinter(std::ostream& out)
    : out(out), base_indent(0) {}

ASTPrinter::ASTPrinter(std::ostream& out, int_fast32_t base_indent)
    : out(out), base_indent(base_indent) {}


void ASTPrinter::indent(std::string str) const {
    int_fast32_t spaces = base_indent * tab_size;
    for (int_fast32_t i = 0; i < spaces; i++) {
        out << " ";
    }
    out << str;
}


void ASTPrinter::indent() const {
    int_fast32_t spaces = base_indent * tab_size;
    for (int_fast32_t i = 0; i < spaces; i++) {
        out << " ";
    }
}


void ASTPrinter::indent(std::string str, int_fast32_t extra_indent) const {
    int_fast32_t spaces = (base_indent + extra_indent) * tab_size;
    for (int_fast32_t i = 0; i < spaces; i++) {
        out << " ";
    }
    out << str;
}


void ASTPrinter::operator()(ast::Root const& r) const {
    indent("{\n");
    auto printer = ASTPrinter(out, base_indent+1);
    printer.indent();
    boost::apply_visitor(printer, r.selection);
    out << ",\n";
    printer.indent();
    printer(r.graph_pattern);
    out << ",\n";
    printer.indent();
    printer(r.where);
    out << ",\n";
    printer.indent("\"LIMIT\":");
    if (r.limit) {
        out << r.limit.get();
    } else {
        out << 0;
    }
    indent("\n}\n");
}


void ASTPrinter::operator()(std::vector<ast::Element> const& select) const {
    out << "\"SELECT\": [\n";
    auto printer = ASTPrinter(out, base_indent+1);
    auto it = select.begin();
    while (it != select.end()) {
        printer.indent();
        printer(*it);
        ++it;
        if (it != select.end()) {
            out << ",\n";
        }
        else {
            out << "\n";
        }
    }
    indent("]");
}


void ASTPrinter::operator() (ast::All const&) const {
    out << "\"SELECT\": \"<ALL>\"";
}


void ASTPrinter::operator() (std::vector<ast::LinearPattern> const& graph_pattern) const {
    out << "\"MATCH\": [\n";
    auto printer = ASTPrinter(out, base_indent+1);
    auto it = graph_pattern.begin();
    while (it != graph_pattern.end()) {
        printer.indent();
        printer(*it);
        ++it;
        if (it != graph_pattern.end()) {
            out << ",\n";
        }
        else {
            out << "\n";
        }
    }
    indent("]");
}


void ASTPrinter::operator() (ast::LinearPattern const& linear_pattern) const {
    out << "{\n";
    auto printer = ASTPrinter(out, base_indent+1);
    printer.indent("\"GRAPH\": ");
    out << '"' << linear_pattern.graph_name << '"'<< ",\n";
    printer.indent();
    printer(linear_pattern.root);
    for (auto const& step_path : linear_pattern.path) {
        out << ",\n";
        printer.indent();
        printer(step_path);
    }
    out << "\n";
    indent("}");
}


void ASTPrinter::operator() (ast::StepPath step_path) const {
    (*this)(step_path.edge);
    out << ",\n";
    indent();
    (*this)(step_path.node);
}


void ASTPrinter::operator() (ast::Node node) const {
    out << "\"NODE\": {\n";
    indent("\"VAR\": ", 1);
    out << "\"" << node.var << "\",\n";
    indent("\"LABELS\": [", 1);
    auto label_iter = node.labels.begin();
    while (label_iter != node.labels.end()) {
        out << "\"" << *label_iter << "\"";
        ++label_iter;
        if (label_iter != node.labels.end()) {
            out << ", ";
        }
    }
    out << "],\n";
    if (node.properties.size() > 0) {
        indent("\"PROPERTIES\": [\n", 1);
        auto prop_iter = node.properties.begin();
        while (prop_iter != node.properties.end()) {
            indent("{ \"KEY\": ", 2);
            out << "\"" << (*prop_iter).key << "\"";
            out << ", \"VALUE\": ";
            boost::apply_visitor(*this, (*prop_iter).value);
            ++prop_iter;
            if (prop_iter != node.properties.end()) {
                out << " },\n";
            }
            else {
                out << " }\n";
            }
        }
        indent("]\n", 1);
    }
    else {
        indent("\"PROPERTIES\": []\n", 1);
    }
    indent("}");
}


void ASTPrinter::operator() (ast::Edge edge) const {
    out << "\"EDGE\": {\n";
    indent("\"DIRECTION\": ", 1);
    if (edge.direction == ast::EdgeDirection::right) {
        out << "\"RIGHT\",\n";
    }
    else {
        out << "\"LEFT\",\n";
    }
    indent("\"VAR\": ", 1);
    out << "\"" << edge.var << "\",\n";
    indent("\"LABELS\": [", 1);
    auto label_iter = edge.labels.begin();
    while (label_iter != edge.labels.end()) {
        out << "\"" << *label_iter << "\"";
        ++label_iter;
        if (label_iter != edge.labels.end()) {
            out << ", ";
        }
    }
    out << "],\n";
    if (edge.properties.size() > 0) {
        indent("\"PROPERTIES\": [\n", 1);
        auto prop_iter = edge.properties.begin();
        while (prop_iter != edge.properties.end()) {
            indent("{ \"KEY\": ", 2);
            out << "\"" << (*prop_iter).key << "\"";
            out << ", \"VALUE\": ";
            boost::apply_visitor(*this, (*prop_iter).value);
            ++prop_iter;
            if (prop_iter != edge.properties.end()) {
                out << " },\n";
            }
            else {
                out << " }\n";
            }
        }
        indent("]\n", 1);
    }
    else {
        indent("\"PROPERTIES\": []\n", 1);
    }
    indent("}");
}


void ASTPrinter::operator()(ast::Element const& element) const {
    out << "{\n";
    if (!element.function.empty()) {
        indent("\"FUNCTION\": \"", 1);
        out << element.function << "\",\n";
    }
    indent("\"KEY\": \"", 1);
    out << element.key << "\",\n";
    indent("\"VAR\": \"", 1);
    out << element.variable << "\"\n";
    indent("}");
}


void ASTPrinter::operator()(boost::optional<ast::Formula> const& where) const {
    out << "\"FORMULA\": {\n";
    if (where) {
        ast::Formula formula = static_cast<ast::Formula>(where.get());
        auto printer = ASTPrinter(out, base_indent+1);
        printer.indent();
        printer(formula.root);
        for (const auto& step_formula : formula.path) {
            out << ",\n";
            printer.indent();
            printer(step_formula);
        }
        out << "\n";
    }
    indent("}");
}


void ASTPrinter::operator()(ast::Condition const& condition) const {
    if (condition.negation) {
        out << "\"NOT CONDITION\": {\n";
    }

    else {
        out << "\"CONDITION\": {\n";
    }
    auto printer = ASTPrinter(out, base_indent+1);
    printer.indent();
    boost::apply_visitor(printer, condition.content);
    out << "\n";
    indent("}");
}


void ASTPrinter::operator()(ast::Statement const& statement) const {
    out << "\"LEFT\": ";
    (*this)(statement.lhs);
    out << ",\n";
    indent("\"COMPARATOR\": ");
    boost::apply_visitor(*this, statement.comparator);
    out << ",\n";
    indent("\"RIGHT\": ");
    boost::apply_visitor(*this, statement.rhs);
}


void ASTPrinter::operator()(ast::StepFormula const& step_formula) const {
    out << "\"CONNECTOR\": ";
    boost::apply_visitor(*this, step_formula.op);
    out << ",\n";
    indent();
    (*this)(step_formula.condition);
}


void ASTPrinter::operator()(ast::Value const& v) const {
    boost::apply_visitor(*this, v);
}


void ASTPrinter::operator()(std::string const& text) const {
    out << "\"" << text << "\"";
}


void ASTPrinter::operator() (VarId const& var_id) const {out << "VarId(" << var_id.id << ")"; }
void ASTPrinter::operator() (int const& n)        const {out << "(int)" << n; }
void ASTPrinter::operator() (float const& n)      const {out << "(float)" << n; }
void ASTPrinter::operator() (ast::And const&)     const {out << "\"AND\""; }
void ASTPrinter::operator() (ast::Or const&)      const {out << "\"OR\""; }
void ASTPrinter::operator() (ast::EQ const&)      const {out << "\"==\""; }
void ASTPrinter::operator() (ast::NE const&)      const {out << "\"!=\""; }
void ASTPrinter::operator() (ast::GT const&)      const {out << "\">\""; }
void ASTPrinter::operator() (ast::LT const&)      const {out << "\"<\""; }
void ASTPrinter::operator() (ast::GE const&)      const {out << "\">=\""; }
void ASTPrinter::operator() (ast::LE const&)      const {out << "\"<=\""; }


void ASTPrinter::operator() (bool const& b) const {
    if (b)
        out << "TRUE";
    else
        out << "FALSE";
}