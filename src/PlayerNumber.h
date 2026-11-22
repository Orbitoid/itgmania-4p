/* PlayerNumber - A simple type representing a player. */

#ifndef PlayerNumber_H
#define PlayerNumber_H

#include <string>

#include "EnumHelper.h"

// Maximum number of players compiled into the engine (CMake option
// ITG_MAX_PLAYERS, 2 or 4). Default is 2, which keeps the stock behaviour.
#ifndef ITG_MAX_PLAYERS
#define ITG_MAX_PLAYERS 2
#endif
static_assert(ITG_MAX_PLAYERS == 2 || ITG_MAX_PLAYERS == 4,
              "ITG_MAX_PLAYERS must be 2 or 4");

// Player number stuff
enum PlayerNumber {
  PLAYER_1 = 0,
  PLAYER_2,
#if ITG_MAX_PLAYERS >= 4
  PLAYER_3,
  PLAYER_4,
#endif
  NUM_PlayerNumber,  // leave this at the end
  PlayerNumber_Invalid
};
/** Number of per-player sections written to simfile caches (radar values, tech
 * counts). Fixed at 2 so cache/simfile data is identical for every build
 * (ITG_MAX_PLAYERS) and never overruns arrays when read by another build. */
const int NUM_FILE_PLAYERS = 2;
const PlayerNumber NUM_PLAYERS = NUM_PlayerNumber;
const PlayerNumber PLAYER_INVALID = PlayerNumber_Invalid;
const std::string& PlayerNumberToString(PlayerNumber pn);
const std::string& PlayerNumberToLocalizedString(PlayerNumber pn);
LuaDeclareType(PlayerNumber);
/** @brief A foreach loop to handle the different players. */
#define FOREACH_PlayerNumber(pn) FOREACH_ENUM(PlayerNumber, pn)

#if ITG_MAX_PLAYERS >= 4
const PlayerNumber OPPOSITE_PLAYER[NUM_PLAYERS] = {PLAYER_2, PLAYER_1, PLAYER_4,
                                                   PLAYER_3};
#else
const PlayerNumber OPPOSITE_PLAYER[NUM_PLAYERS] = {PLAYER_2, PLAYER_1};
#endif

enum MultiPlayer {
  MultiPlayer_P1 = 0,
  MultiPlayer_P2,
  MultiPlayer_P3,
  MultiPlayer_P4,
  MultiPlayer_P5,
  MultiPlayer_P6,
  MultiPlayer_P7,
  MultiPlayer_P8,
  MultiPlayer_P9,
  MultiPlayer_P10,
  MultiPlayer_P11,
  MultiPlayer_P12,
  MultiPlayer_P13,
  MultiPlayer_P14,
  MultiPlayer_P15,
  MultiPlayer_P16,
  MultiPlayer_P17,
  MultiPlayer_P18,
  MultiPlayer_P19,
  MultiPlayer_P20,
  MultiPlayer_P21,
  MultiPlayer_P22,
  MultiPlayer_P23,
  MultiPlayer_P24,
  MultiPlayer_P25,
  MultiPlayer_P26,
  MultiPlayer_P27,
  MultiPlayer_P28,
  MultiPlayer_P29,
  MultiPlayer_P30,
  MultiPlayer_P31,
  MultiPlayer_P32,
  NUM_MultiPlayer,  // leave this at the end
  MultiPlayer_Invalid
};
const std::string& MultiPlayerToString(MultiPlayer mp);
const std::string& MultiPlayerToLocalizedString(MultiPlayer mp);
LuaDeclareType(MultiPlayer);
/** @brief A foreach loop to handle the different Players in MultiPlayer. */
#define FOREACH_MultiPlayer(pn) FOREACH_ENUM(MultiPlayer, pn)

#endif

/*
 * (c) 2001-2004 Chris Danford, Chris Gomez
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
