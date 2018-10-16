
default: main
.PHONY: default

include $(PUREC_DIR)/mk/target.mk

SHELL := /bin/bash
srcs := src test
deps := bower_components/purescript-*/src
$(eval $(call purs_mk_target,main,Test.Main,$(srcs),$(deps)))
