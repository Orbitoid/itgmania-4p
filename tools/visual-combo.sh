#!/bin/bash
# 4P layout check: forced live combos 12/123/1234/12345 + W1 judgment in each quarter.
source "$(dirname "$0")/visual-lib.sh"
export ITG_TEST_FAKECOMBO="${ITG_TEST_FAKECOMBO:-12,123,1234,12345}"
exec "$(dirname "$0")/visual-test.sh" "${1:-30}"
