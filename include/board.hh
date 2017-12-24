#ifndef BOARD_H
#define BOARD_H

#include <location.hh>
#include <megalith.hh>
#include <move.hh>
#include <enum_list.hh>
#include <unordered_map>
#include <stack>
#include <memory>

//Forward declarations
class Location;
class Megalith;
class Player;
typedef std::shared_ptr<Location> LocationPtr;
typedef std::shared_ptr<Megalith> MegalithPtr;
typedef std::shared_ptr<Player> PlayerPtr;

class Board
{
   typedef std::unordered_map<int, LocationPtr> LOC_MAP;
   typedef std::unordered_map<int, MegalithPtr> MEG_MAP;

public:
    Board();
    Board(Board&);
    bool move(const Move&);
    bool undo(const Move&);
    void update_megaliths(PlayerPtr, PlayerPtr);
    void update_megaliths(Player*, Player*);
    void reset_megaliths();
    // Helpers
    void print_board() const;
    void print_structure() const;
    LocationPtr get_location(uint);
    MegalithPtr get_megalith(uint);
    std::vector<uint> get_empty_locations() const;


    const size_t LOCATION_SIZE_ = 18;
    const size_t MEGALITH_SIZE_ = 15;

private:
    void init_board();
    void init_board(Board&);
    // we need a stack of history of megalith list because when going down in a game tree
    // just before every possible move we update undo_megalith_list_ hence it causes
    // ambiguity
    std::stack<std::vector<Megalith>> undo_megalith_stack_;
    LOC_MAP location_map_;
    MEG_MAP megalith_map_;
    // Game class also stores these values. Check..

};

#endif // BOARD_H
