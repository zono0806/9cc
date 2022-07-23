#!/bin/bash
try(){
	expected="$1"
	input="$2"
	
	./9cc "$input" > tmp.s
	gcc -o tmp tmp.s
	./tmp
	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo "$input => $actual"
	else
		echo "$input => $expected expected, but got $actual"
		exit 1
	fi
}

try 0 0
try 2 2
try 20 '7+20-7'
try 10 '1+2+3+4'
try 10 '3*3+1'
try 36 '(4+8) * 3'
try 3  '(10 + 2 ) / (50 - 46) '
echo OK

