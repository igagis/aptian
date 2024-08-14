include prorab.mk
include prorab-clang-format.mk
include prorab-license.mk

$(eval $(call prorab-config, config))

this_name := aptian

this__aptian_lib := out/$(c)/libaptianlib.a

this__version_cpp := $(this_out_dir)version.cpp

this_srcs := src/main.cpp $(this__version_cpp)

this_ldlibs += $(this__aptian_lib)
this_ldlibs += -Wl,-Bstatic -ltml -lpapki -lclargs -lutki -Wl,-Bdynamic

$(eval $(prorab-build-app))
$(eval $(call prorab-depend, $(prorab_this_name), $(this__aptian_lib)))

define this__rules
$(d)$(this__version_cpp): $(d)debian/changelog
$(.RECIPEPREFIX)$(a)echo "extern const char* const program_version = \"$$$$(myci-deb-version.sh)\";" > $(this__version_cpp)
endef
$(eval $(this__rules))

###################
#### build lib ####
$(eval $(prorab-clear-this-vars))

$(eval $(call prorab-config, config))

this_name := aptianlib

this_srcs := $(filter-out src/main.cpp, $(call prorab-src-dir, src))

this_static_lib_only := true
this_no_install := true

$(eval $(prorab-build-lib))

############################
#### license and format ####
$(eval $(prorab-clear-this-vars))

this_src_dir := src
$(eval $(prorab-clang-format))
this_license_file := LICENSE
$(eval $(prorab-license))

#################
#### subdirs ####
$(eval $(prorab-include-subdirs))
