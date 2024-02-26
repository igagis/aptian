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
	std::vector<std::string> control;

public:
	struct control_fields {
		std::string_view package;
		std::string_view version;
		std::string_view architecture;

		std::string_view source;
		std::string_view filename;
	};

private:
	static control_fields parse(utki::span<const std::string> control);

public:
	control_fields fields;

	package(std::string_view control);

	std::string to_string() const;

	std::string get_name() const;

	void append_filename(std::string_view pool_path);

	bool operator==(const package& p) const
	{
		return this->control == p.control;
	}
};

static_assert(std::is_move_constructible_v<package>, "class package must be movable");
static_assert(std::is_move_assignable_v<package>, "class package must be movable");

std::vector<package> read_packages_file(papki::file& fi);

} // namespace aptian
