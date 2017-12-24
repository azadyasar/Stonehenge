#include "board.hh"
#include <iostream>
#include <stdio.h>

using std::cout;
using std::endl;

Board::Board()
{
    this->init_board();
}

Board::Board(Board &_rhs) {
    this->init_board(_rhs);
}

bool Board::move(const Move &_move) {
    LOC_MAP::iterator loc_map_it = this->location_map_.find(_move.position_id_);
    if (loc_map_it == this->location_map_.end()) {
        cout << "[ERROR]: Location with id " << _move.position_id_ << " does not exist!" << endl;
        return false;
    }

    if (!loc_map_it->second.get()->is_empty()) {
        cout << "[ERROR]: Location with id " << _move.position_id_ << " is already occupied!" << endl;
        return false;
    }

    loc_map_it->second.get()->set_piece(*(_move.piece_));
    return true;
}

bool Board::undo(const Move &_old_move)
{
    LOC_MAP::iterator loc_map_it = this->location_map_.find(_old_move.position_id_);
    if (loc_map_it == this->location_map_.end()) {
        cout << "[ERROR]: Location with id " << _old_move.position_id_ << " does not exist (from undo)\n";
        return false;
    }

    if (loc_map_it->second.get()->is_empty()) {
        cout << "[ERROR]: Location with id " << _old_move.position_id_ << " is already empty, nothing to undo\n";
        return false;
    }

    loc_map_it->second.get()->reset();
    return true;
}

void Board::update_megaliths(PlayerPtr _p1, PlayerPtr _p2) {
    for (auto &it : this->megalith_map_)
        it.second.get()->check_win(_p1, _p2);
}

// used by minimax algorithm. We need to keep the old state of the megaliths
// in order to recover when undoing
void Board::update_megaliths(Player *_p1, Player *_p2)
{
    std::vector<Megalith> meg_list;
    MegalithPtr mptr;
    for (auto &it : this->megalith_map_) {
        mptr = it.second;
        meg_list.push_back(Megalith(mptr.get()->get_id(), mptr.get()->won(),
                                                     mptr.get()->is_empty(), mptr.get()->who_won(),
                                                     mptr.get()->who_played_first(), mptr.get()->get_win_value()));
        it.second.get()->check_win(_p1, _p2);
    }
    this->undo_megalith_stack_.push(meg_list);
}

void Board::reset_megaliths()
{
    std::vector<Megalith> meg_list = this->undo_megalith_stack_.top();
    this->undo_megalith_stack_.pop();
    MEG_MAP::iterator m_iter;
    for (Megalith &it : meg_list) {
        m_iter = this->megalith_map_.find(it.get_id());
        if (m_iter == this->megalith_map_.end())
            cout << "No megalith found with id " << it.get_id() << " when resetting megaliths from board\n";
        m_iter->second.get()->refresh(it.won(), it.is_empty(), it.who_won(), it.who_played_first(), it.get_win_value());
    }
}

LocationPtr Board::get_location(uint _id) {
    LOC_MAP::iterator loc_map_it = this->location_map_.find(_id);
    if (loc_map_it == this->location_map_.end()) {
        cout << "[WARNING]: Location with id " << _id << " does not exist.\n";
        return nullptr;
    }
    return loc_map_it->second;
}


MegalithPtr Board::get_megalith(uint _id) {
    MEG_MAP::iterator meg_map_it = this->megalith_map_.find(_id);
    if (meg_map_it == this->megalith_map_.end()) {
        cout << "[WARNING]: Megalith with id " << _id << " does not exist.\n";
        return nullptr;
    }
    return meg_map_it->second;
}

std::vector<uint> Board::get_empty_locations() const
{
    std::vector<uint> result;
    for (auto& it: this->location_map_)
        if (it.second.get()->is_empty())
            result.push_back(it.first);
    return result;
}


void Board::print_board() const {
    MEG_MAP::const_iterator meg_map_it;
    LOC_MAP::const_iterator loc_map_it;

    printf("\n\t\t\t\t\t### BOARD ###\n\n");

    // ----------------------------------FIRST ROW------------------------------------------------
    printf("\t\t\t\t\t\t");
    meg_map_it = this->megalith_map_.find(1);
    if (meg_map_it->second.get()->is_empty() || !meg_map_it->second.get()->won())
        printf("|*|\t");
    else if (meg_map_it->second.get()->get_win_status() == ALREADY_WON)
        printf("%c_%lu\t", meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());
    else // just won
        printf("$%c_%lu\t", meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());

    meg_map_it = this->megalith_map_.find(2);
    if (meg_map_it->second.get()->is_empty() || !meg_map_it->second.get()->won())
        printf("|*|\n");
    else if (meg_map_it->second.get()->get_win_status() == ALREADY_WON)
        printf("%c_%lu\n", meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());
    else // just won
        printf("$%c_%lu\n", meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());

    printf("\t\t\t\t\t\t/\t/\n");

    // ----------------------------------SECOND ROW------------------------------------------------
    printf("\t\t\t\t\t");
    meg_map_it = this->megalith_map_.find(3);
    if (meg_map_it->second.get()->is_empty() || !meg_map_it->second.get()->won())
        printf("|*|____");
    else if (meg_map_it->second.get()->get_win_status() == ALREADY_WON)
        printf("%c_%lu____", meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());
    else
        printf("$%c_%lu____", meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());

    loc_map_it = this->location_map_.find(1);
    if (loc_map_it->second.get()->is_empty())
        printf("|_|____");
    else
        printf("%c_%lu____", loc_map_it->second.get()->get_piece()->get_color(), loc_map_it->second.get()->get_piece()->get_value());

    loc_map_it = this->location_map_.find(2);
    if (loc_map_it->second.get()->is_empty())
        printf("|_%-4c", '|');
    else
        printf("%c_%-4lu", loc_map_it->second.get()->get_piece()->get_color(), loc_map_it->second.get()->get_piece()->get_value());

    meg_map_it = this->megalith_map_.find(4);
    if (meg_map_it->second.get()->is_empty() || !meg_map_it->second.get()->won())
        printf("|*|\n");
    else if (meg_map_it->second.get()->get_win_status() == ALREADY_WON)
        printf("%c_%lu\n", meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());
    else
        printf("$%c_%lu\n", meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());

    printf("\t\t\t\t\t%8c \\%5c \\%4c\n", '/', '/', '/');
    printf("\t\t\t\t\t%7c%4c%3c%4c%2c\n", '/', '\\', '/', '\\', '/');


    // ----------------------------------THIRD ROW------------------------------------------------
    printf("\t\t\t\t   ");
    meg_map_it = this->megalith_map_.find(5);
    if (meg_map_it->second.get()->is_empty() || !meg_map_it->second.get()->won())
        printf("%2c*|____", '|');
    else if (meg_map_it->second.get()->get_win_status() == ALREADY_WON)
        printf("%2c_%lu____", meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());
    else
        printf("%2c%c_%lu____", '$', meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());

    loc_map_it = this->location_map_.find(3);
    if (loc_map_it->second.get()->is_empty())
        printf("|_|____");
    else
        printf("%c_%lu____", loc_map_it->second.get()->get_piece()->get_color(), loc_map_it->second.get()->get_piece()->get_value());

    loc_map_it = this->location_map_.find(4);
    if (loc_map_it->second.get()->is_empty())
        printf("|_|____");
    else
        printf("%c_%lu____", loc_map_it->second.get()->get_piece()->get_color(), loc_map_it->second.get()->get_piece()->get_value());

    loc_map_it = this->location_map_.find(5);
    if (loc_map_it->second.get()->is_empty())
        printf("|_%-4c", '|');
    else
        printf("%c_%-4lu", loc_map_it->second.get()->get_piece()->get_color(), loc_map_it->second.get()->get_piece()->get_value());

    meg_map_it = this->megalith_map_.find(6);
    if (meg_map_it->second.get()->is_empty() || !meg_map_it->second.get()->won())
        printf("|*|\n");
    else if (meg_map_it->second.get()->get_win_status() == ALREADY_WON)
        printf("%c_%lu\n", meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());
    else
        printf("$%c_%lu\n", meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());

    printf("\t\t\t\t%12c \\%5c \\%5c \\%4c\n", '/', '/', '/', '/');
    printf("\t\t\t\t\t%3c%4c%3c%4c%3c%4c%2c\n", '/', '\\', '/', '\\', '/', '\\', '/');

    // ----------------------------------FOURTH ROW------------------------------------------------

    printf("\t\t");
    meg_map_it = this->megalith_map_.find(7);
    if (meg_map_it->second.get()->is_empty() || !meg_map_it->second.get()->won())
        printf("%18c*|____", '|');
    else if (meg_map_it->second.get()->get_win_status() == ALREADY_WON)
        printf("%18c_%lu___", meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());
    else
        printf("%17c%c_%lu___", '$', meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());


    loc_map_it = this->location_map_.find(6);
    if (loc_map_it->second.get()->is_empty())
        printf("|_|____");
    else
        printf("%c_%lu____", loc_map_it->second.get()->get_piece()->get_color(), loc_map_it->second.get()->get_piece()->get_value());

    loc_map_it = this->location_map_.find(7);
    if (loc_map_it->second.get()->is_empty())
        printf("|_|____");
    else
        printf("%c_%lu____", loc_map_it->second.get()->get_piece()->get_color(), loc_map_it->second.get()->get_piece()->get_value());

    loc_map_it = this->location_map_.find(8);
    if (loc_map_it->second.get()->is_empty())
        printf("|_|____");
    else
        printf("%c_%lu____", loc_map_it->second.get()->get_piece()->get_color(), loc_map_it->second.get()->get_piece()->get_value());

    loc_map_it = this->location_map_.find(9);
    if (loc_map_it->second.get()->is_empty())
        printf("|_%-4c", '|');
    else
        printf("%c_%-4lu", loc_map_it->second.get()->get_piece()->get_color(), loc_map_it->second.get()->get_piece()->get_value());

    meg_map_it = this->megalith_map_.find(8);
    if (meg_map_it->second.get()->is_empty() || !meg_map_it->second.get()->won())
        printf("|*|\n");
    else if (meg_map_it->second.get()->get_win_status() == ALREADY_WON)
        printf("%c_%lu\n", meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());
    else
        printf("$%c_%lu\n", meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());

    printf("\t\t\t\t%9c%2c%5c%2c%5c%2c%5c%2c%4c\n", '/', '\\', '/', '\\', '/', '\\', '/', '\\', '/');
    printf("\t\t\t\t%8c%4c%3c%4c%3c%4c%3c%4c%2c\n", '/', '\\', '/', '\\', '/', '\\', '/', '\\', '/');

    // ----------------------------------FIFTH ROW------------------------------------------------
    printf("\t\t");

    meg_map_it = this->megalith_map_.find(9);
    if (meg_map_it->second.get()->is_empty() || !meg_map_it->second.get()->won())
        printf("%14c*|____", '|');
    else if (meg_map_it->second.get()->get_win_status() == ALREADY_WON)
        printf("%14c_%lu____", meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());
    else
        printf("%14c%c_%lu____", '$', meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());

    loc_map_it = this->location_map_.find(10);
    if (loc_map_it->second.get()->is_empty())
        printf("|_|____");
    else
        printf("%c_%lu____", loc_map_it->second.get()->get_piece()->get_color(), loc_map_it->second.get()->get_piece()->get_value());

    loc_map_it = this->location_map_.find(11);
    if (loc_map_it->second.get()->is_empty())
        printf("|_|____");
    else
        printf("%c_%lu____", loc_map_it->second.get()->get_piece()->get_color(), loc_map_it->second.get()->get_piece()->get_value());

    loc_map_it = this->location_map_.find(12);
    if (loc_map_it->second.get()->is_empty())
        printf("|_|____");
    else
        printf("%c_%lu____", loc_map_it->second.get()->get_piece()->get_color(), loc_map_it->second.get()->get_piece()->get_value());

    loc_map_it = this->location_map_.find(13);
    if (loc_map_it->second.get()->is_empty())
        printf("|_|____");
    else
        printf("%c_%lu____", loc_map_it->second.get()->get_piece()->get_color(), loc_map_it->second.get()->get_piece()->get_value());

    loc_map_it = this->location_map_.find(14);
    if (loc_map_it->second.get()->is_empty())
        printf("|_|\n");
    else
        printf("%c_%lu\n", loc_map_it->second.get()->get_piece()->get_color(), loc_map_it->second.get()->get_piece()->get_value());

    printf("\t\t\t\t%7c%5c%2c%5c%2c%5c%2c%5c%2c\n", '\\', '/', '\\', '/', '\\', '/', '\\', '/', '\\');
    printf("\t\t\t\t%8c%3c%4c%3c%4c%3c%4c%3c%4c\n", '\\', '/', '\\', '/', '\\', '/', '\\', '/', '\\');


    // ----------------------------------SIXTH ROW------------------------------------------------

    printf("\t\t");
    meg_map_it = this->megalith_map_.find(10);
    if (meg_map_it->second.get()->is_empty() || !meg_map_it->second.get()->won())
        printf("%17c*|____", '|');
    else if (meg_map_it->second.get()->get_win_status() == ALREADY_WON)
        printf("%17c_%lu____", meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());
    else
        printf("%17c%c_%lu____", '$', meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());

    loc_map_it = this->location_map_.find(15);
    if (loc_map_it->second.get()->is_empty())
        printf("|_|____");
    else
        printf("%c_%lu____", loc_map_it->second.get()->get_piece()->get_color(), loc_map_it->second.get()->get_piece()->get_value());

    loc_map_it = this->location_map_.find(16);
    if (loc_map_it->second.get()->is_empty())
        printf("|_|____");
    else
        printf("%c_%lu____", loc_map_it->second.get()->get_piece()->get_color(), loc_map_it->second.get()->get_piece()->get_value());

    loc_map_it = this->location_map_.find(17);
    if (loc_map_it->second.get()->is_empty())
        printf("|_|____");
    else
        printf("%c_%lu____", loc_map_it->second.get()->get_piece()->get_color(), loc_map_it->second.get()->get_piece()->get_value());

    loc_map_it = this->location_map_.find(18);
    if (loc_map_it->second.get()->is_empty())
        printf("|_%-5c", '|');
    else
        printf("%c_%-5lu", loc_map_it->second.get()->get_piece()->get_color(), loc_map_it->second.get()->get_piece()->get_value());

    meg_map_it = this->megalith_map_.find(11);
    if (meg_map_it->second.get()->is_empty() || !meg_map_it->second.get()->won())
        printf("|*|\n");
    else if (meg_map_it->second.get()->get_win_status() == ALREADY_WON)
        printf("%c_%lu\n", meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());
    else
        printf("$%c_%lu\n", meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());

    printf("\t\t\t\t%9c%7c%7c%7c\n",  '\\', '\\', '\\', '\\');
    printf("\t\t\t\t%10c%7c%7c%7c\n",  '\\', '\\', '\\', '\\');

    // ----------------------------------SEVENTH ROW------------------------------------------------

    printf("\t\t\t");
    meg_map_it = this->megalith_map_.find(12);
    if (meg_map_it->second.get()->is_empty() || !meg_map_it->second.get()->won())
        printf("%18c*|", '|');
    else if (meg_map_it->second.get()->get_win_status() == ALREADY_WON)
        printf("%18c_%lu", meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());
    else
        printf("%18c%c_%lu", '$', meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());

    meg_map_it = this->megalith_map_.find(13);
    if (meg_map_it->second.get()->is_empty() || !meg_map_it->second.get()->won())
        printf("%5c*|", '|');
    else if (meg_map_it->second.get()->get_win_status() == ALREADY_WON)
        printf("%5c_%lu", meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());
    else
        printf("%5c%c_%lu", '$', meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());

    meg_map_it = this->megalith_map_.find(14);
    if (meg_map_it->second.get()->is_empty() || !meg_map_it->second.get()->won())
        printf("%5c*|", '|');
    else if (meg_map_it->second.get()->get_win_status() == ALREADY_WON)
        printf("%5c_%lu", meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());
    else
        printf("%5c%c_%lu", '$', meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());

    meg_map_it = this->megalith_map_.find(15);
    if (meg_map_it->second.get()->is_empty() || !meg_map_it->second.get()->won())
        printf("%5c*|", '|');
    else if (meg_map_it->second.get()->get_win_status() == ALREADY_WON)
        printf("%5c_%lu", meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());
    else
        printf("%5c%c_%lu", '$', meg_map_it->second.get()->who_won(), meg_map_it->second.get()->get_win_value());

    printf("\n\n");

}

void Board::print_structure() const {
    cout << "Megaliths\n";
    for (MEG_MAP::const_iterator it = this->megalith_map_.begin();
            it != this->megalith_map_.end(); it++)
    {
        std::cout << "Megalith [" << it->second.get()->get_id() << "]: ";
        for (auto& inner_it : it->second.get()->get_locations())
            cout << inner_it.get()->get_id() << ", ";
        cout << endl;
    }

    cout << "Locations\n";
    for (LOC_MAP::const_iterator it = this->location_map_.begin();
         it != this->location_map_.end(); it++)
    {
        cout << "Location [" << it->second.get()->get_id() << "]: ";
        for (auto& inner_it : it->second.get()->get_ley_lines())
            cout << inner_it.get()->get_id() << ", ";
        cout << endl;
    }
}

// left-of FILL BOARD;
void Board::init_board() {
//    LOC_MAP location_map_;
//    MEG_MAP megalith_map_;

    std::unordered_map<int, std::vector<int>> locations_megaliths;
    const std::vector<int> loc1  =  {1, 3, 15};
    locations_megaliths[1]       = loc1;
    const std::vector<int> loc2  =  {2, 3, 11};
    locations_megaliths[2]       = loc2;
    const std::vector<int> loc3  =  {1, 5, 14};
    locations_megaliths[3]       = loc3;
    const std::vector<int> loc4  =  {2, 5, 15};
    locations_megaliths[4]       = loc4;
    const std::vector<int> loc5  =  {4, 5, 11};
    locations_megaliths[5]       = loc5;
    const std::vector<int> loc6  =  {1, 7, 13};
    locations_megaliths[6]       = loc6;
    const std::vector<int> loc7  =  {2, 7, 14};
    locations_megaliths[7]       = loc7;
    const std::vector<int> loc8  =  {4, 7, 15};
    locations_megaliths[8]       = loc8;
    const std::vector<int> loc9  =  {6, 7, 11};
    locations_megaliths[9]       = loc9;
    const std::vector<int> loc10 =  {1, 9, 12};
    locations_megaliths[10]       = loc10;
    const std::vector<int> loc11 =  {2, 9, 13};
    locations_megaliths[11]       = loc11;
    const std::vector<int> loc12 =  {4, 9, 14};
    locations_megaliths[12]       = loc12;
    const std::vector<int> loc13 =  {6, 9, 15};
    locations_megaliths[13]       = loc13;
    const std::vector<int> loc14 =  {8, 9, 11};
    locations_megaliths[14]       = loc14;
    const std::vector<int> loc15 = {2, 10, 12};
    locations_megaliths[15]       = loc15;
    const std::vector<int> loc16 = {4, 10, 13};
    locations_megaliths[16]       = loc16;
    const std::vector<int> loc17 = {6, 10, 14};
    locations_megaliths[17]       = loc17;
    const std::vector<int> loc18 = {8, 10, 15};
    locations_megaliths[18]       = loc18;


    for (size_t i = 1; i <= this->LOCATION_SIZE_; ++i)
        this->location_map_.insert(std::make_pair(i, std::make_shared<Location>(Location(i))));

    for (size_t i = 1; i <= this->MEGALITH_SIZE_; ++i)
        this->megalith_map_.insert(std::make_pair(i, std::make_shared<Megalith>(Megalith(i))));

    std::unordered_map<int, std::vector<int>>::const_iterator c_it;
    for (auto & it : this->location_map_) {

        c_it = locations_megaliths.find(it.second.get()->get_id());

        if (c_it == locations_megaliths.end()) {
            std::cout << "Location with id " << it.second.get()->get_id() << " does not exist. [init_board1]\n";
            return;
        }

        MEG_MAP::iterator meg_map_it;
        for (auto& inner_it: c_it->second) {
              meg_map_it = this->megalith_map_.find(inner_it);
              if (meg_map_it == this->megalith_map_.end()) {
                  std::cout << "Megalith with id " << inner_it << " does not exist. [init_board1]\n";
                  return;
              }
            it.second.get()->add_ley_line(meg_map_it->second);
        }
    }

    std::unordered_map<int, std::vector<int>> megaliths_locations;
    const std::vector<int> meg1  =        {1, 3, 6, 10};
    megaliths_locations[1]       =                 meg1;
    const std::vector<int> meg2  =    {2, 4, 7, 11, 15};
    megaliths_locations[2]       =                 meg2;
    const std::vector<int> meg3  =               {1, 2};
    megaliths_locations[3]       =                 meg3;
    const std::vector<int> meg4  =       {5, 8, 12, 16};
    megaliths_locations[4]       =                 meg4;
    const std::vector<int> meg5  =            {3, 4, 5};
    megaliths_locations[5]       =                 meg5;
    const std::vector<int> meg6  =          {9, 13, 17};
    megaliths_locations[6]       =                 meg6;
    const std::vector<int> meg7  =         {6, 7, 8, 9};
    megaliths_locations[7]       =                 meg7;
    const std::vector<int> meg8  =             {14, 18};
    megaliths_locations[8]       =                 meg8;
    const std::vector<int> meg9  = {10, 11, 12, 13, 14};
    megaliths_locations[9]       =                 meg9;
    const std::vector<int> meg10 =     {15, 16, 17, 18};
    megaliths_locations[10]      =                meg10;
    const std::vector<int> meg11 =        {2, 5, 9, 14};
    megaliths_locations[11]      =                meg11;
    const std::vector<int> meg12 =             {10, 15};
    megaliths_locations[12]      =                meg12;
    const std::vector<int> meg13 =          {6, 11, 16};
    megaliths_locations[13]      =                meg13;
    const std::vector<int> meg14 =       {3, 7, 12, 17};
    megaliths_locations[14]      =                meg14;
    const std::vector<int> meg15 =    {1, 4, 8, 13, 18};
    megaliths_locations[15]      =                meg15;

    for (auto & it : this->megalith_map_) {

        c_it = megaliths_locations.find(it.second.get()->get_id());

        if (c_it == megaliths_locations.end()) {
            std::cout << "Location with id " << it.second.get()->get_id() << " does not exist. [init_board2]\n";
            return;
        }

        LOC_MAP::iterator loc_map_it;
        for (auto& inner_it: c_it->second) {
              loc_map_it = this->location_map_.find(inner_it);
              if (loc_map_it == this->location_map_.end()) {
                  std::cout << "Megalith with id " << inner_it << " does not exist. [init_board2]\n";
                  return;
              }
            it.second.get()->add_location(loc_map_it->second);
        }
    }
}


void Board::init_board(Board& _board) {
//    LOC_MAP location_map_;
//    MEG_MAP megalith_map_;

    std::unordered_map<int, std::vector<int>> locations_megaliths;
    const std::vector<int> loc1  =  {1, 3, 15};
    locations_megaliths[1]       = loc1;
    const std::vector<int> loc2  =  {2, 3, 11};
    locations_megaliths[2]       = loc2;
    const std::vector<int> loc3  =  {1, 5, 14};
    locations_megaliths[3]       = loc3;
    const std::vector<int> loc4  =  {2, 5, 15};
    locations_megaliths[4]       = loc4;
    const std::vector<int> loc5  =  {4, 5, 11};
    locations_megaliths[5]       = loc5;
    const std::vector<int> loc6  =  {1, 7, 13};
    locations_megaliths[6]       = loc6;
    const std::vector<int> loc7  =  {2, 7, 14};
    locations_megaliths[7]       = loc7;
    const std::vector<int> loc8  =  {4, 7, 15};
    locations_megaliths[8]       = loc8;
    const std::vector<int> loc9  =  {6, 7, 11};
    locations_megaliths[9]       = loc9;
    const std::vector<int> loc10 =  {1, 9, 12};
    locations_megaliths[10]       = loc10;
    const std::vector<int> loc11 =  {2, 9, 13};
    locations_megaliths[11]       = loc11;
    const std::vector<int> loc12 =  {4, 9, 14};
    locations_megaliths[12]       = loc12;
    const std::vector<int> loc13 =  {6, 9, 15};
    locations_megaliths[13]       = loc13;
    const std::vector<int> loc14 =  {8, 9, 11};
    locations_megaliths[14]       = loc14;
    const std::vector<int> loc15 = {2, 10, 12};
    locations_megaliths[15]       = loc15;
    const std::vector<int> loc16 = {4, 10, 13};
    locations_megaliths[16]       = loc16;
    const std::vector<int> loc17 = {6, 10, 14};
    locations_megaliths[17]       = loc17;
    const std::vector<int> loc18 = {8, 10, 15};
    locations_megaliths[18]       = loc18;


    MegalithPtr meg_ptr_rhs;
    LocationPtr loc_ptr_rhs;
    for (size_t i = 1; i <= this->LOCATION_SIZE_; ++i) {
        loc_ptr_rhs = _board.get_location(i);
        LocationPtr tmp_loc_ptr = std::make_shared<Location>(Location(loc_ptr_rhs.get()->get_id()));
        if (!loc_ptr_rhs.get()->is_empty())
            tmp_loc_ptr.get()->set_piece(*loc_ptr_rhs.get()->get_piece());
        this->location_map_.insert(std::make_pair(i, tmp_loc_ptr));
    }

    for (size_t i = 1; i <= this->MEGALITH_SIZE_; ++i) {
        meg_ptr_rhs = _board.get_megalith(i);
        this->megalith_map_.insert(std::make_pair(i, std::make_shared<Megalith>(Megalith(meg_ptr_rhs.get()->get_id(),
                                                                                         meg_ptr_rhs.get()->won(),
                                                                                         meg_ptr_rhs.get()->is_empty(),
                                                                                         meg_ptr_rhs.get()->who_won(),
                                                                                         meg_ptr_rhs.get()->who_played_first(),
                                                                                         meg_ptr_rhs.get()->get_win_value()))));
    }

    std::unordered_map<int, std::vector<int>>::const_iterator c_it;
    for (auto & it : this->location_map_) {

        c_it = locations_megaliths.find(it.second.get()->get_id());

        if (c_it == locations_megaliths.end()) {
            std::cout << "Location with id " << it.second.get()->get_id() << " does not exist. [init_board1]\n";
            return;
        }

        MEG_MAP::iterator meg_map_it;
        for (auto& inner_it: c_it->second) {
              meg_map_it = this->megalith_map_.find(inner_it);
              if (meg_map_it == this->megalith_map_.end()) {
                  std::cout << "Megalith with id " << inner_it << " does not exist. [init_board1]\n";
                  return;
              }
            it.second.get()->add_ley_line(meg_map_it->second);
        }
    }

    std::unordered_map<int, std::vector<int>> megaliths_locations;
    const std::vector<int> meg1  =        {1, 3, 6, 10};
    megaliths_locations[1]       =                 meg1;
    const std::vector<int> meg2  =    {2, 4, 7, 11, 15};
    megaliths_locations[2]       =                 meg2;
    const std::vector<int> meg3  =               {1, 2};
    megaliths_locations[3]       =                 meg3;
    const std::vector<int> meg4  =       {5, 8, 12, 16};
    megaliths_locations[4]       =                 meg4;
    const std::vector<int> meg5  =            {3, 4, 5};
    megaliths_locations[5]       =                 meg5;
    const std::vector<int> meg6  =          {9, 13, 17};
    megaliths_locations[6]       =                 meg6;
    const std::vector<int> meg7  =         {6, 7, 8, 9};
    megaliths_locations[7]       =                 meg7;
    const std::vector<int> meg8  =             {14, 18};
    megaliths_locations[8]       =                 meg8;
    const std::vector<int> meg9  = {10, 11, 12, 13, 14};
    megaliths_locations[9]       =                 meg9;
    const std::vector<int> meg10 =     {15, 16, 17, 18};
    megaliths_locations[10]      =                meg10;
    const std::vector<int> meg11 =        {2, 5, 9, 14};
    megaliths_locations[11]      =                meg11;
    const std::vector<int> meg12 =             {10, 15};
    megaliths_locations[12]      =                meg12;
    const std::vector<int> meg13 =          {6, 11, 16};
    megaliths_locations[13]      =                meg13;
    const std::vector<int> meg14 =       {3, 7, 12, 17};
    megaliths_locations[14]      =                meg14;
    const std::vector<int> meg15 =    {1, 4, 8, 13, 18};
    megaliths_locations[15]      =                meg15;

    for (auto & it : this->megalith_map_) {

        c_it = megaliths_locations.find(it.second.get()->get_id());

        if (c_it == megaliths_locations.end()) {
            std::cout << "Location with id " << it.second.get()->get_id() << " does not exist. [init_board2]\n";
            return;
        }

        LOC_MAP::iterator loc_map_it;
        for (auto& inner_it: c_it->second) {
              loc_map_it = this->location_map_.find(inner_it);
              if (loc_map_it == this->location_map_.end()) {
                  std::cout << "Megalith with id " << inner_it << " does not exist. [init_board2]\n";
                  return;
              }
            it.second.get()->add_location(loc_map_it->second);
        }
    }
}




















