#! /bin/csh
find . \( -name "*.c" -o -name "*.h" \) -exec wc -l {} \;
