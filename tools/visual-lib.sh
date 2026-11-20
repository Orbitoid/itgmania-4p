#!/bin/bash
# Shared helpers for ITGmania 4P visual testing under Xvfb. Source this file.
# Usage: source tools/visual-lib.sh; vt_setup; vt_start; vt_key Return; vt_shot name; vt_stop
REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
export DISPLAY=${DISPLAY:-:99}
export LIBGL_ALWAYS_SOFTWARE=1 GALLIUM_DRIVER=llvmpipe SDL_AUDIODRIVER=dummy
SHOTDIR=${SHOTDIR:-/tmp/vt-shots}
vt_setup() {
  mkdir -p "$SHOTDIR" "$REPO/Songs/QuadGroup"
  [ -f "$REPO/tests/songs/QuadTest/QuadTest.sm" ] || python3 "$REPO/tools/make-test-song.py"
  rm -rf "$REPO/Songs/QuadGroup/QuadTest"; cp -r "$REPO/tests/songs/QuadTest" "$REPO/Songs/QuadGroup/"
  touch "$REPO/Portable.ini"
  pgrep -x Xvfb >/dev/null || { Xvfb "$DISPLAY" -screen 0 ${VT_RES:-1280x720}x24 >/dev/null 2>&1 & sleep 2; }
}
vt_set_pref() { # vt_set_pref Key Value : edit every "Key=" line in Save/Preferences.ini (game must be stopped;
  # a dying game rewrites the file, so vt_stop waits). First run: start game once so the file exists.
  local f="$REPO/Save/Preferences.ini"; [ -f "$f" ] || return 1
  sed -i "s|^$1=.*|$1=$2|" "$f"
}
vt_start() { # launch game and wait for title screen
  vt_stop
  (cd "$REPO" && ./itgmania >"$SHOTDIR/stdout.log" 2>&1 &)
  for i in $(seq 1 90); do grep -q 'PushScreen: "ScreenTitleMenu"' "$REPO/Logs/log.txt" 2>/dev/null && break; sleep 1; done
  sleep 3
}
vt_stop() { pkill -x itgmania || true; sleep 3; }
vt_key() { xdotool key --delay ${KEYDELAY:-150} "$@"; }
vt_shot() { sleep ${SHOTWAIT:-1}; import -window root "$SHOTDIR/$1.png"; echo "shot: $SHOTDIR/$1.png"; }
vt_wait_screen() { for i in $(seq 1 ${2:-60}); do grep -q "PushScreen: \"$1\"" "$REPO/Logs/log.txt" && return 0; sleep 1; done; return 1; }
