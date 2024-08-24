include prorab.mk
include prorab-test.mk

$(eval $(call prorab-config, ../../config))

this_name := tests

this_no_install := true

this_srcs := $(call prorab-src-dir, src)

this__libaptian := ../../src/lib/out/$(c)/libaptian.a

this_cxxflags += -I ../../src/lib/
this_ldlibs += $(this__libaptian) -ltst -lpapki -lutki -lclargs -ltml

$(eval $(prorab-build-app))

$(eval $(call prorab-depend, $(prorab_this_name), $(this__libaptian)))

this_test_cmd := $(prorab_this_name)
this_test_deps := $(prorab_this_name)
this_test_ld_path := $(prorab_space)
$(eval $(prorab-test))

$(eval $(call prorab-include, ../../src/makefile))