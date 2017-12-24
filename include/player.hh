#ifndef PLAYER_H
#define PLAYER_H

#include <enum_list.hh>
#include <piece.hh>
#include <vector>
#include <megalith.hh>
#include <move.hh>
#include <board.hh>
#include <memory>

class Megalith;
class Board;

class Player
{
public:
    Player(Color, bool);
    Player(const Player&);
    Color get_color() const;
    uint nbr_of_megaliths_won() const;
    uint nbr_of_megaliths_won_minmax() const;
    uint nbr_of_remaining_pieces() const;
    uint total_of_remaining_pieces() const;
    bool is_ai() const;
    std::vector<Megalith> get_megaliths() const;
    void reset_megaliths();
    std::vector<uint> get_remaining_pieces() const;
    void add_megalith(Megalith);
    Move* make_move();
    void decrease_piece(const Move&);
    void reset_move(const Move&);
    uint get_best_value(size_t) const;
    void print_pieces() const;


    const uint MAX_PIECE_VALUE = 6;
private:
    bool is_move_valid(uint) const;


    Color color_;
    bool is_ai_;
    std::vector<uint> remaining_pieces_;
    std::vector<Megalith> megaliths_won_;
    // meg_won_bfore_minmax: If the minimax depth of the game tree reaches the max depth, the utility value of the board
    // is calculated. Utility function needs the number of megaliths won by each player to determine the utility value
    // hence already won megaliths should not have an effect on the utility, we need to keep the initial number of megaliths
    // won. Used with nbr_of_megaliths_won_minimax()
    uint megs_won_bfore_minmax_;

};

typedef std::shared_ptr<Player> PlayerPtr;

#endif // PLAYER_H
