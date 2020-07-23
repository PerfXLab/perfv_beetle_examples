proc fuse_boot_from_flash {} {
    array set fuse_array {
        0 0x10
        1 0
        2 0
        3 0
        4 0
        5 0
        6 0
        7 0
        8 0
        9 0
        10 0
        11 0
        12 0
        13 0
        14 0
        15 0
        16 0
        17 0
        18 0
        19 0
        20 0
        21 0
        22 0
        23 0
        24 0
        25 0
        26 0
        27 0
        28 0
        29 0
        30 0
        31 0x0
    }
    gap_fuse_once 0x1c000190 0x1 0 8 0xf 32
}

proc fuse_flash_hyperflash {} {
    array set fuse_array {
        0 0x1
        1 0
        2 0
        3 0
        4 0
        5 0
        6 0
        7 0
        8 0
        9 0
        10 0
        11 0
        12 0
        13 0
        14 0
        15 0
        16 0
        17 0
        18 0
        19 0
        20 0
        21 0
        22 0
        23 0
        24 0
        25 0
        26 0
        27 0
        28 0
        29 0
        30 0
        31 0x0
    }
    gap_fuse_once 0x1c000190 0x1 296 8 0xf 32
}

proc fuse_hyperflash_boot {} {
    gap_fuse_open 0x1c000190
    fuse_boot_from_flash
    fuse_flash_hyperflash
    # now close the flasher
    gap_fuse_terminate 0x1c000190
}

proc fuse_spiflash_boot {} {
    gap_fuse_open 0x1c000190
    fuse_boot_from_flash
    # now close the flasher
    gap_fuse_terminate 0x1c000190
}
