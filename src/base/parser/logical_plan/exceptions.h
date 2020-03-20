#ifndef BASE__PARSING_EXCEPTIONS_H_
#define BASE__PARSING_EXCEPTIONS_H_

#include <exception>
#include <string>

class ParsingException : public std::exception {
	const char * what () const throw () {
    	return "Error parsing query";
    }
};

class NotSupportedException : public std::exception {
	std::string error;

	NotSupportedException(std::string operation) {
		error = "Operation " + operation + " not supported yet.";
	}

	const char * what () const throw () {
    	return error.c_str();
    }
};

#endif // BASE__PARSING_EXCEPTIONS_H_
