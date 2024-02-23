#include <tst/set.hpp>
#include <tst/check.hpp>

#include "../../../src/packages.hpp"

using namespace std::string_literals;

namespace{
const tst::set set("packages", [](tst::suite& suite){
    suite.add("parse_2_packages", [](){
        std::vector<aptian::package> expected{
            {
                .package = "libantigrain0-dbg"s,
                .source = "libantigrain"s,
                .version = "2.8.7"s,
                .architecture = "amd64"s,
                .maintainer = "Ivan Gagis <igagis@gmail.com>"s,
                .installed_size = "189"s,
                .depends = "libantigrain0 (= 2.8.7)"s,
                .section = "debug"s,
                .priority = "extra"s,
                .description_brief = "debugging symbols for libantigrain0 package."s,
                .build_ids = "1b017cdcff1ccdaeea6fc28e73fb94fe0f2abc1a"s,
                .filename = "pool/focal/main/liba/libantigrain/libantigrain0-dbg_2.8.7_amd64.deb"s,
                .md5sum = "1ce93b01faf88feb4603ab26d14ac9d8"s,
                .sha1 = "c5537f5947cfdc8c91139cb35a7f6c35f56bffc1"s,
                .sha256 = "b1f9a1227804979de77875a2d65096cd1e92cbacb409d31d8478384d6fecc9e2"s,
                .sha512 = "cf98478027c4e054ee47f381cad2125454227eb68a9d20400da77a41989f59441d3d9b41216cd4dbe1eb337801b5996c1d897c8a7bf6f615a13979cb998b493b"s,
                .size = "167312"s
            },
            {
                .package = "libaumiks-dev"s,
                .source = "libaumiks"s,
                .version = "0.3.30"s,
                .architecture = "all"s,
                .maintainer = "Ivan Gagis <igagis@gmail.com>"s,
                .installed_size = "192"s,
                .depends = "libaumiks0 (= 0.3.30), libaumiks0-dbg (= 0.3.30), libpapki-dev, libaudout-dev"s,
                .suggests = "libaumiks-doc"s,
                .section = "devel"s,
                .priority = "extra"s,
                .description_brief = "Audio mixing and playback engine in C++."s,
                .description = "libaumiks is a library written in C++ which allows easy audio playback.\n"
                  "It has sound mixer for playing several sounds simultaneously."s,
                .filename = "pool/focal/main/liba/libaumiks/libaumiks-dev_0.3.30_all.deb"s,
                .md5sum = "0082880df1082463f3ad3ff5ce5ebaf8"s,
                .sha1 = "b845f92bd21696d50651896310bc84eea205053d"s,
                .sha256 = "c729d04ea5b67837481f915033c89cc873dfefd423b5f7def1e9e1b099678de0"s,
                .sha512 = "1d23aebc473582fc62780a756778b5dd21c85f2574f79ca689b8a74cd01d67395a935b73edd424c8b34926529877766d816a98d0e66fdee89764c95555dae75b"s,
                .size = "27548"s
            }
        };
    });
});
}