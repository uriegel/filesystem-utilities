# filesystem-utils
File system utilities for Node.js
##
This project is newly created!

cp -rv arte_HD-20201222201458--Große_Erwartungen.mts ~ & progress -mp $! | ./mach.py

import sys

for line in sys.stdin:
    x = line.find("[2;9H")
    if x > -1:
        print(line[x+5:-1])