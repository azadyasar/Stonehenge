#include "player.hh"
#include <move.hh>
#include <cstdio>
#include <queue>
#include <cassert>

Player::Player(Color _color, bool _is_ai) : color_(_color), is_ai_(_is_ai)
{
    for (uint i = 1; i <= MAX_PIECE_VALUE; ++i) {
        if (i < 4)
            this->remaining_pieces_.push_back(2);
        else
            this->remaining_pieces_.push_back(1);
    }
}

Player::Player(const Player & _p) {
    this->color_                 = _p.get_color();
    this->is_ai_                 = _p.is_ai();
    this->remaining_pieces_      = _p.get_remaining_pieces();
    this->megaliths_won_         = _p.get_megaliths();
    this->megs_won_bfore_minmax_ = _p.nbr_of_megaliths_won();
}

Color Player::get_color() const { return this->color_; }

uint Player::nbr_of_megaliths_won() const { return this->megaliths_won_.size(); }

uint Player::nbr_of_megaliths_won_minmax() const
{
    return this->megaliths_won_.size() - this->megs_won_bfore_minmax_;
}

uint Player::nbr_of_remaining_pieces() const { return this->remaining_pieces_.size(); }

uint Player::total_of_remaining_pieces() const {
    uint total = 0;
    for (uint i = 1; i <= MAX_PIECE_VALUE; ++i)
        total += this->remaining_pieces_[i-1] * i;
    return total;
}

bool Player::is_ai() const { return this->is_ai_; }

void Player::add_megalith(Megalith _megalith) {
    this->megaliths_won_.push_back(_megalith);
}

Move* Player::make_move() {
//    if (this->is_ai_)
//        make_move_ai();
    printf("%s's Move (Location Piece): ", this->color_ == WHITE ? "White" : "Black");
    int location, piece;
    bool is_move_valid = false;
    scanf("%d %d", &location, &piece);
    is_move_valid = this->is_move_valid(piece);
    while (!is_move_valid) {
        printf("You have no such piece!!\n");
        printf("Piece: ");
        scanf("%d", &piece);
        is_move_valid = this->is_move_valid(piece);
    }
    this->remaining_pieces_[piece-1] -= 1;
    return new Move(Piece(this->color_, piece), location);
}

void Player::decrease_piece(const Move & _move)
{
    if (this->remaining_pieces_[_move.piece_->get_value()-1] <= 0)
        printf("Piece is already 0\n\n");

    this->remaining_pieces_[_move.piece_->get_value()-1] -= 1;

}


// When move is not valid which is returned by Board::move, the removed piece
// will be put back
void Player::reset_move(const Move& _wrong_move) {
    this->remaining_pieces_[_wrong_move.piece_->get_value()-1] += 1;
}

bool Player::is_move_valid(uint _piece_value) const {
    if (_piece_value <= 0 || _piece_value > Player::MAX_PIECE_VALUE)
        return false;
    if (this->remaining_pieces_[_piece_value-1] > 0)
        return true;
    return false;
}

uint Player::get_best_value(size_t _size) const {
    unsigned int total_value = 0;
    std::priority_queue<uint> p_queue;

    assert(this->remaining_pieces_.size() >= _size);


    for (auto &it : this->remaining_pieces_)
        p_queue.push(it);

    for (size_t i = 1; i <= MAX_PIECE_VALUE; ++i)
        for (size_t j = 0; j < this->remaining_pieces_[i-1]; ++j)
            p_queue.push(i);

    for (size_t i  = 0; i < _size; ++i) {
        total_value += p_queue.top();
        p_queue.pop();
    }

    return total_value;
}

std::vector<Megalith> Player::get_megaliths() const {
    return this->megaliths_won_;
}

void Player::reset_megaliths()
{
    this->megaliths_won_.clear();
}
std::vector<uint> Player::get_remaining_pieces() const {
    return this->remaining_pieces_;
}

void Player::print_pieces() const {
    for (size_t i = 1; i <= MAX_PIECE_VALUE; ++i)
        for (size_t j = 0; j < this->remaining_pieces_[i-1]; ++j)
            printf("[%lu] ", i);
//    printf("%-30s", "");
}




//public:
//    Player(Color);
//    size_t total_of_remaining_pieces() const;
//    size_t nbr_of_remaining_pieces() const;
//    size_t nbr_of_megaliths_won() const;
//    const Move& make_move();


//private:
//    Color color_;
//    std::vector<Piece> remaining_pieces_;
//    std::vector<Megalith> megaliths_won_;
