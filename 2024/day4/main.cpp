#include <iostream>
#include <algorithm>

#include <fstream>
#include <sstream>
#include <string>

#include <memory>
#include <vector>

using namespace std;

// test.txt = 10x10
// data.txt = 140x140

#define DATA "data.txt"
#define W 140
#define H W

// First answer: 2406

constexpr char MATCH1[] = "XMAS";
constexpr size_t MATCH1_SIZE = sizeof(MATCH1);

constexpr char MATCH2[] = "MAS";
constexpr size_t MATCH2_SIZE = sizeof(MATCH2);

enum MoveType
{
    UL, U, UR,
     L, X,  R,
    DL, D, DR,
    INVALID
};

struct Position
{
    const uint16_t row;
    const uint16_t col;

    Position(const uint16_t r, const uint16_t c) : row{r}, col{c} 
    {

    }

    bool isValid() const
    {
        return (row < H && col < W);
    }

    string print() const 
    {
        return "[" + to_string(row) + "," + to_string(col) + "]";
    }

    uint16_t index() const
    {
        return col + (row * W);
    }

    Position move(MoveType mt) const
    {
        switch(mt) 
        {
            case UL: return Position(row-1, col-1);
            case  U: return Position(row-1, col);
            case UR: return Position(row-1, col+1);
            case  L: return Position(row,   col-1);
            case  X: return Position(row,   col);
            case  R: return Position(row,   col+1);
            case DL: return Position(row+1, col-1);
            case  D: return Position(row+1, col);
            case DR: return Position(row+1, col+1);
        }

        // Just some invalid value
        return Position(UINT16_MAX,UINT16_MAX);
    }

    bool operator == (const Position &other) const
    {      
        return col == other.col && row == other.row; 
    } 

    bool operator!=(const Position& other) const 
    {
        return !(*this == other);
    }
};

class Board 
{
public:

    Board(const std::string input, const char startPosChar)
    {
        data_ = std::unique_ptr<char[]>(new char[W * H]);

        int read_pos = 0;

        for (uint16_t r = 0; r < H; ++r) 
        {
            for (uint16_t c = 0; c < W; ++c) 
            {
                auto pos = Position(r, c);
                char v = input[read_pos];
                while (v == '\n' || v == '\r') 
                {
                    read_pos++;
                    v = input[read_pos];
                }

                data_[pos.index()] = v;

                if (v == startPosChar) 
                {
                    startPositions_.push_back(pos);
                }

                read_pos++;
            }
        }
    }

    char charAt(const Position p) const
    {
        char v = 0;

        if (p.isValid())
        {
            v = data_[p.index()];
        }
        else 
        {
            cout << "Invalid query" << endl;
        }
        return v;
    }

    void print() 
    {
        for (uint16_t r = 0; r < H; ++r) 
        {
            for (uint16_t c = 0; c < W; ++c) 
            {
                cout << charAt(Position(r, c)) << " ";
            }
            cout << endl;
        }
    }

    const vector<Position>& startPositions() const
    {
        return startPositions_;
    }   

private:

    std::unique_ptr<char[]> data_;

    // Helpers
    vector<Position> startPositions_; // all positions where 'X' 

};

std::ostream& operator<<(std::ostream& out, const MoveType mt) 
{
    switch(mt) 
    {
        case UL: return out <<  "UL";
        case  U: return out <<  " U";
        case UR: return out <<  "UR";
        case  L: return out <<  " L";
        case  X: return out <<  " X";
        case  R: return out <<  " R";
        case DL: return out <<  "DL";
        case  D: return out <<  " D";
        case DR: return out <<  "DR";
    }

    return out << "INVALID";
}

std::ostream& operator<<(std::ostream& out, const Position& mt) 
{
    return out << mt.print();
}

// std::ostream& operator<<(std::ostream& out, const Board& bd) 
// {
//     return out << bd.print();
// }


std::unique_ptr<Board> read_input(const string filename, const char startPosChar) 
{
    ifstream instream(filename);
    stringstream buffer;

    if (instream.good()) 
    {
        buffer << instream.rdbuf();

        auto board = std::unique_ptr<Board>(new Board(buffer.str(), startPosChar));
        return std::move(board);
    }
    else 
    {
        return nullptr;
    }
}

/// @brief match in any given position in the given direction
/// @param board the board
/// @param m value to match with (XMAS or MAS)
/// @param ms size of the m string
/// @param direction  the direction to match in
/// @param sp start postion
/// @param mp recursive counter in match string
/// @return true when match in direction is found
bool match(const Board* board, const char* m, size_t ms, const MoveType direction, const Position sp, uint16_t mp = 0) 
{
    if (mp == ms-1) 
    {
        // match found if we arrive here!
        return 1;
    }
    else if (sp.isValid() && board->charAt(sp) == m[mp]) 
    {
        // This is good char, now move to the next postion
        return match(board, m, ms, direction, sp.move(direction), ++mp);
    }
    else 
    {
        // Not a valid char or we moved off the board
        return false;
    }
}

void part1() 
{
    auto board = read_input("/home/thys/source/aoc/2024/day4/" DATA, 'X');

    if (board) 
    {
        const auto& sps = board->startPositions();
        bool res = false;
        int count = 0;
        for (const auto& sp : sps) 
        {
            for (int mti = 0; mti < MoveType::INVALID; mti++) 
            {
                res = match(board.get(), MATCH1, MATCH1_SIZE, (MoveType)mti, sp);
                if (res) count++;
            }
        }

        cout << "Count: " << count << endl;
    }
    else 
    {
        cout << "Failed to read input.." << endl;
    }
}

#define META board.get(), "MAS", 4

void part2() 
{
    auto board = read_input("/home/thys/source/aoc/2024/day4/" DATA, 'A');

    if (board) 
    {
        const auto& sps = board->startPositions(); // all with an 'A'
        bool res = false;
        int count = 0;
        for (const auto& sp : sps) 
        {
            // Match count, once this is 2, match is found
            int matchCount = 0;

            // All possible start positions
            auto pUL = sp.move(UL);
            auto pUR = sp.move(UR);
            auto pDL = sp.move(DL);
            auto pDR = sp.move(DR);

            // Upper left -> down right
            if (pUL.isValid() && match(META, DR, pUL)) 
            {
                matchCount++;
            }
            // Upper right -> down left
            if (pUR.isValid() && match(META, DL, pUR)) 
            {
                matchCount++;
                if (matchCount >= 2) // Done!!
                {
                    count++;
                    continue;
                }
            }
            // Down left -> upper right
            if (pDL.isValid() && match(META, UR, pDL)) 
            {
                matchCount++;
                if (matchCount >= 2) // Done!!
                {
                    count++;
                    continue;
                }
            }
            // Down right -> upper left
            if (pDR.isValid() && match(META, UL, pDR)) 
            {
                matchCount++;
                if (matchCount >= 2) // Done!!
                {
                    count++;
                    continue;
                }
            }
        }

        cout << "Count: " << count << endl;
    }
    else 
    {
        cout << "Failed to read input.." << endl;
    }
}

int main (int argc, char* argv[]) 
{
    cout << "hallo day 4" << endl;

    //part1();
    part2();

    return 0;
}


