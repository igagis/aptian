include prorab.mk
include prorab-clang-format.mk
include prorab-license.mk

$(eval $(call prorab-config, config))

this_name := aptian
this__aptainlib := out/$(c)/libaptianlib.a

this_srcs := src/main.cpp

this_ldlibs += $(this__aptainlib) -lpapki -lutki -lclargs -ltml

$(eval $(prorab-build-app))
$(eval $(call prorab-depend, $(prorab_this_name), $(this__aptainlib)))

# build lib #####################
$(eval $(prorab-clear-this-vars))

$(eval $(call prorab-config, config))

this_name := aptianlib

this_srcs := $(filter-out src/main.cpp, $(call prorab-src-dir, src))

this_static_lib_only := true
this_no_install := true

$(eval $(prorab-build-lib))

# license and format ############
$(eval $(prorab-clear-this-vars))

this_src_dir := src
$(eval $(prorab-clang-format))
this_license_file := LICENSE
$(eval $(prorab-license))

# subdirs #######################
$(eval $(prorab-include-subdirs))
