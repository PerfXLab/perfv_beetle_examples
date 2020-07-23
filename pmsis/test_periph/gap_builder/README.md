##################################################
Build a flash image:

gap8_flash_image_builder.py --verbose --flash-boot-binary=<BINARY_PATH> --raw=<FLASH_IMG_NAME>.raw --flash-type=spi

BINARY_PATH: the path and app binary you want to put into flash, and boot with.
FLASH_IMG_NAME: the path and name of the flash image you want to make


##################################################
Flash a binary in Flash

./flasher.sh BOARD_NAME FLASH_IMG_NAME

BOARD_NAME: the name of board, which can be found in board folder.
FLASH_IMG_NAME: the name of flash image. (same as in the previous step)



#################################################
Program the efuse for boot from flash

./fuser


