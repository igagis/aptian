include prorab.mk
include prorab-clang-format.mk
include prorab-license.mk

this__root_dir_rel := ../../

$(eval $(call prorab-config, $(this__root_dir_rel)config))

this_name := aptian

this__libaptian := ../lib/out/$(c)/libaptian.a

this__version_cpp := $(this_out_dir)version.cpp

this_srcs := $(this__version_cpp) $(call prorab-src-dir, src)

this_ldlibs += $(this__libaptian)
this_ldlibs += -Wl,-Bstatic -ltml -lpapki -lclargs -lutki -Wl,-Bdynamic

$(eval $(prorab-build-app))

define this__rules
$(abspath $(d)$(this__version_cpp)): $(abspath $(d)$(this__root_dir_rel)debian/changelog)
$(.RECIPEPREFIX)$(a)mkdir -p $$(dir $$@)
$(.RECIPEPREFIX)$(a)(cd $(d)$(this__root_dir_rel); echo "extern const char* const program_version = \"$$$$(myci-deb-version.sh)\";" > $$@)
endef
$(eval $(this__rules))

############################
#### license and format ####

this_src_dir := src
$(eval $(prorab-clang-format))

this_license_file := $(this__root_dir_rel)LICENSE
$(eval $(prorab-license))
