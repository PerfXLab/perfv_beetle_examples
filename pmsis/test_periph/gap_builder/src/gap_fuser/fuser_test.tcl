# fuse byte 126 with a mark to check
proc fuse_byte_126 {} {
    array set fuse_array {
        0 0xaf
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
    gap_fuse_once 0x1c000190 0x1 1008 8 0x0 32
}

# read fuse byte 126 with a mark to check
proc read_byte_126 {} {
    array set fuse_array {
        0 0x77
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
    gap_fuse_once 0x1c000190 0x0 1008 8 0x0 32
    puts "read byte: $fuse_array(0)"
}

proc fuse_burn_one_test {} {
    gap_fuse_open 0x1c000190
    fuse_byte_126
    # now close the flasher
    gap_fuse_terminate 0x1c000190
}

proc fuse_read_one_test {} {
    gap_fuse_open 0x1c000190
    read_byte_126
    gap_fuse_terminate 0x1c000190
}


