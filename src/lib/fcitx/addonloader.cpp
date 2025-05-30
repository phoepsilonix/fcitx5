/*
 * SPDX-FileCopyrightText: 2016-2016 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */

#include "addonloader.h"
#include <exception>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "fcitx-utils/flags.h"
#include "fcitx-utils/library.h"
#include "fcitx-utils/log.h"
#include "fcitx-utils/standardpaths.h"
#include "fcitx-utils/stringutils.h"
#include "addoninfo.h"
#include "addoninstance.h"
#include "addonloader_p.h"
#include "config.h"

namespace fcitx {

AddonLoader::~AddonLoader() {}

SharedLibraryLoader::~SharedLibraryLoader() {}

AddonInstance *SharedLibraryLoader::load(const AddonInfo &info,
                                         AddonManager *manager) {
    auto iter = registry_.find(info.uniqueName());
    if (iter == registry_.end()) {
        std::vector<std::string> libnames =
            stringutils::split(info.library(), ";");

        if (libnames.empty()) {
            FCITX_ERROR() << "Failed to parse Library field: " << info.library()
                          << " for addon " << info.uniqueName();
            return nullptr;
        }

        std::vector<Library> libraries;
        for (std::string_view libname : libnames) {
            Flags<LibraryLoadHint> flag = LibraryLoadHint::DefaultHint;
            if (stringutils::consumePrefix(libname, "export:")) {
                flag |= LibraryLoadHint::ExportExternalSymbolsHint;
            }
            const auto file =
                stringutils::concat(libname, FCITX_LIBRARY_SUFFIX);
            const auto libraryPaths = StandardPaths::global().locateAll(
                StandardPathsType::Addon, file);
            if (libraryPaths.empty()) {
                FCITX_ERROR() << "Could not locate library " << file
                              << " for addon " << info.uniqueName() << ".";
            }
            bool loaded = false;
            for (const auto &libraryPath : libraryPaths) {
                Library library(libraryPath);
                if (library.load(flag)) {
                    libraries.push_back(std::move(library));
                    loaded = true;
                    break;
                }
                FCITX_ERROR()
                    << "Failed to load library for addon " << info.uniqueName()
                    << " on " << libraryPath << ". Error: " << library.error();
            }
            if (!loaded) {
                break;
            }
        }

        if (libraries.size() == libnames.size()) {
            try {
                registry_.emplace(info.uniqueName(),
                                  std::make_unique<SharedLibraryFactory>(
                                      info, std::move(libraries)));
            } catch (const std::exception &e) {
                FCITX_ERROR() << "Failed to initialize addon factory for addon "
                              << info.uniqueName() << ". Error: " << e.what();
            }
            iter = registry_.find(info.uniqueName());
        }
    }

    if (iter == registry_.end()) {
        return nullptr;
    }

    try {
        return iter->second->factory()->create(manager);
    } catch (const std::exception &e) {
        FCITX_ERROR() << "Failed to create addon: " << info.uniqueName() << " "
                      << e.what();
    } catch (...) {
        FCITX_ERROR() << "Failed to create addon: " << info.uniqueName();
    }
    return nullptr;
}

StaticLibraryLoader::StaticLibraryLoader(StaticAddonRegistry *registry_)
    : registry(registry_) {}

AddonInstance *StaticLibraryLoader::load(const AddonInfo &info,
                                         AddonManager *manager) {
    auto iter = registry->find(info.uniqueName());
    if (iter == registry->end()) {
        return nullptr;
    }
    try {
        return iter->second->create(manager);
    } catch (const std::exception &e) {
        FCITX_ERROR() << "Failed to create addon: " << info.uniqueName() << " "
                      << e.what();
    } catch (...) {
        FCITX_ERROR() << "Failed to create addon: " << info.uniqueName();
    }
    return nullptr;
}
} // namespace fcitx
