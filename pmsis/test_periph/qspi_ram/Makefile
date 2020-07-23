# User Test
#------------------------------------

APP              = test
APP_SRCS        += test_qspi_ram.c
APP_INC         +=
APP_CFLAGS      +=

ifeq ($(ASYNC), 1)
APP_CFLAGS      += -DASYNC=1
endif

PMSIS_OS ?= freertos

include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk
