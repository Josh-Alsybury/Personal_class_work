#pragma once
#include "Piece.h"

struct Player
{
    // counts for placement phase
    int donkeys = 3;
    int snake = 1;
    int frog = 1;

    // which piece the player is trying to place
    PieceType selected = PieceType::Donkey;

    //consume one unit of 'selected'
    bool take()
    {
        if (selected == PieceType::Donkey && donkeys > 0) { --donkeys; return true; }
        if (selected == PieceType::Snake && snake > 0) { --snake;   return true; }
        if (selected == PieceType::Frog && frog > 0) { --frog;    return true; }
        return false;
    }

    bool empty() const { return donkeys + snake + frog == 0; }
};
