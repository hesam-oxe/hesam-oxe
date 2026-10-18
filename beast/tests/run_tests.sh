#!/usr/bin/env bash
set -u
BIN=./behemoth
PASS=0; FAIL=0; TOTAL=0
for f in tests/cases/*.bst; do
    [ -e "$f" ] || continue
    TOTAL=$((TOTAL+1))
    base=$(basename "$f" .bst)
    exp="tests/cases/$base.expected"
    got=$("$BIN" run "$f" 2>&1)
    if [ -f "$exp" ]; then
        want=$(cat "$exp")
    else
        want=""
    fi
    if [ "$got" = "$want" ]; then
        PASS=$((PASS+1))
        echo "PASS: $base"
    else
        FAIL=$((FAIL+1))
        echo "FAIL: $base"
        echo "  expected: [$want]"
        echo "  got:      [$got]"
    fi
done
echo "----"
echo "Passed $PASS / $TOTAL (failed $FAIL)"
[ "$FAIL" -eq 0 ]
