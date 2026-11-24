#!/bin/bash
# Select music (quad): each player changes ONLY their own difficulty with their own Down key
# (Down = harder). P1 Down x1, P2 KP_2 x2, P3 'p' x3, P4 's' x4 from Beginner -> expect Easy/Medium/Hard/Challenge.
source "$(dirname "$0")/visual-lib.sh"
vt_setup; vt_stop
export ITG_TEST_AUTOSTART="quad;QuadGroup/QuadTest;Beginner,Beginner,Beginner,Beginner;ScreenSelectMusic"
rm -f "$REPO/Logs/log.txt"; vt_start; sleep 22
# NB: each key is sent twice (0.5 s apart) per step; under llvmpipe a lone press is not always seen by the game
press() { for i in $(seq $2); do xdotool key --delay 500 $1 $1; sleep 3; done; }
press Down 1; press KP_2 2; press p 3; press s 4
sleep 3; vt_shot diff-after; vt_stop
grep "ChangeSteps" "$REPO/Logs/log.txt"
