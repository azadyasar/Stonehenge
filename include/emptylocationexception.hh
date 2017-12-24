#ifndef EMPTYLOCATIONEXCEPTION_H
#define EMPTYLOCATIONEXCEPTION_H

#include <exception>

class EmptyLocationException : public std::exception {
    virtual const char* what() const throw() {
        return "Location is empty, no owner!!";
    }
};

#endif // EMPTYLOCATIONEXCEPTION_H
