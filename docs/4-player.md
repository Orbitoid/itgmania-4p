# 4-player mode (dance-quad)

Four people, each on their own 4-panel pad (or keyboard column), all playing at once on one machine.
This is **not** Routine/Couples. Default builds are unchanged (2 players).

## Build
```
git submodule update --init --recursive --depth 1
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DITG_MAX_PLAYERS=4
cmake --build build -j$(nproc)
```
`-DITG_MAX_PLAYERS=2` (default) gives stock behaviour. With 4, `PLAYER_3/4`, `GameController_3/4` and the
`quad` style of `dance` exist; only the `quad` style lets more than two sides join.

## Theme
Use the overlay theme **Simply Love 4P** (`Themes/Simply Love 4P`, falls back to Simply Love). It lays each
player's field, score, life bar, name, difficulty meter, judgment (zoom 0.6) and stacked combo into its own
screen quarter. Regenerate P3/P4 metrics with `python3 tools/gen-4p-metrics.py`.

## Default keyboard map (left/down/up/right)
| Player | keys | Start |
|---|---|---|
| P1 | q w e r | Enter (arrows for menus) |
| P2 | t y u i | KP_Enter |
| P3 | o p [ ] | l |
| P4 | a s d f | g |

## Test tooling
- `tools/visual-test.sh` – Xvfb + llvmpipe run; boots straight into 4P gameplay on the generated test song
  (`tests/songs/QuadTest`, silent) with four difficulties and saves screenshots in `/tmp/vt-shots`.
- `tools/input-test.sh` – mashes one player's keys at a time, asserts only that player's judgments change.
- `tools/visual-combo.sh` – forced combos 12/123/1234/12345 + judgments for the layout check.
- `tools/selftest.sh` – in-engine unit tests (`ITG_TEST_SELFTEST=1`): player-count constants, style tables,
  input<->column round trips, per-player stage-stat aggregation.
- Env hooks (only active when set): `ITG_TEST_AUTOSTART`, `ITG_TEST_LOG`, `ITG_TEST_FAKECOMBO`, `ITG_TEST_SELFTEST`.

## Known limitations / UNVERIFIED
- Real dance pads and audio are UNVERIFIED (only keyboard under a dummy audio driver was tested).
- Menu screens other than gameplay/select-music/evaluation are not laid out for 3-4 players.
- Tug/Rave/battle modes remain 2-player only. Saved data formats are unchanged.
- `tools/diff-change-test.sh`: per-player difficulty change on select music (see 4P_NOTES run 7).
