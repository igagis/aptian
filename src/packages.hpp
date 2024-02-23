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

#pragma once

#include <papki/file.hpp>

namespace aptian {

struct package {
	std::string package;
	std::string source;
	std::string version;
	std::string architecture;
	std::string maintainer;
	std::string installed_size;
	std::string depends;
	std::string suggests;
	std::string section;
	std::string priority;
	std::string description_brief;
	std::string description;
	std::string build_ids;
	std::string filename;
	std::string md5sum;
	std::string sha1;
	std::string sha256;
	std::string sha512;
	std::string size;
};

std::vector<package> read_packages_file(papki::file& fi);

} // namespace aptian
