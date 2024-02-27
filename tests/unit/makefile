include prorab.mk
include prorab-test.mk

$(eval $(call prorab-config, ../../config))

this_name := tests

this_no_install := true

this_srcs := $(call prorab-src-dir, src)

this__aptianlib := ../../out/$(c)/libaptianlib.a

this_ldlibs += $(this__aptianlib) -ltst -lpapki -lutki -lclargs -ltml

$(eval $(prorab-build-app))

$(eval $(call prorab-depend, $(prorab_this_name), $(this__aptianlib)))

this_test_cmd := $(prorab_this_name)
this_test_deps := $(prorab_this_name)
this_test_ld_path := $(prorab_space)
$(eval $(prorab-test))

$(eval $(call prorab-include, ../../makefile))