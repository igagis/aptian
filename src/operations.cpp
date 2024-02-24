#include "operations.hpp"

#include <papki/fs_file.hpp>
#include <utki/debug.hpp>

using namespace std::string_literals;

using namespace aptian;

void aptian::init(std::string_view dir, std::string_view gpg)
{
	ASSERT(!dir.empty())
	ASSERT(!gpg.empty())

	papki::fs_file df([&]() -> std::string {
        if(dir.back() == '/'){
            return std::string(dir);
        }
        std::stringstream ss;
        ss << dir << '/';
        return ss.str();
    }());

	if (!df.exists()) {
        std::stringstream ss;
        ss << "directory '" << dir  << "' does not exist";
		throw std::invalid_argument(ss.str());
	}

    if(!df.list_dir().empty()){
        std::stringstream ss;
        ss << "directory '" << dir << "' is not empty";
        throw std::invalid_argument(ss.str());
    }

    // TODO:
}
