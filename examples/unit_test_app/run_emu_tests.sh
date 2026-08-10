#!/usr/bin/env bash
# Run the Unity test groups under esp-emu without pytest.
# Workaround for macOS, where pytest-embedded's forked serial reader hangs;
# CI (Linux) uses pytest_unit_test_app.py instead. Keep the group list in
# sync with that file.
#
# Usage: ./run_emu_tests.sh [group ...]   (default: all groups)
set -u

FIRMWARE=build/merged-binary.bin
[ -f "$FIRMWARE" ] || { echo "$FIRMWARE missing — run: idf.py set-target esp32c3 build merge-bin"; exit 1; }

TEST_GROUPS=("$@")
[ ${#TEST_GROUPS[@]} -eq 0 ] && TEST_GROUPS=(get_val get_val_type report update jsontlv cluster_lifecycle optional_clusters)
# ponytail: multi-stage reboot case (attribute persistence) not covered here, CI pytest runs it

rc=0
for group in "${TEST_GROUPS[@]}"; do
    log=$(mktemp)
    echo "=== [$group] ==="
    esp-emu --chip esp32c3 --firmware "$FIRMWARE" \
        --inject-on "Press ENTER to see the list of tests" --inject "[$group]\n" \
        --exit-on "Enter next test" --timeout 300s 2>/dev/null | tee "$log" | grep -E ":(PASS|FAIL)|Failures"
    if grep -q ":FAIL" "$log" || ! grep -q " 0 Failures" "$log"; then
        echo "*** [$group] FAILED (full log: $log)"
        rc=1
    else
        rm -f "$log"
    fi
done
exit $rc
