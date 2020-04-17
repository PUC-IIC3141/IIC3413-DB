#ifndef BASE__PARSING_EXCEPTIONS_H_
#define BASE__PARSING_EXCEPTIONS_H_

#include <exception>
#include <string>

struct QueryException : public std::runtime_error {
	QueryException(std::string msg)
		: std::runtime_error(msg) { }
};

struct QueryParsingException : public QueryException {
	QueryParsingException()
		: QueryException("Error parsing query") { }
};

struct QuerySemanticException : public QueryException {
	QuerySemanticException(std::string msg)
		: QueryException("Bad query semantic: " + msg + ".") { }
};

struct NotSupportedException : public QueryException {
	NotSupportedException(std::string operation)
		: QueryException("Operation " + operation + " not supported yet.") { }
};

struct GraphDoesNotExist : public QueryException {
	GraphDoesNotExist(std::string graph_name)
		: QueryException("Graph '" + graph_name + "' does not exist.") { }
};

#endif // BASE__PARSING_EXCEPTIONS_H_
