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

#include "packages.hpp"

using namespace aptian;

package::package(std::string control) :
	control(std::move(control))
{}

namespace {
class parser
{
	std::vector<char> buf;

	void feed(utki::span<const char> span)
	{
		for (char c : span) {
			if (c == '\n') {
			}
		}
	}

public:
	std::vector<package> packages;

	void parse(papki::file& fi)
	{
		papki::file::guard file_guard(fi, papki::file::mode::read);

		constexpr auto read_buffer_size = 0x1000;
		std::array<uint8_t, read_buffer_size> buf{};

		for (;;) {
			auto num_bytes_read = fi.read(buf);
			if (num_bytes_read == 0) {
				// EOF reached
				this->feed("\n\n");
				break;
			}

			auto span = utki::make_span(buf.data(), num_bytes_read);
			this->feed(utki::to_char(span));
		}
	}
};
} // namespace

std::vector<package> read_packages_file(papki::file& fi)
{
	parser p;
	p.parse(fi);
	return std::move(p.packages);
}
