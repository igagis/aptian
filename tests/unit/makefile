include prorab.mk
include prorab-test.mk

$(eval $(call prorab-config, ../../config))

this_name := tests

this_no_install := true

this_srcs := $(call prorab-src-dir, src)
this_srcs += $(filter-out %/main.cpp, $(call prorab-src-dir, ../../src))

this_ldlibs += -ltst -lpapki -lutki -lclargs -ltreeml

$(eval $(prorab-build-app))

this_test_cmd := $(prorab_this_name)
this_test_deps := $(prorab_this_name)
this_test_ld_path := $(prorab_space)
$(eval $(prorab-test))
