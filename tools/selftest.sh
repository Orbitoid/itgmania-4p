#!/bin/bash
# In-engine unit tests (stage 5). Needs a build with -DITG_MAX_PLAYERS=4. Exit 0 on success.
source "$(dirname "$0")/visual-lib.sh"
vt_setup; vt_stop
vt_set_pref Theme "Simply Love 4P"
python3 "$REPO/tools/gen-4p-metrics.py" >/dev/null
rm -f "$REPO/Logs/log.txt"
export ITG_TEST_SELFTEST=1
(cd "$REPO" && ./itgmania >"$SHOTDIR/stdout.log" 2>&1 &)
# the engine exits by itself when the self-test finishes (the final DONE log line is not always flushed)
sleep 3; for i in $(seq 1 90); do pgrep -x itgmania >/dev/null || break; sleep 1; done
grep 'SELFTEST' "$REPO/Logs/log.txt" | sed 's/^.*SELFTEST/SELFTEST/' | grep -v PASS
pass=$(grep -c 'SELFTEST PASS' "$REPO/Logs/log.txt"); fails=$(grep -c 'SELFTEST FAIL' "$REPO/Logs/log.txt")
echo "selftest: $pass passed, $fails failed"
[ "$fails" -eq 0 ] && [ "$pass" -gt 0 ]
r=$?; vt_stop; exit $r
