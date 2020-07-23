#!/bin/bash

# Copyright Greenwaves Technologies 2019
# Licence: Apache
# Script to wrap openocd flash

GAP_BUILDER="$PWD"

echo "Which board you want to use: 1-GAPUINO 2-GAPOC_A 3-GAPOC_A_QPIF 4-GAP8_QPIF_BREAKOUT 5-GAP_PERFXLAB [1]: "
read chip
chip=${chip:-1}
if [ $chip -eq 1  ]; then
    BOARD_NAME=gapuino
elif [ $chip -eq 2  ]; then
    BOARD_NAME=gapoc_a
elif [ $chip -eq 3  ]; then
    BOARD_NAME=gapoc_a_qpif
elif [ $chip -eq 4  ]; then
    BOARD_NAME=gap8_qpif_breakout
elif [ $chip -eq 5  ]; then
    BOARD_NAME=gap_perfxlab
else
	echo "Unknown board: $chip"
fi
echo The target board you have chosen is : $BOARD_NAME

BOARD_SCRIPT="board/$BOARD_NAME.cfg"

echo "openocd -c script $BOARD_SCRIPT; gap_boot_fuse $GAP_BUILDER"
openocd -c "script $BOARD_SCRIPT; gap_boot_fuse $GAP_BUILDER"
