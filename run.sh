#!/bin/sh
# Runs every tests/cases/NAME.in through ./computorv2 and compares with NAME.out
# Usage: tests/run.sh            check all cases
#        tests/run.sh --update   (re)write the .out files from the current output — review the diff!
BIN=./computorv2
DIR=tests/cases
pass=0; fail=0

for input in "$DIR"/*.in; do
    name=$(basename "$input" .in)
    expected="$DIR/$name.out"
    actual=$("$BIN" < "$input" 2>&1)
    if [ "$1" = "--update" ]; then
        printf '%s\n' "$actual" > "$expected"
        echo "updated  $name"
        continue
    fi
    if [ ! -f "$expected" ]; then
        echo "MISSING  $name  (run tests/run.sh --update, then check the .out file by hand)"
        fail=$((fail + 1))
    elif printf '%s\n' "$actual" | diff -u "$expected" - > /tmp/computor_diff_$$; then
        echo "ok       $name"
        pass=$((pass + 1))
    else
        echo "FAIL     $name"
        cat /tmp/computor_diff_$$
        fail=$((fail + 1))
    fi
done
rm -f /tmp/computor_diff_$$
[ "$1" = "--update" ] && exit 0
echo "-- $pass passed, $fail failed"
[ "$fail" -eq 0 ]