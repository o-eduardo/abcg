#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include <bitset>

enum class Input { Right, Left, Down, Up, Fire };
enum class State { StartClick, StopClick };

struct GameData {
  State m_state{State::StopClick};

};

#endif