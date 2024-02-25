include prorab.mk
include prorab-clang-format.mk
include prorab-license.mk

$(eval $(call prorab-config, config))

this_name := aptian

this_srcs := $(call prorab-src-dir, src)

this_ldlibs += -lpapki -lutki -lclargs -ltml

$(eval $(prorab-build-app))

this_src_dir := src
$(eval $(prorab-clang-format))

this_license_file := LICENSE
$(eval $(prorab-license))

$(eval $(prorab-include-subdirs))
