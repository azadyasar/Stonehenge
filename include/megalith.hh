#ifndef MEGALITH_H
#define MEGALITH_H

#include <location.hh>
#include <enum_list.hh>
#include <vector>
#include <memory>
#include <player.hh>

// forward declaration
class Location;
class Megalith;
class Player;

typedef std::shared_ptr<Location> LocationPtr;
typedef std::shared_ptr<Megalith> MegalithPtr;
typedef std::shared_ptr<Piece> PiecePtr;

class Megalith
{
    typedef std::shared_ptr<Player> PlayerPtr;
public:
    Megalith(uint);
    Megalith(uint, bool, bool, Color, Color, size_t);
    std::vector<LocationPtr> get_locations() const;
    uint get_id() const;
    void set_first_played(Color);
    bool is_empty() const;
    size_t get_win_value() const;
    bool won() const;
    Color who_played_first() const;
    Color who_won() const;
    MEGALITH_STATUS get_win_status() const;
    void add_location(LocationPtr);
    void check_win(PlayerPtr, PlayerPtr);
    void check_win(Player*, Player*);
    void refresh(bool, bool, Color, Color, size_t);


    bool operator ==(const Megalith&) const;
    bool operator ==(const Megalith*) const;
    Megalith& operator =(const Megalith&);
    Megalith& operator =(const Megalith*);

private:
    // currently not used migrated to Player class
    unsigned int get_best_value(const std::vector<Piece>&, size_t) const;
    void update_win_status();

    uint id_;
    std::vector<LocationPtr> locations_;
    bool won_;
    bool is_empty_;
    Color who_won_;
    Color player_first_played_;
    size_t win_value_;
    MEGALITH_STATUS win_status_;   // win_status: Possible values (0, 1, 2) 0: not won yet. 1: either players won at the last turn 2: either players already won.
                        // When printing the board, it is used to distinguish between newly won megaliths.

};

#endif // MEGALITH_H
