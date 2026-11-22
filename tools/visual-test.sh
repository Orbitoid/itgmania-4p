#!/bin/bash
# One-shot visual smoke test of 4-player mode. Needs a build with -DITG_MAX_PLAYERS=4 (see docs/4-player.md).
# Usage: tools/visual-test.sh [seconds-into-song]   -> screenshots in $SHOTDIR (default /tmp/vt-shots)
set -e
source "$(dirname "$0")/visual-lib.sh"
vt_setup
vt_stop
[ -f "$REPO/Save/Preferences.ini" ] || { (cd "$REPO" && ./itgmania >/dev/null 2>&1 & sleep 15; pkill -x itgmania; sleep 3); }
vt_set_pref Theme "Simply Love 4P"
python3 "$REPO/tools/gen-4p-metrics.py" >/dev/null
export ITG_TEST_AUTOSTART="quad;QuadGroup/QuadTest;Beginner,Easy,Medium,Hard;ScreenGameplay"
rm -f "$REPO/Logs/log.txt"
vt_start
sleep "${1:-20}"
vt_shot quad-gameplay
vt_stop
# Optional: run the full suite (unit self-test + scripted per-player input smoke test)
if [ "$2" = "--all" ]; then
  "$REPO/tools/selftest.sh" && "$REPO/tools/input-test.sh" && "$REPO/tools/menu-join-test.sh"
fi
