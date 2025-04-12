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
#include <papki/util.hpp>
#include <utki/string.hpp>

#include "operations.hpp"

using namespace aptian;

using namespace std::string_literals;
using namespace std::string_view_literals;

extern const char* const program_version;

namespace {
constexpr std::string_view program_name = "aptian"sv;
} // namespace

namespace {
void handle_init_command(utki::span<std::string_view> args)
{
	bool help = false;
	std::string dir;
	std::string gpg;

	clargs::parser p;

	p.add( //
		"help"s,
		"show 'init' command help information"s,
		[&]() {
			help = true;
			p.stop();
		}
	);

	p.add( //
		'd',
		"dir"s,
		"path to base directory for creating repository structure, must be empty"s,
		[&](std::string_view v) {
			dir = v;
		}
	);

	p.add( //
		'k',
		"gpg"s,
		"GPG key to use for signing"s,
		[&](std::string_view v) {
			gpg = v;
		}
	);

	p.parse(args);

	if (help) {
		std::cout << "initialize APT repository" << '\n';
		std::cout << '\n';
		std::cout << "Usage:" << '\n';
		std::cout << "  " << program_name << " init --dir=<base-repo-dir> --gpg=<gpg-key>" << '\n';
		std::cout << '\n';
		std::cout << "Options:" << '\n';
		std::cout << p.description();
		std::cout << '\n';
		std::cout << "Example:" << '\n';
		std::cout << "  " << program_name << " init --dir=/var/www/repo/ --gpg=mailbox@somemail.com" << '\n';
		std::cout << std::endl;
		return;
	}

	if (dir.empty()) {
		throw std::invalid_argument("--dir argument is not given");
	}
	if (gpg.empty()) {
		throw std::invalid_argument("--gpg argument is not given");
	}

	init(papki::as_dir(dir), gpg);
}
} // namespace

namespace {
void handle_add_command(utki::span<std::string_view> args)
{
	bool help = false;
	std::string dir;
	std::string dist;
	std::string comp;

	clargs::parser p;

	p.add( //
		"help"s,
		"show 'add' command help information"s,
		[&]() {
			help = true;
			p.stop();
		}
	);

	p.add( //
		'd',
		"dir"s,
		"path to base directory of APT repository"s,
		[&](std::string_view v) {
			dir = v;
		}
	);

	p.add( //
		"dist"s,
		"name of *nix distribution, e.g. 'bookworm', 'jammy'"s,
		[&](std::string_view v) {
			dist = v;
		}
	);

	p.add( //
		"comp"s,
		"name of APT component, e.g. 'main'"s,
		[&](std::string_view v) {
			comp = v;
		}
	);

	auto packages = p.parse(args);

	if (help) {
		std::cout << "add packages to APT repository" << '\n';
		std::cout << '\n';
		std::cout << "Usage:" << '\n';
		std::cout
			<< "  " << program_name
			<< " add --dir=<repo-base-dir> --dist=<distribution> --comp=<component> package1.deb [package2.deb ...]"
			<< '\n';
		std::cout << '\n';
		std::cout << "Options:" << '\n';
		std::cout << p.description();
		std::cout << '\n';
		std::cout << "Example:" << '\n';
		std::cout << "  " << program_name
				  << " add --dir=/var/www/repo/ --dist=bookworm --comp=main my-package_1.0.0_amd64.deb" << '\n';
		std::cout << std::endl;
		return;
	}

	if (dir.empty()) {
		throw std::invalid_argument("--dir argument is not given");
	}
	if (dist.empty()) {
		throw std::invalid_argument("--dist argument is not given");
	}
	if (comp.empty()) {
		throw std::invalid_argument("--comp argument is not given");
	}
	if (packages.empty()) {
		throw std::invalid_argument("no package files given");
	}

	add( //
		papki::as_dir(dir),
		dist,
		comp,
		packages
	);
}
} // namespace

namespace {
void handle_command(std::string_view command, utki::span<std::string_view> args)
{
	if (command == "init") {
		handle_init_command(args);
	} else if (command == "add") {
		handle_add_command(args);
	} else {
		std::stringstream ss;
		ss << "invalid commad given: " << command;
		throw std::invalid_argument(ss.str());
	}
}
} // namespace

namespace {

void print_commands_list()
{
	std::cout << "Commands:" << "\n";
	std::cout << "  init  initialize APT repository directory structure" << "\n";
	std::cout << "  add   add debian packages to an APT repository" << "\n";
}

void print_help(std::string_view args_description)
{
	std::cout << program_name << ' ' << program_version << " - Debian APT repository management tool" << "\n";
	std::cout << "\n";
	std::cout << "Usage:" << "\n";
	std::cout << "  " << program_name << " [options] <command> [--help] [command arguments]" << "\n";
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

	p.add( //
		"version",
		"print program version",
		[&]() {
			std::cout << program_name << ' ' << program_version << std::endl;
			p.stop();
			no_action = false;
		}
	);

	p.add([&](std::string_view command, utki::span<std::string_view> cmd_args) {
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
