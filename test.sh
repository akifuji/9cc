#!/bin/bash
try() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$expected expected, but got $actual"
    exit 1
  fi
}

try 0 '0;'
try 42 '42;'
try 21 '5+20-4;'
try 41 " 12 + 34 - 5;"
try 47 "5 + 6 * 7;"
try 15 "5 * (9 - 6);"
try 4 "(3 + 5) / 2;"
try 8 "-3 * 4 - (-20);"
try 1 "1 < 3;"
try 0 "3 < 1;"
try 1 "3 > 1;"
try 1 "1 <= 3;"
try 1 "3 <= 3;"
try 0 "3 <= 1;"
try 1 "3 >= 1;"
try 1 "3 >= 3;"
try 0 "1 >= 3;"
try 1 "3 == 3;"
try 0 "1 == 3;"
try 1 "1 != 3;"
try 0 "3 != 3;"
try 14 "a = 3; b = 5 * 6 - 8; a + b / 2;"
try 14 "a = 3; b = 5 * 6 - 8; return a + b / 2; return 5;"

echo OK
