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

#include <stdexcept>

#include <utki/string.hpp>

using namespace std::string_view_literals;

using namespace aptian;

namespace {
constexpr std::string_view package_entry = "Package: "sv;
constexpr std::string_view filename_entry = "Filename: "sv;
constexpr std::string_view version_entry = "Version: "sv;
} // namespace

package::package(decltype(control) control) :
	control(std::move(control)),
	fields([this]() {
		control_fields ret;

		utki::string_parser p(utki::make_string_view(this->control));

		for (;;) {
			auto line = p.read_chars_until('\n');

			if (line.starts_with(package_entry)) {
				ret.package = line.substr(package_entry.size());
				std::cout << "package entry found: " << ret.package << std::endl;
			}
			if (line.starts_with(filename_entry)) {
				ret.filename = line.substr(filename_entry.size());
			}
			if (line.starts_with(version_entry)) {
				ret.version = line.substr(version_entry.size());
			}

			if (p.empty()) {
				break;
			}

			p.read_char(); // read the '\n' char
		}

		if (ret.package.empty()) {
			throw std::invalid_argument("Package control file doesn't have 'Package:' entry");
		}
		if (ret.filename.empty()) {
			throw std::invalid_argument("Package control file doesn't have 'Filename:' entry");
		}
		if (ret.version.empty()) {
			throw std::invalid_argument("Package control file doesn't have 'Version:' entry");
		}

		return ret;
	}())
{}

namespace {
class parser
{
	bool line_start = true;

	std::vector<char> buf;

	void feed(utki::span<const char> span)
	{
		for (char c : span) {
			if (c == '\r') {
				continue;
			}
			if (c == '\n') {
				if (this->line_start) {
					// package parsed
					if (!this->buf.empty()) {
						this->packages.emplace_back(std::move(this->buf));
						ASSERT(this->buf.empty())

						// std::cout << "package read:" << '\n';
						// std::cout << this->packages.back().to_string();
					}
				} else {
					if (!this->buf.empty()) {
						this->buf.push_back(c);
					}
				}
				this->line_start = !this->line_start;
			} else {
				this->line_start = false;
				this->buf.push_back(c);
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
				this->feed("\n\n"sv);
				break;
			}

			auto span = utki::make_span(buf.data(), num_bytes_read);
			this->feed(utki::to_char(span));
		}
	}
};
} // namespace

std::vector<package> aptian::read_packages_file(papki::file& fi)
{
	parser p;
	p.parse(fi);
	return std::move(p.packages);
}
