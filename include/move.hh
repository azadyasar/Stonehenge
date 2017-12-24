#ifndef MOVE_H
#define MOVE_H

#include <enum_list.hh>
#include <piece.hh>
#include <iostream>

struct Move {

    Move() : piece_(nullptr) {}
    /**
     * @brief Move
     * @param _piece: piece which is put by current player (color of the piece defines the player)
     * @param _position_id: id of location for piece to be allocated
     */
    Move(const Piece &_piece, int _position_id) : position_id_(_position_id)
    {
        this->piece_ = new Piece(_piece.get_color(), _piece.get_value());
    }

    Move(const Move &_move) : position_id_(_move.position_id_) {
        this->piece_ = new Piece(_move.piece_->get_color(), _move.piece_->get_value());
    }

    ~Move() {
        delete this->piece_;
    }

    Move& operator =(const Move& _rhs) {
        if (this->piece_ != nullptr)
            delete this->piece_;
        this->piece_ = new Piece(_rhs.piece_->get_color(), _rhs.piece_->get_value());
        this->position_id_ = _rhs.position_id_;
        return *this;
    }

    Move& operator =(const Move* _rhs) {
        if (this->piece_ != nullptr)
            delete this->piece_;
        this->piece_ = new Piece(_rhs->piece_->get_color(), _rhs->piece_->get_value());
        this->position_id_ = _rhs->position_id_;
        return *this;
    }

    int position_id_;
    Piece *piece_;
    friend std::ostream& operator<<(std::ostream& _os,  Move* _move) {
        _os << (char)_move->piece_->get_color() << "-" << _move->piece_->get_value() << " at "
            << _move->position_id_ << std::endl;
        return _os;
    }
};



#endif // MOVE_H
