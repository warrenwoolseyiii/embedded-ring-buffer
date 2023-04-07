#!/bin/bash
uncrustify -c utilities/format/format.cfg --no-backup src/*.c
uncrustify -c utilities/format/format.cfg --no-backup src/*.h
uncrustify -c utilities/format/format.cfg --no-backup test/*.cc
dos2unix src/*.c
dos2unix src/*.h
dos2unix test/*.cc
git add src/*.c
git add src/*.h
git add test/*.cc
