#!/bin/bash
# Stage 3 input test: for each player, mash only that player's 4 keys during gameplay and assert that
# non-Miss judgments (TESTJUDGE lines in the log, enabled by ITG_TEST_LOG) only appear for that player.
# Needs a build with -DITG_MAX_PLAYERS=4.
source "$(dirname "$0")/visual-lib.sh"
vt_setup; vt_stop
vt_set_pref Theme "Simply Love 4P"
python3 "$REPO/tools/gen-4p-metrics.py" >/dev/null
export ITG_TEST_LOG=1
export ITG_TEST_AUTOSTART="quad;QuadGroup/QuadTest;Beginner,Easy,Medium,Hard;ScreenGameplay"
rm -f "$REPO/Logs/log.txt"
vt_start
KEYS=("q w e r" "t y u i" "o p bracketleft bracketright" "a s d f")
sleep 12
fail=0
for p in 0 1 2 3; do
  before=$(grep -c 'TESTJUDGE P[1-4] [^M]' "$REPO/Logs/log.txt")
  mark=$(wc -l < "$REPO/Logs/log.txt")
  end=$((SECONDS+14))
  while [ $SECONDS -lt $end ]; do KEYDELAY=20 vt_key ${KEYS[$p]}; done
  tail -n +$((mark+1)) "$REPO/Logs/log.txt" | grep 'TESTJUDGE' | grep -v ' Miss' > "$SHOTDIR/judge-p$((p+1)).txt"
  for q in 1 2 3 4; do
    n=$(grep -c "TESTJUDGE P$q " "$SHOTDIR/judge-p$((p+1)).txt")
    echo "pressing P$((p+1)) keys: hits registered for P$q = $n"
    if [ $q -eq $((p+1)) ]; then [ "$n" -gt 0 ] || { echo "FAIL: pressed player got no hits"; fail=1; }
    else [ "$n" -eq 0 ] || { echo "FAIL: P$q got hits from P$((p+1)) keys"; fail=1; }; fi
  done
  vt_shot "input-p$((p+1))"
done
vt_stop
[ $fail -eq 0 ] && echo "INPUT TEST PASS" || echo "INPUT TEST FAIL"
exit $fail
