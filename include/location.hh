#ifndef LOCATION_H
#define LOCATION_H

#include <enum_list.hh>
#include <piece.hh>
#include <player.hh>
#include <string>
#include <vector>
#include <memory>

// forward declaration
class Megalith;
class Location;
class Player;

typedef std::shared_ptr<Location> LocationPtr;
typedef std::shared_ptr<Megalith> MegalithPtr;
typedef std::shared_ptr<Piece> PiecePtr;
typedef std::shared_ptr<Player> PlayerPtr;

class Location
{
public:

    Location(uint);
    ~Location();
    uint get_id() const;
    Piece* get_piece();
    bool is_empty() const;
    void set_piece(Piece);
    void reset();
    void add_ley_line(MegalithPtr);
    const std::vector<MegalithPtr>& get_ley_lines() const;

    bool operator ==(const Location&) const;
    bool operator ==(const Location*) const;
//    Location& operator =(const Location&);
//    Location& operator =(const Location*);


private:
    uint id_;
    bool is_empty_;
    Piece* piece_;
    std::vector<MegalithPtr> ley_lines_;

};

std::ostream& operator<<(std::ostream&, Location&);
std::ostream& operator<<(std::ostream&,  Location*);

#endif // LOCATION_H
