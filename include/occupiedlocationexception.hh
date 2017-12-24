#ifndef OCCUPIEDLOCATIONEXCEPTION_H
#define OCCUPIEDLOCATIONEXCEPTION_H


#include <exception>

class OccupiedLocationException : public std::exception {

public:
    virtual const char* what() const throw() {
        return "Location is occupied!!";
    }

};

#endif // OCCUPIEDLOCATIONEXCEPTION_H
