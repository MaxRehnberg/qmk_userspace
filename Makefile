.SILENT:

MAKEFLAGS += --no-print-directory

QMK_USERSPACE := $(patsubst %/,%,$(dir $(shell realpath "$(lastword $(MAKEFILE_LIST))")))
ifeq ($(QMK_USERSPACE),)
    QMK_USERSPACE := $(shell pwd)
endif

QMK_FIRMWARE_ROOT = $(shell qmk config -ro user.qmk_home | cut -d= -f2 | sed -e 's@^None$$@@g')
ifeq ($(QMK_FIRMWARE_ROOT),)
    $(error Cannot determine qmk_firmware location. `qmk config -ro user.qmk_home` is not set)
endif

.PHONY: elora elora-left elora-right ploopy

elora: elora-left elora-right

elora-left:
	qmk compile -kb splitkb/halcyon/elora/rev2 -km max_rehnberg -e HLC_ENCODER=1 -e TARGET=elora_left

elora-right:
	qmk compile -kb splitkb/halcyon/elora/rev2 -km max_rehnberg -e HLC_NONE=1 -e TARGET=elora_right

ploopy:
	qmk compile -kb ploopyco/nano_2/rev2_003 -km max_rehnberg

%:
	+$(MAKE) -C $(QMK_FIRMWARE_ROOT) $(MAKECMDGOALS) QMK_USERSPACE=$(QMK_USERSPACE)
