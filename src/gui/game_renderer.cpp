#include "gui/game_renderer.hpp"
#include "core/game_config.hpp"
#include "gui/game_version.hpp"

#include <SDL.h>

#include <concepts>

namespace
{
  bool is_inside(const SDL_Rect& rect, int x, int y)
  {
    return x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h;
  }

  template<mswpr::Enumeration To, To Base, std::integral Index>
  constexpr To to_sprite(Index index_integral)
  {
    const auto index = static_cast<size_t>(index_integral);
    constexpr auto BASE_INDEX = mswpr::enum_to<size_t>(Base);
    return mswpr::to_enum<To>(BASE_INDEX + index);
  }

  template<mswpr::Enumeration To, To Base, mswpr::Enumeration From>
  constexpr To to_sprite(From value)
  {
    const auto index = mswpr::enum_to<size_t>(value);
    return to_sprite<To, Base>(index);
  }

}  // namespace

mswpr::game_renderer::game_renderer(std::string_view title, size_t xpos, size_t ypos) : d_face_rect(), d_field_rect()
{
  SDL_Log("sdl2_minesweeper version: %s\n", mswpr::get_game_version());

  const Uint32 window_mode = 0;
  const size_t window_width = static_cast<size_t>(2) * cfg::BOARD_OFFSET_X + cfg::CELL_WIDTH * cfg::FIELD_WIDTH;
  const size_t window_height = cfg::BOARD_OFFSET_Y + cfg::BOARD_OFFSET_X + cfg::CELL_HEIGHT * cfg::FIELD_HEIGHT;

  d_window.reset(SDL_CreateWindow(
    title.data(), static_cast<int>(xpos), static_cast<int>(ypos), window_width, window_height, window_mode));
  if (!d_window)
  {
    SDL_Log("Unable to create SDL_window: %s", SDL_GetError());
    return;
  }

  SDL_Log("Window created!\n");

  d_renderer.reset(SDL_CreateRenderer(d_window.get(), -1, 0), mswpr::sdl_deleter{});
  if (!d_renderer)
  {
    SDL_Log("Unable to create SDL_Renderer: %s", SDL_GetError());
    return;
  }

  SDL_SetRenderDrawColor(d_renderer.get(), 190, 190, 190, 0);
  SDL_Log("Renderer created!\n");

  d_texture_manager.init(d_renderer, "assets/winxpskin.bmp");

  const int face_x = cfg::FIELD_WIDTH * cfg::CELL_WIDTH / 2 - cfg::FACE_WIDTH / 2 + cfg::BOARD_OFFSET_X;
  d_face_rect = { face_x, cfg::HUD_OFFSET_Y, cfg::FACE_WIDTH, cfg::FACE_HEIGHT };

  d_field_rect = {
    cfg::BOARD_OFFSET_X, cfg::BOARD_OFFSET_Y, cfg::CELL_WIDTH * cfg::FIELD_WIDTH, cfg::CELL_HEIGHT * cfg::FIELD_HEIGHT
  };
}

bool mswpr::game_renderer::is_inside_face(int mouse_x, int mouse_y)
{
  return is_inside(d_face_rect, mouse_x, mouse_y);
}

bool mswpr::game_renderer::is_inside_field(int mouse_x, int mouse_y)
{
  return is_inside(d_field_rect, mouse_x, mouse_y);
}

void mswpr::game_renderer::render(const mswpr::minefield& field,
                                  mswpr::face_type face,
                                  const mswpr::mines_counter& counter,
                                  const mswpr::game_timer& i_timer)
{
  SDL_RenderClear(d_renderer.get());

  draw_mines_counter(counter);
  draw_timer(i_timer);
  d_texture_manager.draw(face, d_face_rect);
  draw_field(field);

  SDL_RenderPresent(d_renderer.get());
}

void mswpr::game_renderer::draw_field(const mswpr::minefield& field)
{
  for (size_t y = 0; y < cfg::FIELD_HEIGHT; ++y)
  {
    for (size_t x = 0; x < cfg::FIELD_WIDTH; ++x)
    {
      const auto cell_x = static_cast<int>(x * cfg::CELL_WIDTH) + cfg::BOARD_OFFSET_X;
      const auto cell_y = static_cast<int>(y * cfg::CELL_HEIGHT) + cfg::BOARD_OFFSET_Y;
      const SDL_Rect dst_rect = { .x = cell_x, .y = cell_y, .w = cfg::CELL_WIDTH, .h = cfg::CELL_HEIGHT };

      sprite_type sprite = sprite_type::EMPTY_CLOSED;
      auto state = field.get_cell_state(x, y);

      switch (state)
      {
      case cell_state::CLOSED:
        sprite = sprite_type::EMPTY_CLOSED;
        break;
      case cell_state::FLAGGED:
        sprite = sprite_type::FLAG;
        break;
      case cell_state::OPENED:
        sprite = to_sprite<sprite_type, sprite_type::EMPTY_OPENED>(field.get_cell_value(x, y));
        break;
      case cell_state::DETONATED:
        sprite = sprite_type::BOMB_RED;
        break;
      case cell_state::NOT_FLAGGED_BOMB:
        sprite = sprite_type::BOMB_FAILED;
        break;
      default:
        break;
      }

      d_texture_manager.draw(sprite, dst_rect);
    }
  }
}

void mswpr::game_renderer::draw_mines_counter(const mswpr::mines_counter& counter)
{
  const SDL_Rect empty_display_rect = {
    .x = cfg::COUNTER_OFFSET_X, .y = cfg::HUD_OFFSET_Y, .w = cfg::COUNTER_WIDTH, .h = cfg::COUNTER_HEIGHT
  };
  d_texture_manager.draw(mswpr::display_digits_type::EMPTY_DISPLAY, empty_display_rect);

  const auto digits = counter.value_to_str();

  for (size_t i = 0; i < digits.size(); ++i)
  {
    const size_t rect_x = cfg::COUNTER_OFFSET_X + cfg::DIGIT_OFFSET + (cfg::DIGIT_OFFSET + cfg::DIGIT_WIDTH) * i;
    const SDL_Rect first_digit_rect = { .x = static_cast<int>(rect_x),
                                        .y = cfg::DIGIT_OFFSET + cfg::HUD_OFFSET_Y,
                                        .w = cfg::DIGIT_WIDTH,
                                        .h = cfg::DIGIT_HEIGHT };

    const auto sprite = digits[i] == '-' ? display_digits_type::MINUS
                                         : to_sprite<display_digits_type, display_digits_type::ZERO>(digits[i] - '0');
    d_texture_manager.draw(sprite, first_digit_rect);
  }
}

void mswpr::game_renderer::draw_timer(const mswpr::game_timer& i_timer)
{
  const size_t window_width = static_cast<size_t>(2) * cfg::BOARD_OFFSET_X + cfg::CELL_WIDTH * cfg::FIELD_WIDTH;
  const size_t x_timer = window_width - cfg::COUNTER_OFFSET_X - cfg::COUNTER_WIDTH - 2;
  const SDL_Rect empty_display_rect = {
    .x = static_cast<int>(x_timer), .y = cfg::HUD_OFFSET_Y, .w = cfg::COUNTER_WIDTH, .h = cfg::COUNTER_HEIGHT
  };
  d_texture_manager.draw(mswpr::display_digits_type::EMPTY_DISPLAY, empty_display_rect);

  const auto digits = i_timer.extract_digits_from_seconds();

  for (size_t i = 0; i < digits.size(); ++i)
  {
    const size_t rect_x = x_timer + cfg::DIGIT_OFFSET + (cfg::DIGIT_OFFSET + cfg::DIGIT_WIDTH) * i;
    const SDL_Rect first_digit_rect = { .x = static_cast<int>(rect_x),
                                        .y = cfg::DIGIT_OFFSET + cfg::HUD_OFFSET_Y,
                                        .w = cfg::DIGIT_WIDTH,
                                        .h = cfg::DIGIT_HEIGHT };

    const auto sprite = to_sprite<display_digits_type, display_digits_type::ZERO>(digits[i]);
    d_texture_manager.draw(sprite, first_digit_rect);
  }
}
