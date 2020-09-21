#include <algorithm>
#include <random>

#include "minefield.hpp"
#include "texture_manager.hpp"
#include "types.hpp"


namespace
{
constexpr mswpr::sprite_type to_sprite(mswpr::cell_value value)
{
    const size_t index = mswpr::enum_to<size_t>(value);
    constexpr size_t empty_index = mswpr::enum_to<size_t>(mswpr::sprite_type::EMPTY_OPENED);
    return mswpr::to_enum<mswpr::sprite_type>(empty_index + index);
}

} // namespace

namespace mswpr
{
minefield::minefield(size_t width, size_t height, size_t bombs_cnt)
    : width_(width), height_(height), bombs_cnt_(bombs_cnt), field_(width_ * height_, {cell_value::EMPTY, cell_state::CLOSED})
{
    generate();
}

void minefield::handle_input()
{
}

void minefield::update()
{
}

void minefield::render(texture_manager& manager)
{
    for (size_t i = 0; i < height_; ++i)
    {
        for (size_t j = 0; j < width_; ++j)
        {
            const auto item = field_[i * width_ + j];
            const SDL_Rect dst_rect = {int(j * 32), int(i * 32), 32, 32};

            sprite_type sprite = sprite_type::EMPTY_CLOSED;

            switch (item.state)
            {
            case cell_state::CLOSED:
                sprite = sprite_type::EMPTY_CLOSED;
                break;
            case cell_state::FLAGGED:
                sprite = sprite_type::FLAG;
                break;
            case cell_state::OPENED:
                sprite = to_sprite(item.value);
                break;
            default:
                break;
            }

            manager.draw(sprite, dst_rect);
        }
    }
}

void minefield::generate()
{
    std::vector<size_t> coords(width_ * height_, 0);

    std::iota(coords.begin(), coords.end(), 0);

    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(coords.begin(), coords.end(), g);

    for (size_t i = 0; i < bombs_cnt_; ++i)
        field_[coords[i]].value = cell_value::BOMB;

    static constexpr std::array<int, 8> dir_x = {-1, 0, 1, -1, 1, -1, 0, 1};
    static constexpr std::array<int, 8> dir_y = {-1, -1, -1, 0, 0, 1, 1, 1};

    const int width_i = static_cast<int>(width_);
    const int height_i = static_cast<int>(height_);
    for (int y = 0; y < width_i; ++y)
    {
        for (int x = 0; x < height_i; ++x)
        {
            if (field_[y * width_ + x].value == cell_value::BOMB)
                continue;

            size_t cnt = 0;
            for (size_t i = 0; i < dir_x.size(); ++i)
            {
                int i_x = x - dir_x[i];
                int i_y = y - dir_y[i];
                if (i_x >= 0 && i_x < width_i && i_y >= 0 && i_y < height_i && field_[i_y * width_ + i_x].value == cell_value::BOMB)
                    ++cnt;
            }

            field_[y * width_ + x].value = to_enum<cell_value>(cnt);
        }
    }
}

void minefield::on_left_click(size_t mouse_x, size_t mouse_y)
{
    const size_t x = mouse_x / 32;
    const size_t y = mouse_y / 32;
    // SDL_Log("Left mouse click at (%ld, %ld)", x, y);
    if (x >= width_ || y >= height_)
        return;

    auto& elem = field_[y * width_ + x];
    if (elem.state == cell_state::CLOSED)
        elem.state = cell_state::OPENED;
}

void minefield::on_right_click(size_t mouse_x, size_t mouse_y)
{
    const size_t x = mouse_x / 32;
    const size_t y = mouse_y / 32;
    // SDL_Log("Right mouse click at (%ld, %ld)", x, y);
    if (x >= width_ || y >= height_)
        return;

    auto& elem = field_[y * width_ + x];
    if (elem.state == cell_state::CLOSED)
        elem.state = cell_state::FLAGGED;
    else if (elem.state == cell_state::FLAGGED)
        elem.state = cell_state::CLOSED;
}

} // namespace mswpr
