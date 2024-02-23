#include <tst/set.hpp>
#include <tst/check.hpp>

#include <papki/span_file.hpp>

#include "../../../src/packages.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace{
const tst::set set("packages", [](tst::suite& suite){ // NOLINT
    suite.add("parse_2_packages", [](){
        std::vector<aptian::package> expected{
            utki::make_vector(utki::make_span("Package: libantigrain0-dbg" "\n"
            "Source: libantigrain" "\n"
            "Version: 2.8.7" "\n"
            "Architecture: amd64" "\n"
            "Maintainer: Ivan Gagis <igagis@gmail.com>" "\n"
            "Installed-Size: 189" "\n"
            "Depends: libantigrain0 (= 2.8.7)" "\n"
            "Section: debug" "\n"
            "Priority: extra" "\n"
            "Description: debugging symbols for libantigrain0 package." "\n"
            "Build-Ids: 1b017cdcff1ccdaeea6fc28e73fb94fe0f2abc1a" "\n"
            "Filename: pool/focal/main/liba/libantigrain/libantigrain0-dbg_2.8.7_amd64.deb" "\n"
            "MD5sum: 1ce93b01faf88feb4603ab26d14ac9d8" "\n"
            "SHA1: c5537f5947cfdc8c91139cb35a7f6c35f56bffc1" "\n"
            "SHA256: b1f9a1227804979de77875a2d65096cd1e92cbacb409d31d8478384d6fecc9e2" "\n"
            "SHA512: cf98478027c4e054ee47f381cad2125454227eb68a9d20400da77a41989f59441d3d9b41216cd4dbe1eb337801b5996c1d897c8a7bf6f615a13979cb998b493b" "\n"
            "Size: 167312" "\n"sv)),

            utki::make_vector(utki::make_span("Package: libaumiks-dev" "\n"
            "Source: libaumiks" "\n"
            "Version: 0.3.30" "\n"
            "Architecture: all" "\n"
            "Maintainer: Ivan Gagis <igagis@gmail.com>" "\n"
            "Installed-Size: 192" "\n"
            "Depends: libaumiks0 (= 0.3.30), libaumiks0-dbg (= 0.3.30), libpapki-dev, libaudout-dev" "\n"
            "Suggests: libaumiks-doc" "\n"
            "Section: devel" "\n"
            "Priority: extra" "\n"
            "Description: Audio mixing and playback engine in C++." "\n"
            " libaumiks is a library written in C++ which allows easy audio playback." "\n"
            " It has sound mixer for playing several sounds simultaneously." "\n"
            "Filename: pool/focal/main/liba/libaumiks/libaumiks-dev_0.3.30_all.deb" "\n"
            "MD5sum: 0082880df1082463f3ad3ff5ce5ebaf8" "\n"
            "SHA1: b845f92bd21696d50651896310bc84eea205053d" "\n"
            "SHA256: c729d04ea5b67837481f915033c89cc873dfefd423b5f7def1e9e1b099678de0" "\n"
            "SHA512: 1d23aebc473582fc62780a756778b5dd21c85f2574f79ca689b8a74cd01d67395a935b73edd424c8b34926529877766d816a98d0e66fdee89764c95555dae75b" "\n"
            "Size: 27548" "\n"sv)),
        };

        std::stringstream ss;
        for(const auto& p : expected){
            ss << p.to_string() << "\n";
        }

        auto expected_string = ss.str();

        // std::cout << "exp str = " << expected_string << std::endl;

        papki::span_file fi(expected_string);
        auto packages = aptian::read_packages_file(fi);

        // std::cout << "packages[0] = " << packages[1].to_string() << std::endl;
        // std::cout << "expected[0] = " << expected[1].to_string() << std::endl;

        tst::check_eq(packages.size(), expected.size());
        tst::check_eq(packages[0].to_string(), expected[0].to_string());
        tst::check_eq(packages[1].to_string(), expected[1].to_string());

        tst::check_eq(packages[0].fields.package, "libantigrain0-dbg"sv);
        tst::check_eq(packages[0].fields.version, "2.8.7"sv);
        tst::check_eq(packages[0].fields.filename, "pool/focal/main/liba/libantigrain/libantigrain0-dbg_2.8.7_amd64.deb"sv);

        tst::check_eq(packages[1].fields.package, "libaumiks-dev"sv);
        tst::check_eq(packages[1].fields.version, "0.3.30"sv);
        tst::check_eq(packages[1].fields.filename, "pool/focal/main/liba/libaumiks/libaumiks-dev_0.3.30_all.deb"sv);
    });
});
}
