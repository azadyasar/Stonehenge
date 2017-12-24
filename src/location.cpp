#include "location.hh"

#include <megalith.hh>
#include <occupiedlocationexception.hh>
#include <emptylocationexception.hh>
#include <iostream>


Location::Location(uint _id) : id_(_id), is_empty_(true), piece_(nullptr) {}


Location::~Location() { delete this->piece_; }

uint Location::get_id() const { return this->id_; }

// _p1 and _p2 is passed to be used in Megaliths' check_win method
void Location::set_piece(Piece _piece) {
    if (!this->is_empty_)
        throw OccupiedLocationException();

    this->piece_    = new Piece(_piece.get_color(), _piece.get_value());
    this->is_empty_ = false;

    // if any of the three megaliths within the range of this
    // position does not contain any piece. Then
    // assign its first piece for later use e.g., when checking
    // the ownership of the megalith
    for (auto &it : this->ley_lines_)
        if (it.get()->is_empty())
            it.get()->set_first_played(_piece.get_color());

}

void Location::reset()
{
    if (this->is_empty())
        throw EmptyLocationException();

    delete this->piece_; this->piece_ = nullptr;
    this->is_empty_ = true;

}

Piece* Location::get_piece() {
    return this->piece_;
}

bool Location::is_empty() const { return this->is_empty_; }

// Check whether megalith already inserted or not
void Location::add_ley_line(MegalithPtr  _megalith) {
    this->ley_lines_.push_back(_megalith);
}

const std::vector<MegalithPtr>& Location::get_ley_lines() const {
    return this->ley_lines_;
}

bool Location::operator ==(const Location& _rhs) const {
    if (this->id_ == _rhs.get_id())
        return true;
    return false;
}

bool Location::operator ==(const Location* _rhs) const {
    if (this->id_ == _rhs->get_id())
        return true;
    return false;
}

//Location& Location::operator =(const Location& _rhs)  {
//    this->id_ = _rhs.get_id();
//    if (this->piece_ != nullptr)
//        delete this->piece_;
//    this->piece_ = new Piece(_rhs.get_piece()->get_color(), _rhs.get_piece()->get_value());
//    this->ley_lines_ = _rhs.get_ley_lines();
//    return *this;
//}

//Location& Location::operator =(const Location* _rhs)  {
//    this->id_ = _rhs->get_id();
//    if (this->piece_ != nullptr)
//        delete this->piece_;
//    this->piece_ = new Piece(_rhs->get_piece().get_color(), _rhs->get_piece().get_value());
//    this->ley_lines_ = _rhs->get_ley_lines();
//    return *this;
//}


std::ostream& operator<<(std::ostream& os, Location& _rhs) {
    os << "[" << _rhs.get_piece()->get_color() << _rhs.get_id() << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, Location* _rhs) {
    os << "[" << _rhs->get_piece()->get_color() << _rhs->get_id() << "]";
    return os;
}

