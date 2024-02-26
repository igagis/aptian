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

#include <filesystem>
#include <map>

#include <papki/fs_file.hpp>
#include <tml/tree.hpp>
#include <utki/debug.hpp>
#include <utki/string.hpp>

#include "packages.hpp"

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
constexpr std::string_view tmp_subdir = "tmp/"sv;
constexpr std::string_view lib_prefix = "lib"sv;
constexpr std::string_view binary_prefix = "binary-"sv;
constexpr std::string_view packages_filename = "Packages"sv;
} // namespace

namespace {
bool is_aptian_repo(std::string_view dir)
{
	return papki::fs_file(utki::concat(dir, dists_subdir)).exists() &&
		papki::fs_file(utki::concat(dir, pool_subdir)).exists() &&
		papki::fs_file(utki::concat(dir, config_filename)).exists();
}
} // namespace

namespace {
std::string apt_pool_prefix(std::string_view package_name)
{
	ASSERT(!package_name.empty())

	constexpr auto lib_prefix_size = lib_prefix.size() + 1;
	if (package_name.starts_with(lib_prefix) && package_name.size() >= lib_prefix_size) {
		return papki::as_dir(package_name.substr(0, lib_prefix_size));
	}
	return papki::as_dir(package_name.substr(0, 1));
}
} // namespace

void aptian::init( //
	std::string_view dir,
	std::string_view gpg
)
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
	papki::fs_file(utki::concat(df.path(), dists_subdir)).make_dir();

	std::cout << "create '" << pool_subdir << "'" << std::endl;
	papki::fs_file(utki::concat(df.path(), pool_subdir)).make_dir();

	std::cout << "create " << config_filename << std::endl;
	{
		tml::forest cfg = {tml::tree("gpg"s, {tml::tree(gpg)})};

		papki::fs_file cfg_file(df.path() + std::string(config_filename));
		tml::write(cfg, cfg_file);
	}

	std::cout << "done" << std::endl;
}

// TODO: use utki::trim
template <typename element_type>
std::basic_string_view<element_type> trim_front(std::basic_string_view<element_type> s)
{
	return s.substr( //
		std::distance( //
			s.begin(),
			std::find_if( //
				s.begin(),
				s.end(),
				[](auto c) {
					return !std::isspace(c);
				}
			)
		)
	);
}

template <typename element_type>
std::basic_string_view<element_type> trim_back(std::basic_string_view<element_type> s)
{
	return s.substr( //
		0,
		std::distance( //
			s.begin(),
			std::find_if( //
				s.rbegin(),
				s.rend(),
				[](auto c) {
					return !std::isspace(c);
				}
			).base()
		)
	);
}

template <typename element_type>
std::basic_string_view<element_type> trim(std::basic_string_view<element_type> s)
{
	return trim_front(trim_back(s));
}

namespace {
struct repo_dirs {
	std::string dist;
	std::string comp; // directory under dist_dir
	std::string pool;
	std::string tmp;
};

struct unadded_package {
	std::string file_path;
	package pkg;
};

std::vector<unadded_package> prepare_control_info(utki::span<const std::string> package_paths, const repo_dirs& dirs)
{
	std::vector<unadded_package> unadded_packages;

	for (const auto& pkg_path : package_paths) {
		auto filename = papki::not_dir(pkg_path);
		auto suffix = papki::suffix(filename);
		if (suffix != "deb") {
			std::cout << "unsupported package suffix: ." << suffix << std::endl;
			std::cout << "  skipping: " << filename << std::endl;
			continue;
		}

		papki::fs_file tmp_dir_file(dirs.tmp);
		if (tmp_dir_file.exists()) {
			std::filesystem::remove_all(tmp_dir_file.path());
		}
		tmp_dir_file.make_dir();

		// extract control information from deb package
		{
			if (std::system(utki::concat("dpkg-deb --control ", pkg_path, " ", dirs.tmp).c_str()) != 0) {
				throw std::runtime_error(utki::concat("could not extract control information from ", filename));
			}
		}

		auto control_file_path = utki::concat(dirs.tmp, "control");
		auto control = papki::fs_file(control_file_path).load();

		package pkg(trim(utki::make_string_view(control)));

		// calculate hash sums
		{
			auto md5_path = utki::concat(dirs.tmp, "md5");
			auto sha1_path = utki::concat(dirs.tmp, "sha1");
			auto sha256_path = utki::concat(dirs.tmp, "sha256");
			auto sha512_path = utki::concat(dirs.tmp, "sha512");
			if (std::system(utki::concat("md5sum ", pkg_path, " | cut -d\" \" -f1 > ", md5_path).c_str()) != 0) {
				throw std::runtime_error( //
					utki::concat("could not calculcate md5 hash sum of the package ", filename)
				);
			}
			if (std::system(utki::concat("sha1sum ", pkg_path, " | cut -d\" \" -f1 > ", sha1_path).c_str()) != 0) {
				throw std::runtime_error( //
					utki::concat("could not calculcate sha1 hash sum of the package ", filename)
				);
			}
			if (std::system(utki::concat("sha256sum ", pkg_path, " | cut -d\" \" -f1 > ", sha256_path).c_str()) != 0) {
				throw std::runtime_error( //
					utki::concat("could not calculcate sha256 hash sum of the package ", filename)
				);
			}
			if (std::system(utki::concat("sha512sum ", pkg_path, " | cut -d\" \" -f1 > ", sha512_path).c_str()) != 0) {
				throw std::runtime_error( //
					utki::concat("could not calculcate sha512 hash sum of the package ", filename)
				);
			}
			pkg.append_md5(trim(utki::make_string_view(papki::fs_file(md5_path).load())));
			pkg.append_sha1(trim(utki::make_string_view(papki::fs_file(sha1_path).load())));
			pkg.append_sha256(trim(utki::make_string_view(papki::fs_file(sha256_path).load())));
			pkg.append_sha512(trim(utki::make_string_view(papki::fs_file(sha512_path).load())));
		}

		pkg.append_size(papki::fs_file(pkg_path).size());
		std::cout << "control = " << std::endl;
		std::cout << pkg.to_string();

		auto pkg_name = pkg.get_name();

		auto pkg_pool_dir = utki::concat(dirs.pool, apt_pool_prefix(pkg_name), papki::as_dir(pkg_name));

		auto pkg_pool_path = utki::concat(pkg_pool_dir, filename);

		pkg.append_filename(pkg_pool_path);

		unadded_packages.push_back({//
									.file_path = pkg_path,
									.pkg = std::move(pkg)
		});
	}

	return unadded_packages;
}

void add_packages_to_pool(utki::span<const unadded_package> packages, const repo_dirs& dirs)
{
	// check if any of the package files are already exist in the pool
	for (const auto& p : packages) {
		if (papki::fs_file(p.pkg.fields.filename).exists()) {
			std::stringstream ss;
			ss << "package " << p.pkg.fields.filename << " already exists in the pool";
			throw std::invalid_argument(ss.str());
		}
	}

	// add packages to the pool
	for (const auto& p : packages) {
		const auto& filename = p.pkg.fields.filename;
		std::filesystem::create_directories(papki::dir(filename));

		std::cout << "add " << filename << " to the pool" << std::endl;
		std::filesystem::copy(p.file_path, filename);
	}
}

class architectures
{
	std::map<std::string, std::vector<package>, std::less<>> archs;

	std::string comp_dir;

	auto& load_arch(std::string_view arch)
	{
		auto packages_path = utki::concat(this->comp_dir, binary_prefix, arch, '/', packages_filename);

		papki::fs_file file(packages_path);

		auto packages = [&]() {
			if (file.exists()) {
				return aptian::read_packages_file(file);
			}
			return decltype(archs)::value_type::second_type();
		}();

		auto res = this->archs.insert(decltype(archs)::value_type(arch, std::move(packages)));
		ASSERT(res.second)

		return res.first->second;
	}

	auto& get_arch(std::string_view arch)
	{
		auto i = this->archs.find(arch);
		if (i == this->archs.end()) {
			return this->load_arch(arch);
		}
		return i->second;
	}

public:
	architectures(std::string comp_dir) :
		comp_dir(std::move(comp_dir))
	{}

	void add(package pkg)
	{
		const auto& arch = pkg.fields.architecture;

		ASSERT(!arch.empty())

		// TODO: handle 'all' arch

		auto& packages = this->get_arch(arch);

		packages.push_back(std::move(pkg));
	}

	void write_packages()
	{
		// TODO: does Packages file have to be sorted by package name?
		// TODO: write
	}
};

void add_to_architectures(std::vector<unadded_package> packages, const repo_dirs& dirs)
{
	architectures archs(dirs.comp);

	for (auto& p : packages) {
		archs.add(std::move(p.pkg));
	}

	archs.write_packages();
}

} // namespace

void aptian::add(
	std::string_view dir,
	std::string_view dist,
	std::string_view comp,
	utki::span<const std::string> package_paths
)
{
	ASSERT(!dir.empty())
	ASSERT(!dist.empty())
	ASSERT(!comp.empty())
	ASSERT(!package_paths.empty())

	if (!is_aptian_repo(dir)) {
		std::stringstream ss;
		ss << "given --dir argument is not an aptian repository";
		throw std::invalid_argument(ss.str());
	}

	repo_dirs dirs = {
		.dist = utki::concat(dir, dists_subdir, dist),
		.comp = utki::concat(dirs.dist, comp),
		.pool = utki::concat(dir, pool_subdir, dist, comp),
		.tmp = utki::concat(dir, tmp_subdir)
	};

	std::cout << "dirs.dist = " << dirs.dist << std::endl;
	std::cout << "dirs.comp = " << dirs.comp << std::endl;
	std::cout << "dirs.pool = " << dirs.pool << std::endl;

	auto unadded_packages = prepare_control_info(package_paths, dirs);

	add_packages_to_pool(unadded_packages, dirs);

	add_to_architectures(std::move(unadded_packages), dirs);

	// TODO:
}
