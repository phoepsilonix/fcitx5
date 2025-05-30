/*
 * SPDX-FileCopyrightText: 2016-2016 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#ifndef _FCITX_ADDONLOADER_P_H_
#define _FCITX_ADDONLOADER_P_H_

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <fcitx-utils/library.h>
#include <fcitx-utils/standardpaths.h>
#include <fcitx-utils/stringutils.h>
#include <fcitx/addonfactory.h>
#include <fcitx/addoninfo.h>
#include <fcitx/addoninstance.h>
#include <fcitx/addonloader.h>

namespace fcitx {

namespace {
constexpr char FCITX_ADDON_FACTORY_ENTRY[] = "fcitx_addon_factory_instance";
}

class SharedLibraryFactory {
public:
    SharedLibraryFactory(const AddonInfo &info, std::vector<Library> libraries)
        : libraries_(std::move(libraries)) {
        std::string v2Name = stringutils::concat(FCITX_ADDON_FACTORY_ENTRY, "_",
                                                 info.uniqueName());
        if (libraries_.empty()) {
            throw std::runtime_error("Got empty libraries.");
        }

        // Only resolve with last library.
        auto &library = libraries_.back();
        auto *funcPtr = library.resolve(v2Name.data());
        if (!funcPtr) {
            funcPtr = library.resolve(FCITX_ADDON_FACTORY_ENTRY);
        }
        if (!funcPtr) {
            throw std::runtime_error(library.error());
        }
        auto func = Library::toFunction<AddonFactory *()>(funcPtr);
        factory_ = func();
        if (!factory_) {
            throw std::runtime_error("Failed to get a factory");
        }
    }

    AddonFactory *factory() { return factory_; }

private:
    std::vector<Library> libraries_;
    AddonFactory *factory_;
};

class SharedLibraryLoader : public AddonLoader {
public:
    ~SharedLibraryLoader();
    AddonInstance *load(const AddonInfo &info, AddonManager *manager) override;

    std::string type() const override { return "SharedLibrary"; }

private:
    std::unordered_map<std::string, std::unique_ptr<SharedLibraryFactory>>
        registry_;
};

class StaticLibraryLoader : public AddonLoader {
public:
    StaticLibraryLoader(StaticAddonRegistry *registry_);

    AddonInstance *load(const AddonInfo &info, AddonManager *manager) override;

    std::string type() const override { return "StaticLibrary"; }

    StaticAddonRegistry *registry;
};
} // namespace fcitx

#endif // _FCITX_ADDONLOADER_P_H_
