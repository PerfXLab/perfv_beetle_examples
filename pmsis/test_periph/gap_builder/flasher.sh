#!/bin/bash

# Copyright Greenwaves Technologies 2019
# Licence: Apache
# Script to wrap openocd flash

GAP_BUILDER="$PWD"
BOARD_NAME=$1
FLASH_IMG_NAME=$2

# We always generate raw, so this will exist, might just not be used in the end
IMG_SIZE=$(stat -c %s "$FLASH_IMG_NAME.raw")

BOARD_SCRIPT="board/$BOARD_NAME.cfg"

openocd -c "script $BOARD_SCRIPT; gap_flash $FLASH_IMG_NAME $IMG_SIZE $GAP_BUILDER $BOARD_NAME"
