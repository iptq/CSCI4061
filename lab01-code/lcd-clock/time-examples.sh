#!/bin/bash

# Loop to produce various example times. clock_sim program must be
# built preceding use.

for t in 999 7777 121212; do
    echo '> clock_sim' $t
    ./clock_sim $t
    echo
    echo
done

