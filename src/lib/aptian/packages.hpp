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

#include <fsif/file.hpp>
#include <utki/string.hpp>

namespace aptian {

struct file_hashes {
	std::string md5;
	std::string sha1;
	std::string sha256;
	std::string sha512;

	bool operator==(const file_hashes& h) const
	{
		return //
			this->md5 == h.md5 && //
			this->sha1 == h.sha1 && //
			this->sha256 == h.sha256 && //
			this->sha512 == h.sha512;
	}
};

// TODO: refactor, avoid keeping string_views, use strings
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

	package(const package&);

	// no copy assignment, just in case. Was not needed so far.
	package& operator=(const package&) = delete;

	package(package&&) = default;
	package& operator=(package&&) = default;

	~package() = default;

	std::string to_string() const;

	std::string get_name() const;

	void append(std::string_view pool_path, size_t size, const file_hashes& hashes);

	bool operator==(const package& p) const
	{
		return this->control == p.control;
	}
};

static_assert(std::is_move_constructible_v<package>, "class package must be movable");
static_assert(std::is_move_assignable_v<package>, "class package must be movable");

std::vector<package> read_packages_file(const fsif::file& fi);

std::string to_string(utki::span<const package> packages);

} // namespace aptian
