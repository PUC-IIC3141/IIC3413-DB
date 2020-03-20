#ifndef RELATIONAL_MODEL__BULK_IMPORT_VALUE_VISITOR_H_
#define RELATIONAL_MODEL__BULK_IMPORT_VALUE_VISITOR_H_

#include "base/graph/value/value_int.h"
#include "base/graph/value/value_float.h"
#include "base/graph/value/value_bool.h"
#include "base/graph/value/value_string.h"
#include "relational_model/import/bulk_import_ast.h"

#include <boost/variant.hpp>

class BulkImportValueVisitor : public boost::static_visitor<std::unique_ptr<Value>> {

public:
    std::unique_ptr<Value> operator() (bulk_import_ast::Value value) const {
        return boost::apply_visitor(*this, value);
    }

    std::unique_ptr<Value> operator() (int const& n) const {
        return std::make_unique<ValueInt>(n);
    }

    std::unique_ptr<Value> operator() (float const& f) const {
        return std::make_unique<ValueFloat>(f);
    }

    std::unique_ptr<Value> operator() (bool const& b) const {
        return std::make_unique<ValueBool>(b);
    }

    std::unique_ptr<Value> operator() (std::string const& text) const {
        return std::make_unique<ValueString>(text);
    }
};
#endif // RELATIONAL_MODEL__BULK_IMPORT_VALUE_VISITOR_H_
