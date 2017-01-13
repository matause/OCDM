#!/bin/bash

P=texpak

find -type f \( -name "*.bim" -o -name "*.bam" \) -print0 | xargs -0 -n1 rm
find -name 0.png -print0 | xargs -0 -n1 dirname | xargs -n1 $P
