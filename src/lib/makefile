include prorab.mk
include prorab-clang-format.mk
include prorab-license.mk

this__root_dir_rel := ../../

$(eval $(call prorab-config, $(this__root_dir_rel)config))

this_name := aptian

this_srcs := $(call prorab-src-dir, .)

this_static_lib_only := true
this_no_install := true

$(eval $(prorab-build-lib))

############################
#### license and format ####

this_src_dir := $(this_name)
$(eval $(prorab-clang-format))

this_license_file := $(this__root_dir_rel)LICENSE
$(eval $(prorab-license))
