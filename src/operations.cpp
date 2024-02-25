/*
aptian - apt repository tool

Copyright (C) 2024  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.*/

/* ================ LICENSE END ================ */

#include "operations.hpp"

#include <papki/fs_file.hpp>
#include <treeml/tree.hpp>
#include <utki/debug.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace aptian;

namespace {
constexpr std::string_view config_filename = "aptian.conf"sv;
} // namespace

void aptian::init(std::string_view dir, std::string_view gpg)
{
	ASSERT(!dir.empty())
	ASSERT(!gpg.empty())

	papki::fs_file df(papki::as_dir(dir));

	if (!df.exists()) {
		std::stringstream ss;
		ss << "directory '" << dir << "' does not exist";
		throw std::invalid_argument(ss.str());
	}

	if (!df.list_dir().empty()) {
		std::stringstream ss;
		ss << "directory '" << dir << "' is not empty";
		throw std::invalid_argument(ss.str());
	}

	std::cout << "initialize APT repository" << std::endl;

	std::cout << "create 'dists/'" << std::endl;
	papki::fs_file(df.path() + "dists/").make_dir();

	std::cout << "create 'pool/'" << std::endl;
	papki::fs_file(df.path() + "pool/").make_dir();

	std::cout << "create " << config_filename << std::endl;
	{
		tml::forest cfg = {tml::tree("gpg"s, {tml::tree(gpg)})};

		papki::fs_file cfg_file(df.path() + std::string(config_filename));
		tml::write(cfg, cfg_file);
	}

	std::cout << "done" << std::endl;
}
