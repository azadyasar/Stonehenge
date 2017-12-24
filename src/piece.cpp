#include "piece.hh"

Piece::Piece(Color _color, size_t _value) : color_(_color), value_(_value) {}

Piece::Piece(const Piece* _p) : color_(_p->get_color()), value_(_p->get_value()) {}

Color Piece::get_color() const { return this->color_; }
size_t Piece::get_value() const { return this->value_; }

bool Piece::operator <(const Piece& _rhs) const {
    return this->value_ < _rhs.get_value();
}
