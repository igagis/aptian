#include "operations.hpp"

#include <utki/debug.hpp>

#include <papki/fs_file.hpp>

using namespace aptian;

void aptian::init(std::string_view dir, std::string_view gpg){
    ASSERT(!dir.empty())
    ASSERT(!gpg.empty())

    papki::fs_file dir_file(dir);

    

}
