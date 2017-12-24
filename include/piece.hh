#ifndef PIECE_H
#define PIECE_H

#include <enum_list.hh>
#include <cstdlib>

class Piece
{
public:
    Piece(Color, size_t);
    Piece(const Piece*);
    Color get_color() const;
    size_t get_value() const;
    bool operator <(const Piece&) const;

private:
    Color color_;
    size_t value_;

};

#endif // PIECE_H
