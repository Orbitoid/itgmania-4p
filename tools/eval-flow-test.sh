#!/bin/bash
# Plays the quad test song to the end (nothing pressed -> all miss), then follows the post-gameplay screens.
# PASS if ScreenEvaluationStage is reached and ScreenNameEntry*/ScreenEvaluationSummary are skipped in quad.
source "$(dirname "$0")/visual-lib.sh"
vt_setup; vt_stop
vt_set_pref Theme "Simply Love 4P"
python3 "$REPO/tools/gen-4p-metrics.py" >/dev/null
export ITG_TEST_AUTOSTART="quad;QuadGroup/QuadTest;Beginner,Easy,Medium,Hard;ScreenGameplay"
rm -f "$REPO/Logs/log.txt"
vt_start
vt_wait_screen ScreenEvaluationStage ${1:-300} || { echo "FAIL: evaluation never reached"; vt_stop; exit 1; }
sleep 6; vt_shot eval-flow-evaluation
for i in 1 2 3 4 5 6; do KEYDELAY=500 vt_key Return Return; sleep 4; done
vt_shot eval-flow-after
grep -o 'PushScreen: "[A-Za-z]*"' "$REPO/Logs/log.txt" | tail -8
bad=$(grep -c 'PushScreen: "\(ScreenNameEntry[A-Za-z]*\|ScreenEvaluationSummary\)"' "$REPO/Logs/log.txt")
vt_stop
[ "$bad" -eq 0 ] && echo "eval-flow-test: PASS" || { echo "eval-flow-test: FAIL (2P-only screens reached)"; exit 1; }
