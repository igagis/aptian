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

#include "cli.hpp"

#include <clargs/parser.hpp>

#include "operations.hpp"

using namespace aptian;

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace {
void handle_init_command(utki::span<const char* const> args)
{
	std::string dir;
	std::string gpg;

	clargs::parser p;

	p.add('d', "dir"s, "path to base directory for repository structure. Must be empty.", [&](std::string_view v) {
		dir = v;
	});

	p.add('k', "gpg", "GPG key to use for signing", [&](std::string_view v) {
		gpg = v;
	});

	p.parse(args);

	if (dir.empty()) {
		throw std::invalid_argument("--dir argument is not given");
	}

	if (gpg.empty()) {
		throw std::invalid_argument("--gpg argument is not given");
	}

	init(dir, gpg);
}
} // namespace

namespace {
void handle_command(std::string_view command, utki::span<const char* const> args)
{
	if (command == "init") {
		handle_init_command(args);
	}
}
} // namespace

namespace {

constexpr std::string_view program_name = "aptian"sv;

void print_commands_list()
{
	std::cout << "Commands:" << "\n";
	std::cout << "  init  initialize APT repository directory structure" << "\n";
	std::cout << "  add   add debian packages to an APT repository" << "\n";
}

void print_help(std::string_view args_description)
{
	std::cout << program_name << " - Debian APT repository management tool" << "\n";
	std::cout << "\n";
	std::cout << "Usage:" << "\n";
	std::cout << "  " << program_name << " [options] <command> [command arguments]" << "\n";
	std::cout << "\n";
	print_commands_list();
	std::cout << "\n";
	std::cout << "Options:" << "\n";
	std::cout << args_description << std::endl;
}
} // namespace

int aptian::handle_cli(int argc, const char** argv)
{
	bool no_action = true;

	clargs::parser p;

	p.add( //
		"help",
		"print help message",
		[&]() {
			print_help(p.description());
			p.stop();
			no_action = false;
		}
	);

	p.add([&](std::string_view command, utki::span<const char* const> cmd_args) {
		handle_command(command, cmd_args);
		no_action = false;
	});

	p.parse(argc, argv);

	if (no_action) {
		std::cout << "ERROR: no command given. Run with --help to see available commands." << std::endl;
		return 1;
	}

	return 0;
}
