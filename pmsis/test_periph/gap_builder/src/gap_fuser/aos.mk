NAME := gap_fuser

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := gap_fuser with openocd
$(NAME)_SOURCES := gap_fuser.c

$(NAME)_COMPONENTS += osal_aos

GLOBAL_DEFINES += AOS_NO_WIFI

# unecomment to remove all uart refs (and thus all log)
#GLOBAL_CFLAGS += -DGAPOC_NO_UART

GLOBAL_INCLUDES += ./
