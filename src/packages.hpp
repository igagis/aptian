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
#include <utki/string.hpp>

namespace aptian {

class package
{
	// use vector instead of std::string because
	// in case std:string has small-string-optimization then when moving the object
	// saved string_view's will be invalidated, we don't want that here
	std::vector<char> control;

public:
	struct control_fields {
		std::string_view package;
		std::string_view version;
		std::string_view filename;
	};

	control_fields fields;

	package(decltype(control) control);

	std::string_view to_string() const
	{
		return utki::make_string_view(this->control);
	}

	bool operator==(const package& p) const
	{
		return this->control == p.control;
	}
};

static_assert(std::is_move_constructible_v<package>, "class package must be movable");
static_assert(std::is_move_assignable_v<package>, "class package must be movable");

std::vector<package> read_packages_file(papki::file& fi);

} // namespace aptian
