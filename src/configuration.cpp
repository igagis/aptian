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

#include "configuration.hpp"

#include <papki/fs_file.hpp>
#include <tml/crawler.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace aptian;

namespace {
constexpr std::string_view config_filename = "aptian.conf"sv;
} // namespace

configuration::configuration(std::string_view base_repo_dir) :
	conf([&]() {
		papki::fs_file file(utki::cat(base_repo_dir, config_filename));
		if (!file.exists()) {
			throw std::invalid_argument(utki::cat("could not open ", config_filename, " file. Non-aptian repo?"));
		}
		return tml::read(file);
	}())
{}

void configuration::create(std::string_view dir, std::string_view gpg)
{
	tml::forest cfg = {tml::tree("gpg"s, {tml::tree(gpg)})};

	papki::fs_file cfg_file(utki::cat(dir, config_filename));

	// TODO: check if file exists and only overwrite if --force

	tml::write(cfg, cfg_file);
}

std::string_view configuration::get_gpg()
{
	return tml::crawler(this->conf).to("gpg").in().get().value.string;
}
