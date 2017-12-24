#ifndef GAME_H
#define GAME_H

#include <board.hh>
#include <unordered_map>
#include <player.hh>
#include <enum_list.hh>

class Game
{
public:
    Game(Color);
    Game(Color, bool, int);
    ~Game();
    void start_game();

private:
    void init_game();
    bool is_game_finished();
    void update_current_player();
    void propogate();
    void update();
    void update_depth();
    Move* make_move_ai();
    Move minimax();
    int min_value(Board&);
    int max_value(Board&);
    Move minimax_ab();
    int min_value_ab(Board&, int, int);
    int max_value_ab(Board&, int, int);
    int utility();
    std::vector<Move> get_possible_moves(const Board&, const Player&) const;


    Board *board_;
    const Color first_player_;
    PlayerPtr current_player_;
    PlayerPtr user_player_;
    PlayerPtr ai_player_;
    PlayerPtr winner_;
    Player *ai_clone_, *user_clone_;
    uint minimax_depth_; // holds the current depth of minimax tree when searching for a move
    uint max_depth_;     // stores the max depth level when going down on a game tree condition(minimax_depth < max_depth)
    int difficulty_;
    uint game_counter_;
    bool prune_;
    uint nbr_of_nodes_;
    const uint ENOUGH_SCORE_ = 8;
};

#endif // GAME_H
