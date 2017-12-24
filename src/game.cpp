#include "game.hh"
#include <cstdio>
#include <cstring>
#include <thread>

Game::Game(Color _first_player_color)
    : first_player_(_first_player_color), max_depth_(3),
      prune_(true)
{
    this->init_game();
}

Game::Game(Color _first_player_color, bool _prune, int _diff)
    : first_player_(_first_player_color), difficulty_(_diff),
      prune_(_prune)
{
    this->init_game();
}

Game::~Game() {
    delete this->board_;
    this->user_player_.reset();
    this->ai_player_.reset();
}

void Game::init_game() {
    this->board_ = new Board();
    printf("White starts the game. Black or White?(b/w): ");
    char user_color[16];
    scanf("%s", user_color);
    char yesorno[8];
    printf("Vs. AI? [y/n]: ");
    scanf("%s", yesorno);
    bool vsai = false;
    if (strcmp(yesorno, "y") == 0 || strcmp(yesorno, "Y") == 0)
        vsai = true;
    if (vsai) {
        switch (difficulty_) {
        case 1:
            this->max_depth_ = 2;
            printf("Minimax depth = %u\n", this->max_depth_);
            break;
        case 2:
            this->max_depth_ = 2;
            printf("Minimax depth = %u\n", this->max_depth_);
            break;
        case 3:
            this->prune_ = true;
            this->max_depth_ = 2;
            printf("Minimax depth = %u\n", this->max_depth_);
            break;
        default:
            printf("Difficulty level (%d) undefined\n", difficulty_);
            break;
        }
    }

    printf("Playing vs AI: %s, Prune: %s\n", vsai == true ? "Yes" : "No",
                                              this->prune_ ? "Yes" : "No");
    if (strcmp(user_color, "w") == 0 || strcmp(user_color, "W") == 0) {
        this->user_player_ = std::make_shared<Player>(Player(WHITE, false));// new Player(WHITE, false);
        this->ai_player_   = std::make_shared<Player>(Player(BLACK, vsai)); //new Player(BLACK, vsai);
        this->current_player_ =  this->user_player_;
    }
    else {
        this->user_player_ = std::make_shared<Player>(Player(BLACK, false)); //new Player(BLACK, false);
        this->ai_player_   = std::make_shared<Player>(Player(WHITE, vsai)); //new Player(WHITE, vsai);
        this->current_player_ = this->ai_player_;
    }

    printf("Your color: %c, Opponent's color: %c\n", this->user_player_.get()->get_color(), this->ai_player_.get()->get_color());

}

void Game::start_game() {
    this->game_counter_ = 0;
    this->update();

    while (!this->is_game_finished()) {
        // 1 MOVE
        // 2 UPDATE
        // 3 MOVE
        // 4 UPDATE
        this->propogate();
        this->update();
        if (this->is_game_finished())
            break;
        this->propogate();
        this->update();
        this->game_counter_ += 1;
        this->update_depth();
    }

    printf("\n\n!!!%s won the game!!!\n\n", this->winner_.get()->get_color() == WHITE ? "White" : "Black");

}

void Game::propogate() {
    Move* move;

    if (this->current_player_.get()->is_ai()) {
        move = this->make_move_ai();
        std::cout << move << std::endl;
        this->board_->move(*move);
        this->ai_player_.get()->decrease_piece(*move);
        delete move;
    } else {
        bool valid_move = false;
        while (!valid_move) {
            move = this->current_player_.get()->make_move();
            valid_move = this->board_->move(*move);
            if (!valid_move)
                this->current_player_.get()->reset_move(*move);
            delete move;
        }
    }
    this->update_current_player();
}

void Game::update() {
    this->user_player_.get()->reset_megaliths();
    this->ai_player_.get()->reset_megaliths();
    this->board_->update_megaliths(this->user_player_, this->ai_player_);
    this->board_->print_board();
    printf("User:\n\tPieces (%c): ", this->user_player_.get()->get_color());
    this->user_player_.get()->print_pieces();
    printf("\n\tMegaliths won: %5u", this->user_player_.get()->nbr_of_megaliths_won());
    printf("\nAI:\n\tPieces (%c): ", this->ai_player_.get()->get_color());
    this->ai_player_.get()->print_pieces();
    printf("\n\tMegaliths won: %5u", this->ai_player_.get()->nbr_of_megaliths_won());
    printf("\n");
}

void Game::update_depth()
{
    switch (this->difficulty_) {
    case 1:
        // continues as depth 2
        break;
    case 2:
        if (this->game_counter_ >= 4)
            this->max_depth_ += 2;
        break;
    case 3:
        if (this->game_counter_ == 2)
            this->max_depth_ += 1;
        else if (this->game_counter_ == 3)
            this->max_depth_ += 1;
        else if (this->game_counter_ == 4)
            this->max_depth_ += 1;
        else if (this->game_counter_ >= 5)
            this->max_depth_ += 10;

        break;
    default:
        printf("Difficulty level (%d) not define\n", this->difficulty_);
        break;
    }


}

Move *Game::make_move_ai()
{
    printf("Ai is thinkng with depth %u..\n", this->max_depth_);
    if (this->current_player_.get() != this->ai_player_.get())
        printf("Current player is not AI?\n");

    this->ai_clone_   = new Player(*(this->ai_player_.get()));
    this->user_clone_ = new Player(*(this->user_player_.get()));
    this->nbr_of_nodes_ = 0;
    Move* result;
    if (this->prune_) {
        result =  new Move(this->minimax_ab());
        printf("\nTotal number of boards checked: %u, pruning: True\n", this->nbr_of_nodes_);
    }
    else {
        result = new Move(this->minimax());
        printf("\nTotal number of boards checked: %u, pruning: False\n", this->nbr_of_nodes_);
    }

    delete this->ai_clone_;
    delete this->user_clone_;

    return result;
}

Move Game::minimax()
{
    Board test_board(*(this->board_));
    this->minimax_depth_ = 0;
    int max_score, tmp_score;
    Move max_move;

    std::vector<Move> actions = this->get_possible_moves(test_board, *(this->ai_clone_));
    test_board.move(actions[0]);
    this->ai_clone_->decrease_piece(actions[0]);
    this->minimax_depth_ += 1;
    this->ai_clone_->reset_megaliths(); this->user_clone_->reset_megaliths();
    test_board.update_megaliths(this->user_clone_, this->ai_clone_);
    max_score = this->min_value(test_board);
    this->minimax_depth_ -= 1;
    max_move = actions[0];
    test_board.undo(max_move);
    this->ai_clone_->reset_move(max_move);
    this->ai_clone_->reset_megaliths(); this->user_clone_->reset_megaliths();
    test_board.reset_megaliths();
    actions.erase(actions.begin());

    uint counter = 1;
    for (auto &it : actions) {
        printf("%u ", counter++);
        test_board.move(it);
        this->ai_clone_->decrease_piece(it);
        this->minimax_depth_ += 1;
        this->ai_clone_->reset_megaliths(); this->user_clone_->reset_megaliths();
        test_board.update_megaliths(this->user_clone_, this->ai_clone_);
        tmp_score = this->min_value(test_board);
//        printf("\nScore of the board is %d\n", tmp_score);
//        test_board.print_board();
        test_board.undo(it);
        this->ai_clone_->reset_move(it);
        this->ai_clone_->reset_megaliths(); this->user_clone_->reset_megaliths();
        test_board.reset_megaliths();
        this->minimax_depth_ -= 1;
        if (tmp_score > max_score) {
            max_score = tmp_score;
            max_move  = it;
        }
    }

    return max_move;
}

int Game::min_value(Board& _board) {
    if (this->minimax_depth_ == max_depth_) {
        this->nbr_of_nodes_ += 1;
        int utility = this->utility();
//        printf("Utility of the board: %d\n", utility);
//        _board.print_board();
        return utility;
    }

    int min_val = INT32_MAX;
    int tmp_val;
    std::vector<Move> actions = this->get_possible_moves(_board, *(this->user_clone_));

    if (actions.size() == 0) {
        int util = this->utility();
//        printf("Utility of the board: %d\n", util);
//        _board.print_board();
        return util;
    }

    for (auto &it: actions) {
        _board.move(it);
        this->user_clone_->decrease_piece(it);
        this->minimax_depth_ += 1;
        this->ai_clone_->reset_megaliths(); this->user_clone_->reset_megaliths();
        _board.update_megaliths(this->user_clone_, this->ai_clone_);
        tmp_val = this->max_value(_board);
        if (tmp_val < min_val)
            min_val = tmp_val;
        _board.undo(it);
        this->user_clone_->reset_move(it);
        this->ai_clone_->reset_megaliths(); this->user_clone_->reset_megaliths();
        _board.reset_megaliths();
        this->minimax_depth_ -= 1;
    }

    this->nbr_of_nodes_ += 1;
    return min_val;
}

int Game::max_value(Board& _board) {
    if (this->minimax_depth_ == max_depth_) {
        this->nbr_of_nodes_ += 1;
        int utility = this->utility();
//        printf("Utility of the board: %d\n", utility);
//        _board.print_board();
        return utility;
    }

    int max_val = INT32_MIN;
    int tmp_val;
    std::vector<Move> actions = this->get_possible_moves(_board, *(this->ai_clone_));

    if (actions.size() == 0) {
        int util = this->utility();
//        printf("Utility of the board: %d\n", util);
//        _board.print_board();
        return util;
    }

    for (auto& it: actions) {
        _board.move(it);
        this->ai_clone_->decrease_piece(it);
        this->minimax_depth_ += 1;
        this->ai_clone_->reset_megaliths(); this->user_clone_->reset_megaliths();
        _board.update_megaliths(this->ai_clone_, this->user_clone_);
        tmp_val = this->min_value(_board);
        if (tmp_val > max_val)
            max_val = tmp_val;
        _board.undo(it);
        this->ai_clone_->reset_move(it);
        this->ai_clone_->reset_megaliths(); this->user_clone_->reset_megaliths();
        _board.reset_megaliths();
        this->minimax_depth_ -= 1;
    }
    this->nbr_of_nodes_ += 1;
    return max_val;
}

/**
 * @brief Game::minimax_ab
 * @variable max_move: best move so fat
 * @variable alpha: the best score maximizer can get by choosing corresponding path
 * @variable beta: the best score minimizer can get by choosing corresponding path
 * @variable minimax_depth: keeps track of the current depth of the game tree,
 *                          when max_depth is reached, a utility value is returned
 * @return the best move with respect to AI
 */
Move Game::minimax_ab()
{
    Board test_board(*(this->board_));
    this->minimax_depth_ = 0;
    int max_value, tmp_value;
    Move max_move;
    int alpha = INT32_MIN;
    int beta  = INT32_MAX;

    std::vector<Move> actions = this->get_possible_moves(test_board, *(this->ai_clone_));

    // Make the first possible move and store it as the best move, instead of holding an
    // infinite max_value
    test_board.move(actions[0]);
    this->ai_clone_->decrease_piece(actions[0]);
    this->minimax_depth_ += 1;
    this->ai_clone_->reset_megaliths(); this->user_clone_->reset_megaliths();
    test_board.update_megaliths(this->ai_clone_, this->user_clone_);
    max_value = this->min_value_ab(test_board, alpha, beta);
    this->minimax_depth_ -= 1;
    max_move = actions[0];
    test_board.undo(max_move);
    this->ai_clone_->reset_megaliths(); this->user_clone_->reset_megaliths();
    test_board.reset_megaliths();
    this->ai_clone_->reset_move(max_move);

    actions.erase(actions.begin());

    for (auto &it : actions) {
        test_board.move(it);
        this->ai_clone_->decrease_piece(it);
        this->minimax_depth_ += 1;
        this->ai_clone_->reset_megaliths(); this->user_clone_->reset_megaliths();
        test_board.update_megaliths(this->ai_clone_, this->user_clone_);
        tmp_value = this->min_value_ab(test_board, alpha, beta);
        test_board.undo(it);
        this->ai_clone_->reset_move(it);
        this->ai_clone_->reset_megaliths(); this->user_clone_->reset_megaliths();
        test_board.reset_megaliths();
        this->minimax_depth_ -= 1;
        if (tmp_value > max_value) {
            max_value = tmp_value;
            max_move = it;
        }
    }

    return max_move;

}


int Game::min_value_ab(Board &_board, int _alpha, int _beta)
{
    if (this->minimax_depth_ >= max_depth_) {
        this->nbr_of_nodes_ += 1;
        int utility = this->utility();
//        printf("Utility of the board: %d\n", utility);
//        _board.print_board();
        return utility;
    }

    int min_val = INT32_MAX;
    int tmp_val;
    std::vector<Move> actions = this->get_possible_moves(_board, *(this->user_clone_));

    if (actions.size() == 0) {
        int util = this->utility();
//        printf("Utility of the board: %d\n", util);
//        _board.print_board();
        return util;
    }

    for (auto &it : actions) {
        _board.move(it);
        this->user_clone_->decrease_piece(it);
        this->minimax_depth_ += 1;
        this->ai_clone_->reset_megaliths(); this->user_clone_->reset_megaliths();
        _board.update_megaliths(this->ai_clone_, this->user_clone_);
        tmp_val = this->max_value_ab(_board, _alpha, _beta);
        _board.undo(it);
        this->user_clone_->reset_move(it);
        this->ai_clone_->reset_megaliths(); this->user_clone_->reset_megaliths();
        _board.reset_megaliths();
        this->minimax_depth_ -= 1;
        if (tmp_val < min_val)
            min_val = tmp_val;
        if (min_val <= _alpha)
            return min_val;
        _beta = min_val < _beta ? min_val : _beta;
    }
    this->nbr_of_nodes_ += 1;
    return min_val;
}

int Game::max_value_ab(Board &_board, int _alpha, int _beta)
{
    if (this->minimax_depth_ == max_depth_) {
        this->nbr_of_nodes_ += 1;
        int util = this->utility();
//        printf("Utility of the board: %d\n", util);
//        _board.print_board();
        return util;
    }

    int max_val = INT32_MIN;
    int tmp_val;
    std::vector<Move> actions = this->get_possible_moves(_board, *(this->ai_clone_));

    if (actions.size() == 0) {
        int util = this->utility();
//        printf("Utility of the board: %d\n", util);
//        _board.print_board();
        return util;
    }

    for (auto &it : actions) {
        _board.move(it);
        this->ai_clone_->decrease_piece(it);
        this->minimax_depth_ += 1;
        this->ai_clone_->reset_megaliths(); this->user_clone_->reset_megaliths();
        _board.update_megaliths(this->ai_clone_, this->user_clone_);
        tmp_val = this->min_value_ab(_board, _alpha, _beta);
        _board.undo(it);
        this->ai_clone_->reset_move(it);
        this->ai_clone_->reset_megaliths(); this->user_clone_->reset_megaliths();
        _board.reset_megaliths();
        this->minimax_depth_ -= 1;
        max_val = tmp_val > max_val ? tmp_val : max_val;
        if (max_val >= _beta)
            return max_val;
        _alpha = max_val > _alpha ? max_val : _alpha;
    }
    this->nbr_of_nodes_ += 1;
    return max_val;
}

int Game::utility()
{
    if (this->user_clone_->nbr_of_megaliths_won() >= ENOUGH_SCORE_)
        return INT32_MIN;
    else if (this->ai_clone_->nbr_of_megaliths_won() >= ENOUGH_SCORE_)
        return INT32_MAX;

    return 50 * (this->ai_clone_->nbr_of_megaliths_won_minmax() - this->user_clone_->nbr_of_megaliths_won_minmax())
            + 10 * (this->ai_clone_->total_of_remaining_pieces() - this->user_clone_->total_of_remaining_pieces());
}


std::vector<Move> Game::get_possible_moves(const Board &_board,
                                           const Player& _current_player) const
{
    std::vector<Move> possible_moves;
    std::vector<uint> empty_locations = _board.get_empty_locations();

    for (uint i = 1; i <= _current_player.MAX_PIECE_VALUE; ++i)
        if (_current_player.get_remaining_pieces()[i-1] > 0)
            for(auto &it: empty_locations)
                possible_moves.push_back((Move(Piece(_current_player.get_color(), i), it)));

    return possible_moves;
}


bool Game::is_game_finished() {
    if (this->ai_player_.get()->nbr_of_megaliths_won() >= ENOUGH_SCORE_) {
        this->winner_ = this->ai_player_;
        return true;
    } else if (this->user_player_.get()->nbr_of_megaliths_won() >= ENOUGH_SCORE_) {
        this->winner_ = this->user_player_;
        return true;
    }

    return false;
}

void Game::update_current_player() {
    if (this->current_player_.get()->get_color() == this->user_player_.get()->get_color())
        this->current_player_ = this->ai_player_;
    else
        this->current_player_ = this->user_player_;
}





//Board *board_;
//Color first_player_;
//Color current_player_;
//std::unordered_map<int, int> black_player_pieces_;
//std::unordered_map<int, int> white_player_pieces_;
