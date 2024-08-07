#include "core/minefield.hpp"

#include <vector>

#include <gtest/gtest.h>

using namespace mswpr;

namespace mswpr
{
  std::ostream& operator<<(std::ostream& os, const mswpr::cell_coord& coord)
  {
    return os << '(' << coord.x << ", " << coord.y << ')';
  }
}

TEST(MinefieldRevealCells, GetNeighbours_LeftTop)
{
  minefield field(5, 5, 0);

  cell_coord coord{ 0, 0 };
  const auto neighbours = field.get_neighbours(coord);

  const std::vector<cell_coord> cells = { { 1, 0 }, { 0, 1 }, { 1, 1 } };
  EXPECT_EQ(neighbours, cells);
}

TEST(MinefieldRevealCells, GetNeighbours_RightTop)
{
  minefield field(5, 5, 0);

  cell_coord coord{ 4, 0 };
  const auto neighbours = field.get_neighbours(coord);

  const std::vector<cell_coord> cells = { { 3, 0 }, { 3, 1 }, { 4, 1 } };
  EXPECT_EQ(neighbours, cells);
}

TEST(MinefieldRevealCells, GetNeighbours_LeftBottom)
{
  minefield field(5, 5, 0);

  cell_coord coord{ 0, 4 };
  const auto neighbours = field.get_neighbours(coord);

  const std::vector<cell_coord> cells = { { 0, 3 }, { 1, 3 }, { 1, 4 } };
  EXPECT_EQ(neighbours, cells);
}

TEST(MinefieldRevealCells, GetNeighbours_RightBottom)
{
  minefield field(5, 5, 0);

  cell_coord coord{ 4, 4 };
  const auto neighbours = field.get_neighbours(coord);

  const std::vector<cell_coord> cells = { { 3, 3 }, { 4, 3 }, { 3, 4 } };
  EXPECT_EQ(neighbours, cells);
}

TEST(MinefieldRevealCells, GetNeighbours_Top)
{
  minefield field(5, 5, 0);

  cell_coord coord{ 2, 0 };
  const auto neighbours = field.get_neighbours(coord);

  const std::vector<cell_coord> cells = { { 1, 0 }, { 3, 0 }, { 1, 1 }, { 2, 1 }, { 3, 1 } };
  EXPECT_EQ(neighbours, cells);
}

TEST(MinefieldRevealCells, GetNeighbours_Right)
{
  minefield field(5, 5, 0);

  cell_coord coord{ 4, 2 };
  const auto neighbours = field.get_neighbours(coord);

  const std::vector<cell_coord> cells = { { 3, 1 }, { 4, 1 }, { 3, 2 }, { 3, 3 }, { 4, 3 } };
  EXPECT_EQ(neighbours, cells);
}

TEST(MinefieldRevealCells, GetNeighbours_Bottom)
{
  minefield field(5, 5, 0);

  cell_coord coord{ 2, 4 };
  const auto neighbours = field.get_neighbours(coord);

  const std::vector<cell_coord> cells = { { 1, 3 }, { 2, 3 }, { 3, 3 }, { 1, 4 }, { 3, 4 } };
  EXPECT_EQ(neighbours, cells);
}

TEST(MinefieldRevealCells, GetNeighbours_Left)
{
  minefield field(5, 5, 0);

  cell_coord coord{ 0, 2 };
  const auto neighbours = field.get_neighbours(coord);

  const std::vector<cell_coord> cells = { { 0, 1 }, { 1, 1 }, { 1, 2 }, { 0, 3 }, { 1, 3 } };
  EXPECT_EQ(neighbours, cells);
}

TEST(MinefieldRevealCells, GetNeighbours_Central)
{
  minefield field(5, 5, 0);

  cell_coord coord{ 2, 2 };
  const auto neighbours = field.get_neighbours(coord);

  const std::vector<cell_coord> cells = {
    { 1, 1 }, { 2, 1 }, { 3, 1 }, { 1, 2 }, { 3, 2 }, { 1, 3 }, { 2, 3 }, { 3, 3 }
  };
  EXPECT_EQ(neighbours, cells);
}

TEST(MinefieldRevealCells, RevealCells)
{
  const std::vector<size_t> mines_ind = { 6 };
  const size_t width = 3;
  const size_t height = 3;
  const size_t bomb_cnt = mines_ind.size();
  mswpr::minefield field(mines_ind, width, height, bomb_cnt);

  field.reveal_closed(1, 0);

  for (size_t y = 0; y < height; ++y)
  {
    for (size_t x = 0; x < width; ++x)
    {
      if (field.is_bomb(x, y))
      {
        EXPECT_TRUE(field.is_closed(x, y)) << "at (" << x << ", " << y << ")\n";
      }
      else
      {
        EXPECT_TRUE(field.is_opened(x, y)) << "at (" << x << ", " << y << ")\n";
      }
    }
  }
}

TEST(MinefieldRevealCells, AngleComponent)
{
  const std::vector<size_t> mines_ind = { 2, 13 };
  const size_t width = 3;
  const size_t height = 5;
  const size_t bomb_cnt = mines_ind.size();
  mswpr::minefield field(mines_ind, width, height, bomb_cnt);

  field.reveal_closed(0, 2);

  for (size_t y = 0; y < height - 1; ++y)
  {
    for (size_t x = 0; x < width; ++x)
    {
      if (field.is_bomb(x, y))
      {
        EXPECT_TRUE(field.is_closed(x, y)) << "at (" << x << ", " << y << ")\n";
      }
      else
      {
        EXPECT_TRUE(field.is_opened(x, y)) << "at (" << x << ", " << y << ")\n";
      }
    }
  }

  // process last line manually
  EXPECT_TRUE(field.is_closed(0, 4));
  EXPECT_TRUE(field.is_closed(1, 4));
  EXPECT_TRUE(field.is_closed(2, 4));
}

TEST(MinefieldRevealCells, MineInTheMiddleComponent)
{
  const std::vector<size_t> mines_ind = { 12 };
  const size_t width = 5;
  const size_t height = 5;
  const size_t bomb_cnt = mines_ind.size();
  mswpr::minefield field(mines_ind, width, height, bomb_cnt);

  field.reveal_closed(0, 0);

  for (size_t y = 0; y < height; ++y)
  {
    for (size_t x = 0; x < width; ++x)
    {
      if (field.is_bomb(x, y))
      {
        EXPECT_TRUE(field.is_closed(x, y)) << "at (" << x << ", " << y << ")\n";
      }
      else
      {
        EXPECT_TRUE(field.is_opened(x, y)) << "at (" << x << ", " << y << ")\n";
      }
    }
  }
}

TEST(MinefieldRevealCells, OneComponentByDiagonal)
{
  const std::vector<size_t> mines_ind = { 0, 15 };
  const size_t width = 4;
  const size_t height = 4;
  const size_t bomb_cnt = mines_ind.size();
  mswpr::minefield field(mines_ind, width, height, bomb_cnt);

  field.reveal_closed(3, 0);

  for (size_t y = 0; y < height; ++y)
  {
    for (size_t x = 0; x < width; ++x)
    {
      if (field.is_bomb(x, y))
      {
        EXPECT_TRUE(field.is_closed(x, y)) << "at (" << x << ", " << y << ")\n";
      }
      else
      {
        EXPECT_TRUE(field.is_opened(x, y)) << "at (" << x << ", " << y << ")\n";
      }
    }
  }
}
