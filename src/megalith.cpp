#include "megalith.hh"

#include <queue>
#include <iostream>
#include <cassert>

Megalith::Megalith(uint _id) : id_(_id), won_(false), is_empty_(true), win_value_(0),
                                win_status_(NOT_WON) {}

Megalith::Megalith(uint _id, bool _won, bool _isempty, Color _who_won,
         Color _first_player, size_t _win_value) : id_(_id), won_(_won),
    is_empty_(_isempty), who_won_(_who_won), player_first_played_(_first_player) , win_value_(_win_value),
    win_status_(NOT_WON) {}

uint Megalith::get_id() const { return this->id_; }

void Megalith::set_first_played(Color _color) {
    if (!this->is_empty_) {
        std::cout << "[ERROR]: This megalith is already played first..\n";
    }
    this->is_empty_ = false;
    this->player_first_played_ = _color;
}

bool Megalith::is_empty() const { return this->is_empty_; }

size_t Megalith::get_win_value() const { return this->win_value_; }

bool Megalith::won() const { return this->won_; }

Color Megalith::who_played_first() const { return this->player_first_played_; }

std::vector<LocationPtr> Megalith::get_locations() const { return this->locations_; }

Color Megalith::who_won() const { return this->who_won_; }

MEGALITH_STATUS Megalith::get_win_status() const
{
    return win_status_;
}

void Megalith::add_location(LocationPtr _loc) {
    this->locations_.push_back(_loc);
}

/**
 * @brief Megalith::check_win Checks whether the current status of the ley-line is
 *         a win or not
 * @param _player1_pieces list of remaining pieces in the hand of corresponding player
 * @param _player2_pieces list of remaining pieces in the hand of corresponding player
 * @variable empty_cells contains the number of empty cells that the ley-line being
 *           checked has. Later will be used when acquiring the best possible move
 *           for either of the players.
 */
void Megalith::check_win(PlayerPtr _player1, const PlayerPtr _player2) {

    // Megalith is already won by either players.
    if (this->won_) {
        if (this->who_won_ == _player1.get()->get_color())
            _player1.get()->add_megalith(*this);
        else
            _player2.get()->add_megalith(*this);
        update_win_status();
        return;
    }

    if (this->is_empty())
        return;

    size_t empty_cells   = 0;
    size_t player1_value = 0;
    size_t player2_value = 0;
    Color player1_color, player2_color;

    player1_color = _player1.get()->get_color();
    player2_color = _player2.get()->get_color();

    // ??? Create a Player class that holds Color and the remaining pieces ???
//    if (_player1_pieces.size() != 0) {
//        player1_color = _player1_pieces[0].get_color();
//        if (player1_color == BLACK)
//            player2_color = WHITE;
//        else
//            player2_color = BLACK;
//    } else if (_player2_pieces.size() != 0) {
//        player2_color = _player2_pieces[0].get_color();
//        if (player2_color == BLACK)
//            player1_color = WHITE;
//        else
//            player1_color = BLACK;
//    } else {
//        std::cout << "Neither players have pieces!!\n";
//        return;
//    }

    // Calculating players' total values on the ley-line
    for (auto& it : this->locations_)
    {
        if (!it.get()->is_empty()) {
            const Piece* tmp_piece = it.get()->get_piece();
            if (tmp_piece->get_color() == player1_color)
                player1_value += tmp_piece->get_value();
            else
                player2_value += tmp_piece->get_value();
        }
        else
            empty_cells += 1;
    }

    // the ley-line has been completed
    // player who has the greater pieces wins
    // in case of equilibrum player who has placed
    // the first piece wins
    if (empty_cells == 0) {
        if (player1_value > player2_value) {
            this->who_won_   = player1_color;
            this->win_value_ = player1_value;
            _player1.get()->add_megalith(*this);
        }
        else if (player2_value > player1_value) {
            this->who_won_   = player2_color;
            this->win_value_ = player2_value;
            _player2.get()->add_megalith(*this);
        }
        else {
            this->who_won_   = this->player_first_played_;
            if (this->player_first_played_ == player1_color)
                _player1.get()->add_megalith(*this);
            else
                _player2.get()->add_megalith(*this);
            this->win_value_ = player1_value;
        }
        this->won_ = true;
        update_win_status();
        return;
    }
    // In case there are empty cells:
    // Check whether player that is losing can
    // alter the dispute on the ley-line or not
    else {
        if (player1_value > player2_value) {
            size_t best_value = _player2.get()->get_best_value(empty_cells);
//            size_t best_value = this->get_best_value(_player2_pieces, empty_cells);
            if (player2_value + best_value > player1_value)
                return;         // player 2 still has a chance
            else {
                this->who_won_   = player1_color;
                this->win_value_ = player1_value;
                this->won_       = true;
                _player1.get()->add_megalith(*this);
                update_win_status();
                return;
            }
        }
        else if (player2_value > player1_value) {
                size_t best_value = _player1.get()->get_best_value(empty_cells);
//                size_t best_value = this->get_best_value(_player1_pieces, empty_cells);
                if (player1_value + best_value > player2_value)
                    return;     // player 1 still has a chance
                else {
                    this->who_won_   = player2_color;
                    this->win_value_ = player2_value;
                    this->won_       = true;
                    _player2.get()->add_megalith(*this);
                    update_win_status();
                    return;
                }
        }
        else
            return; // either player has the chance
    }
}
void Megalith::check_win(Player *_player1, Player *_player2)
{

    // Megalith is already won by either players.
//    if (this->won_) {
//        if (this->who_won_ == _player1->get_color())
//            _player1->add_megalith(*this);
//        else
//            _player2->add_megalith(*this);
//        return;
//    }
    if (this->is_empty())
        return;

    size_t empty_cells   = 0;
    size_t player1_value = 0;
    size_t player2_value = 0;
    Color player1_color, player2_color;

    player1_color = _player1->get_color();
    player2_color = _player2->get_color();


    // Calculating players' total values on the ley-line
    for (auto& it : this->locations_)
    {
        if (!it.get()->is_empty()) {
            const Piece* tmp_piece = it.get()->get_piece();
            if (tmp_piece->get_color() == player1_color)
                player1_value += tmp_piece->get_value();
            else
                player2_value += tmp_piece->get_value();
        }
        else
            empty_cells += 1;
    }

    // the ley-line has been completed
    // player who has the greated pieces wins
    // in case of equilibrum player who has placed
    // the first piece wins
    if (empty_cells == 0) {
        if (player1_value > player2_value) {
            this->who_won_   = player1_color;
            this->win_value_ = player1_value;
            _player1->add_megalith(*this);
        }
        else if (player2_value > player1_value) {
            this->who_won_   = player2_color;
            this->win_value_ = player2_value;
            _player2->add_megalith(*this);
        }
        else {
            this->who_won_   = this->player_first_played_;
            if (this->player_first_played_ == player1_color)
                _player1->add_megalith(*this);
            else
                _player2->add_megalith(*this);
            this->win_value_ = player1_value;
        }
        this->won_ = true;
        return;
    }
    // In case there are empty cells:
    // Check whether player that is losing can
    // alter the dispute on the ley-line or not
    else {
        if (player1_value > player2_value) {
            size_t best_value = _player2->get_best_value(empty_cells);
//            size_t best_value = this->get_best_value(_player2_pieces, empty_cells);
            if (player2_value + best_value > player1_value)
                return;         // player 2 still has a chance
            else {
                this->who_won_   = player1_color;
                this->win_value_ = player1_value;
                this->won_       = true;
                _player1->add_megalith(*this);
                return;
            }
        }
        else if (player2_value > player1_value) {
                size_t best_value = _player1->get_best_value(empty_cells);
//                size_t best_value = this->get_best_value(_player1_pieces, empty_cells);
                if (player1_value + best_value > player2_value)
                    return;     // player 1 still has a chance
                else {
                    this->who_won_   = player2_color;
                    this->win_value_ = player2_value;
                    this->won_       = true;
                    _player2->add_megalith(*this);
                    return;
                }
        }
        else
            return; // either player has the chance
    }
}

void Megalith::refresh(bool _won, bool _is_empty, Color _who_won, Color _first_player, size_t _win_value)
{
    this->won_ = _won;
    this->is_empty_ = _is_empty;
    this->who_won_  = _who_won;
    this->player_first_played_ = _first_player;
    this->win_value_ = _win_value;
}





/**
 * @brief Megalith::get_best_value
 * @param _pieces list of pieces that corresponding player has
 * @param _size number of empty cells on the ley-line
 * @variable total_value: sum of the best (_size) pieces for the corresponding player
 * @return total_value
 */
unsigned int Megalith::get_best_value(const std::vector<Piece> &_pieces, size_t _size) const {
    unsigned int total_value = 0;
    std::priority_queue<Piece> p_queue;

    assert(_pieces.size() >= _size);

    for (auto &it : _pieces)
        p_queue.push(it);

    for (size_t i  = 0; i < _size; ++i) {
        total_value += p_queue.top().get_value();
        p_queue.pop();
    }

    return total_value;
}

void Megalith::update_win_status()
{
    switch (win_status_) {
    case NOT_WON:
        win_status_ = JUST_WON;
        break;
    case JUST_WON:
        win_status_ = ALREADY_WON;
        break;
    case ALREADY_WON:
        break;
    default:
        break;
    }
}


bool Megalith::operator ==(const Megalith& _rhs) const {
    if (this->id_ == _rhs.get_id())
        return true;
    return false;
}

bool Megalith::operator ==(const Megalith* _rhs) const {
    if (this->id_ == _rhs->get_id())
        return true;
    return false;
}

Megalith& Megalith::operator =(const Megalith& _rhs) {
    this->id_                  = _rhs.get_id();
    this->won_                 = _rhs.won();
    this->is_empty_            = _rhs.is_empty();
    this->who_won_             = _rhs.who_won();
    this->win_value_           = _rhs.get_win_value();
    this->player_first_played_ = _rhs.who_played_first();
    this->locations_           = _rhs.get_locations();

    return *this;
}


Megalith& Megalith::operator =(const Megalith* _rhs) {
    this->id_                  = _rhs->get_id();
    this->won_                 = _rhs->won();
    this->is_empty_            = _rhs->is_empty();
    this->who_won_             = _rhs->who_won();
    this->win_value_           = _rhs->get_win_value();
    this->player_first_played_ = _rhs->who_played_first();
    this->locations_           = _rhs->get_locations();

    return *this;
}
