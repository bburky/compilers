#!/usr/bin/sed -f

#sed -f symbols.sed 4\ left\ factoring.txt > final.txt

s/:=/ASSIGNOP/
s/\.\./ARRAY_RANGE/
s/:/COLON/
s/,/COMMA/
s/\[/LBRACKET/
s/(/LPAREN/
s/]/RBRACKET/
s/)/RPAREN/
s/;/SEMICOLON/
s/\./PERIOD/
s/+/ADDOP/
s/-/ADDOP/
s/ε/EPSILON/
