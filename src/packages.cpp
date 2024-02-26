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
constexpr std::string_view source_entry = "Source: "sv;
constexpr std::string_view md5sum_entry = "MD5sum: "sv;
constexpr std::string_view sha1_entry = "SHA1: "sv;
constexpr std::string_view sha256_entry = "SHA256: "sv;
constexpr std::string_view sha512_entry = "SHA512: "sv;
constexpr std::string_view size_entry = "Size: "sv;
constexpr std::string_view architecture_entry = "Architecture: "sv;
} // namespace

package::package(std::string_view control) :
	control(utki::split(control, '\n')),
	fields(parse(this->control))
{}

package::package(const package& p) :
	control(p.control)
{
	this->fields = parse(this->control);
}

package::control_fields package::parse(utki::span<const std::string> control)
{
	control_fields ret;

	for (std::string_view line : control) {
		if (line.starts_with(package_entry)) {
			ret.package = line.substr(package_entry.size());
			// std::cout << "package entry found: " << ret.package << std::endl;
		}
		if (line.starts_with(filename_entry)) {
			ret.filename = line.substr(filename_entry.size());
			// std::cout << "filename entry found: " << ret.filename << std::endl;
		}
		if (line.starts_with(version_entry)) {
			ret.version = line.substr(version_entry.size());
			// std::cout << "version entry found: " << ret.version << std::endl;
		}
		if (line.starts_with(source_entry)) {
			ret.source = line.substr(source_entry.size());
			// std::cout << "source entry found: " << ret.source << std::endl;
		}
		if (line.starts_with(architecture_entry)) {
			ret.architecture = line.substr(architecture_entry.size());
			// std::cout << "source entry found: " << ret.source << std::endl;
		}
	}

	if (ret.package.empty()) {
		throw std::invalid_argument("Package control file doesn't have 'Package:' entry");
	}
	if (ret.version.empty()) {
		throw std::invalid_argument("Package control file doesn't have 'Version:' entry");
	}
	if (ret.architecture.empty()) {
		throw std::invalid_argument("Package control file doesn't have 'Architecture:' entry");
	}

	return ret;
}

std::string package::to_string() const
{
	std::stringstream ss;

	for (const auto& line : this->control) {
		ss << line << '\n';
	}

	return ss.str();
}

std::string package::get_name() const
{
	if (!this->fields.source.empty()) {
		return std::string(this->fields.source);
	}
	ASSERT(!this->fields.package.empty())
	return std::string(this->fields.package);
}

void package::append_filename(std::string_view pool_path)
{
	ASSERT(!pool_path.empty())
	if (!this->fields.filename.empty()) {
		throw std::logic_error("could not append filename, the package control already has Filename field");
	}

	this->control.push_back(utki::cat(filename_entry, pool_path));
	this->fields.filename = std::string_view(this->control.back()).substr(filename_entry.size());
}

void package::append_md5(std::string_view md5)
{
	this->control.push_back(utki::cat(md5sum_entry, md5));
}

void package::append_sha1(std::string_view sha1)
{
	this->control.push_back(utki::cat(sha1_entry, sha1));
}

void package::append_sha256(std::string_view sha256)
{
	this->control.push_back(utki::cat(sha256_entry, sha256));
}

void package::append_sha512(std::string_view sha512)
{
	this->control.push_back(utki::cat(sha512_entry, sha512));
}

void package::append_size(size_t size)
{
	this->control.push_back(utki::cat(size_entry, size));
}

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
						this->packages.emplace_back(utki::make_string_view(this->buf));
						this->buf.clear();
						// std::cout << "package read:" << '\n';
						// std::cout << this->packages.back().to_string();
					}
					ASSERT(this->buf.empty())
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

std::string aptian::to_string(utki::span<const package> packages)
{
	std::stringstream ss;
	for (const auto& p : packages) {
		ss << p.to_string() << "\n";
	}
	return ss.str();
}
