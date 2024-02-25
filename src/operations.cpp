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
#include <tml/tree.hpp>
#include <utki/debug.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace aptian;

/*

APT repository directory structure:

dists
	<dists>
		<comps>
			binary-<archs>
				Packages
				Packages.gz
				Release
			[binary-source]
				Packages
				Packages.gz
				Release
		InRelease
		Release
		Release.gpg
pool
	<dists>
		<comps>
			<prefix>
				<package-source-name>
					<package-files>
aptian.conf
keyring.gpg
pubkey.gpg

*/

namespace {
constexpr std::string_view config_filename = "aptian.conf"sv;
constexpr std::string_view dists_subdir = "dists/"sv;
constexpr std::string_view pool_subdir = "pool/"sv;
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

	std::cout << "create '" << dists_subdir << "'" << std::endl;
	papki::fs_file((std::stringstream() << df.path() << dists_subdir).str()).make_dir();

	std::cout << "create '" << pool_subdir << "'" << std::endl;
	papki::fs_file((std::stringstream() << df.path() << pool_subdir).str()).make_dir();

	std::cout << "create " << config_filename << std::endl;
	{
		tml::forest cfg = {tml::tree("gpg"s, {tml::tree(gpg)})};

		papki::fs_file cfg_file(df.path() + std::string(config_filename));
		tml::write(cfg, cfg_file);
	}

	std::cout << "done" << std::endl;
}

void aptian::add(
	std::string_view dir,
	std::string_view dist,
	std::string_view comp,
	utki::span<const std::string> packages
)
{
	ASSERT(!dir.empty())
	ASSERT(!dist.empty())
	ASSERT(!comp.empty())
	ASSERT(!packages.empty())

	auto dist_dir = (std::stringstream() << dir << dists_subdir << dist).str();
	auto comp_dir = (std::stringstream() << dist_dir << comp).str();
	auto pool_dir = (std::stringstream() << dir << pool_subdir << dist << comp).str();

	std::cout << "dist_dir = " << dist_dir << std::endl;
	std::cout << "comp_dir = " << comp_dir << std::endl;
	std::cout << "pool_dir = " << pool_dir << std::endl;
}
