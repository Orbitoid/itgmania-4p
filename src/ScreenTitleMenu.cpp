#include "ScreenTitleMenu.h"

#include <string>

#include "AnnouncerManager.h"
#include "CodeDetector.h"
#include "CommonMetrics.h"
#include "GameLoop.h"
#include "GamePreferences.h"
#include "GameSoundManager.h"
#include "GameState.h"
#include "InputEventPlus.h"
#include "InputFilter.h"
#include "LocalizedString.h"
#include "MessageManager.h"
#include "Screen.h"
#include "ScreenManager.h"
#include "ScreenSelectMaster.h"
#include "ThemeManager.h"
#include "Difficulty.h"
#include "GameManager.h"
#include "RageLog.h"
#include "RageUtil.h"
#include "Song.h"
#include "SongManager.h"
#include "SongUtil.h"
#include "Steps.h"
#include "Style.h"
#include <cstdlib>
#include <set>
#include <vector>
#include "StageStats.h"
#include "PlayerStageStats.h"
#include "GameInput.h"

#define COIN_MODE_CHANGE_SCREEN \
  THEME->GetMetric(m_sName, "CoinModeChangeScreen")

REGISTER_SCREEN_CLASS(ScreenTitleMenu);
ScreenTitleMenu::ScreenTitleMenu() {
  /* XXX We really need two common calls:
   * 1) something run when exiting from gameplay (to do this reset),
   * and 2) something run when entering gameplay, to apply default options.
   * Having special cases in attract screens and the title menu to reset
   * things stinks ...
   * [aj] Well... Can't #2 be done in Lua nowadays? and #1 as well? (hi shake)
   */
  GAMESTATE->Reset();

  this->SubscribeToMessage(GamePreferences::m_CoinMode.GetName() + "Changed");
}

void ScreenTitleMenu::Init() {
  ScreenSelectMaster::Init();

  SOUND->PlayOnceFromAnnouncer("title menu game name");
}

void ScreenTitleMenu::Update(float fDeltaTime) {
  ScreenSelectMaster::Update(fDeltaTime);
  if (!m_bTestAutostartDone &&
      (std::getenv("ITG_TEST_AUTOSTART") != nullptr ||
       std::getenv("ITG_TEST_SELFTEST") != nullptr)) {
    m_fTestAutostartTimer += fDeltaTime;
    if (m_fTestAutostartTimer > 2.0f) {
      m_bTestAutostartDone = true;
      if (std::getenv("ITG_TEST_SELFTEST") != nullptr) {
        FourPlayerSelfTest();
      }
      TestAutostart();
    }
  }
}

/* In-engine unit tests for player-count dependent logic (style tables, input
 * mapping, stage-stat aggregation). Run with ITG_TEST_SELFTEST=1; results are
 * logged as "SELFTEST PASS/FAIL ..." lines, and the process exits afterwards
 * unless ITG_TEST_AUTOSTART is also set. */
static int g_iSelfTestFailures = 0;
#define ST_CHECK(cond)                                                      \
  do {                                                                      \
    if (cond) {                                                             \
      LOG->Info("SELFTEST PASS %s", #cond);                                 \
    } else {                                                                \
      LOG->Warn("SELFTEST FAIL %s (line %d)", #cond, __LINE__);             \
      ++g_iSelfTestFailures;                                                \
    }                                                                       \
  } while (0)

void ScreenTitleMenu::FourPlayerSelfTest() {
  g_iSelfTestFailures = 0;
  LOG->Info("SELFTEST begin, NUM_PLAYERS=%d NUM_GameController=%d",
      (int)NUM_PLAYERS, (int)NUM_GameController);
  ST_CHECK((int)NUM_PLAYERS == (int)NUM_GameController);
  ST_CHECK((int)NUM_PLAYERS == 2 || (int)NUM_PLAYERS == 4);
  for (int i = 0; i < (int)NUM_PLAYERS; ++i) {
    PlayerNumber pn = (PlayerNumber)i;
    ST_CHECK(OPPOSITE_PLAYER[i] != pn);
    ST_CHECK(OPPOSITE_PLAYER[OPPOSITE_PLAYER[i]] == pn);
  }
  // Style table: every style's input<->column mapping must round-trip.
  const Style* quad = GAMEMAN->GameAndStringToStyle(GAMESTATE->m_pCurGame, "quad");
  const Style* versus = GAMEMAN->GameAndStringToStyle(GAMESTATE->m_pCurGame, "versus");
  const Style* single = GAMEMAN->GameAndStringToStyle(GAMESTATE->m_pCurGame, "single");
  ST_CHECK(versus != nullptr && single != nullptr);
  if (versus != nullptr) {
    ST_CHECK(versus->m_StyleType == StyleType_TwoPlayersTwoSides);
  }
#if ITG_MAX_PLAYERS >= 4
  ST_CHECK(quad != nullptr);
  if (quad != nullptr) {
    ST_CHECK(quad->m_StyleType == StyleType_FourPlayersFourSides);
    ST_CHECK(quad->m_iColsPerPlayer == 4);
    ST_CHECK(single != nullptr && quad->m_StepsType == single->m_StepsType);
    std::set<int> controllers;
    for (int p = 0; p < 4; ++p) {
      std::set<GameController> pc;
      for (int c = 0; c < quad->m_iColsPerPlayer; ++c) {
        std::vector<GameInput> gi;
        quad->StyleInputToGameInput(c, (PlayerNumber)p, gi);
        ST_CHECK(!gi.empty());
        for (const GameInput& g : gi) {
          pc.insert(g.controller);
          // round trip: input maps back to the same column
          ST_CHECK(quad->GameInputToColumn(g) == c);
        }
      }
      // each player uses exactly one controller, distinct per player
      ST_CHECK(pc.size() == 1);
      if (pc.size() == 1) {
        controllers.insert((int)*pc.begin());
      }
    }
    ST_CHECK(controllers.size() == 4);
    // Columns are laid out on distinct tracks per player
    for (int p = 0; p < 4; ++p) {
      float lo, hi;
      quad->GetMinAndMaxColX((PlayerNumber)p, lo, hi);
      ST_CHECK(lo < hi);
    }
  }
#endif
  // Stage stats aggregation is independent per player.
  {
    StageStats a, b;
    for (int i = 0; i < (int)NUM_PLAYERS; ++i) {
      a.m_player[i].m_iTapNoteScores[TNS_W1] = 10 * (i + 1);
      b.m_player[i].m_iTapNoteScores[TNS_W1] = i + 1;
    }
    a.AddStats(b);
    for (int i = 0; i < (int)NUM_PLAYERS; ++i) {
      ST_CHECK(a.m_player[i].m_iTapNoteScores[TNS_W1] == 11 * (i + 1));
    }
  }
  LOG->Info("SELFTEST DONE failures=%d", g_iSelfTestFailures);
  if (std::getenv("ITG_TEST_AUTOSTART") == nullptr) {
    LOG->Flush();
    std::exit(g_iSelfTestFailures == 0 ? 0 : 1);
  }
}

/* Test-automation hook (only active when the ITG_TEST_AUTOSTART environment
 * variable is set). Format:
 *   ITG_TEST_AUTOSTART="<style>;<group/songdir>;<diff,diff,...>[;<screen>]"
 * e.g. "quad;QuadGroup/QuadTest;Beginner,Easy,Medium,Hard;ScreenGameplay".
 * Joins one player per listed difficulty, sets the style, song and per-player
 * steps, then jumps straight to the given screen. */
void ScreenTitleMenu::TestAutostart() {
  const char* env = std::getenv("ITG_TEST_AUTOSTART");
  if (env == nullptr || *env == '\0') {
    return;
  }
  std::vector<std::string> parts;
  split(env, ";", parts);
  if (parts.size() < 3) {
    LOG->Warn("ITG_TEST_AUTOSTART: need style;song;difficulties");
    return;
  }
  std::vector<std::string> diffs;
  split(parts[2], ",", diffs);
  const Style* style =
      GAMEMAN->GameAndStringToStyle(GAMESTATE->m_pCurGame, parts[0]);
  Song* song = SONGMAN->FindSong(parts[1]);
  if (style == nullptr || song == nullptr) {
    LOG->Warn(
        "ITG_TEST_AUTOSTART: style '%s' or song '%s' not found",
        parts[0].c_str(), parts[1].c_str());
    return;
  }
  for (size_t i = 0; i < diffs.size() && i < (size_t)NUM_PLAYERS; ++i) {
    GAMESTATE->JoinPlayer((PlayerNumber)i);
  }
  GAMESTATE->m_PlayMode.Set(PLAY_MODE_REGULAR);
  GAMESTATE->SetCurrentStyle(style, PLAYER_INVALID);
  GAMESTATE->m_pCurSong.Set(song);
  for (size_t i = 0; i < diffs.size() && i < (size_t)NUM_PLAYERS; ++i) {
    PlayerNumber pn = (PlayerNumber)i;
    Difficulty dc = StringToDifficulty(diffs[i]);
    Steps* steps = SongUtil::GetOneSteps(song, style->m_StepsType, dc);
    if (steps == nullptr) {
      LOG->Warn("ITG_TEST_AUTOSTART: no %s steps", diffs[i].c_str());
      continue;
    }
    GAMESTATE->m_PreferredDifficulty[pn].Set(dc);
    GAMESTATE->m_pCurSteps[pn].Set(steps);
  }
  LOG->Info(
      "ITG_TEST_AUTOSTART: %d player(s) joined, style %s",
      GAMESTATE->GetNumSidesJoined(), style->m_szName);
  SCREENMAN->SetNewScreen(parts.size() > 3 ? parts[3] : "ScreenGameplay");
}

static LocalizedString THEME_("ScreenTitleMenu", "Theme");
static LocalizedString ANNOUNCER_("ScreenTitleMenu", "Announcer");
bool ScreenTitleMenu::Input(const InputEventPlus& input) {
#if defined(DEBUG)
  LOG->Trace(
      "ScreenTitleMenu::Input( %d-%d )", input.DeviceI.device,
      input.DeviceI.button);  // debugging gameport joystick problem
#endif

  if (m_In.IsTransitioning() || m_Cancel.IsTransitioning()) { /* not m_Out */
    return false;
  }

  bool bHandled = false;
  if (input.type == IET_FIRST_PRESS) {
    // detect codes
    if (CodeDetector::EnteredCode(input.GameI.controller, CODE_OPERATOR_MENU)) {
      SCREENMAN->SetNewScreen(CommonMetrics::OPERATOR_MENU_SCREEN);
      return true;
    }
    // Theme changing pad codes are marked as deprecated in _fallback's
    // metrics.ini, remove them after SM5? -Kyz
    if (CodeDetector::EnteredCode(input.GameI.controller, CODE_NEXT_THEME) ||
        CodeDetector::EnteredCode(input.GameI.controller, CODE_NEXT_THEME2)) {
      GameLoop::ChangeTheme(THEME->GetNextSelectableTheme());
      bHandled = true;
    }
    if (CodeDetector::EnteredCode(
            input.GameI.controller, CODE_NEXT_ANNOUNCER) ||
        CodeDetector::EnteredCode(
            input.GameI.controller, CODE_NEXT_ANNOUNCER2)) {
      ANNOUNCER->NextAnnouncer();
      std::string sName = ANNOUNCER->GetCurAnnouncerName();
      if (sName == "") {
        sName = "(none)";
      }
      SCREENMAN->SystemMessage(ANNOUNCER_.GetValue() + ": " + sName);
      SCREENMAN->SetNewScreen(m_sName);
      bHandled = true;
    }
  }

  return ScreenSelectMaster::Input(input) || bHandled;
}

void ScreenTitleMenu::HandleMessage(const Message& msg) {
  if (msg == GamePreferences::m_CoinMode.GetName() + "Changed") {
    /* If the CoinMode was changed, we need to reload this screen
     * so that the right m_aGameCommands will show */
    SCREENMAN->SetNewScreen(COIN_MODE_CHANGE_SCREEN);
  }

  ScreenSelectMaster::HandleMessage(msg);
}

/*
 * (c) 2001-2004 Chris Danford
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
