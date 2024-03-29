#ifndef MSWPR_PLAYING_STATE_HPP
#define MSWPR_PLAYING_STATE_HPP

#include "core/states/state_interface.hpp"

namespace mswpr
{
  class playing_state : public state_interface
  {
  public:
    using state_interface::state_interface;

    void on_left_face_click(bool is_released);
    void on_left_field_click(bool is_released, size_t x, size_t y);
  };
}  // namespace mswpr

#endif  // MSWPR_PLAYING_STATE_HPP
