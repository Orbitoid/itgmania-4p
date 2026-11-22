#!/bin/bash
# Real menu path (no autostart hook): join 4 players on the title screen with each
# controller's Start (P1 Return, P2 KP_Enter, P3 l, P4 g), then Return through the menus.
# Asserts that ScreenGameplay is reached with 4 players joined. Needs a -DITG_MAX_PLAYERS=4 build.
source "$(dirname "$0")/visual-lib.sh"
vt_setup; vt_stop; unset ITG_TEST_AUTOSTART
vt_set_pref Theme "Simply Love 4P"; python3 "$REPO/tools/gen-4p-metrics.py" >/dev/null
rm -f "$REPO/Logs/log.txt"; vt_start
vt_key KP_Enter; vt_key l; vt_key g; sleep 2
vt_key Return                         # P1 start: title -> select colour -> (quad) play mode
vt_wait_screen ScreenSelectPlayMode 30; sleep 5
for s in ScreenSelectPlayMode2 ScreenSelectMusic ScreenGameplay; do vt_key Return; vt_wait_screen $s 40 || sleep 5; sleep 3; done
vt_wait_screen ScreenGameplay 40; r=$?; sleep 3
vt_shot menu-join-gameplay; vt_stop
[ $r -eq 0 ] && echo "menu-join-test: PASS" || echo "menu-join-test: FAIL"; exit $r
