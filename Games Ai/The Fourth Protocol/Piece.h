#pragma once
#include <cstdint>

enum class Owner : std::uint8_t { None, Human, NPC };
enum class PieceType : std::uint8_t { None, Donkey, Snake, Frog };

struct Cell
{
    PieceType type{ PieceType::None };
    Owner owner{ Owner::None };
};
