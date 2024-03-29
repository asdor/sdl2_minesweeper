#include "gui/game_engine.hpp"

int main()
{
  mswpr::game_engine engine("mswpr", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

  while (engine.running())
  {
    engine.handle_input();
    engine.update();
    engine.render();

    engine.limit_fps();
  }

  return 0;
}
