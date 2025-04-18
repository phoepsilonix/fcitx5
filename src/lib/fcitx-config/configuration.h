/*
 * SPDX-FileCopyrightText: 2015-2015 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#ifndef _FCITX_CONFIG_CONFIGURATION_H_
#define _FCITX_CONFIG_CONFIGURATION_H_

#include <memory>
#include <string>
#include <vector>
#include <fcitx-config/fcitxconfig_export.h>
#include <fcitx-config/option.h>
#include <fcitx-config/optiontypename.h>
#include <fcitx-config/rawconfig.h>
#include <fcitx-utils/macros.h>

#define FCITX_CONFIGURATION_EXTEND(NAME, SUBCLASS, ...)                        \
    class NAME;                                                                \
    FCITX_SPECIALIZE_TYPENAME(NAME, #NAME)                                     \
    FCITX_CONFIGURATION_CLASS_EXTEND(NAME, SUBCLASS, __VA_ARGS__)

#define FCITX_CONFIGURATION(NAME, ...)                                         \
    FCITX_CONFIGURATION_EXTEND(NAME, ::fcitx::Configuration, __VA_ARGS__)

#define FCITX_CONFIGURATION_CLASS_EXTEND(NAME, SUBCLASS, ...)                  \
    class NAME : public SUBCLASS {                                             \
    public:                                                                    \
        NAME() {}                                                              \
        NAME(const NAME &other) : NAME() { copyHelper(other); }                \
        NAME &operator=(const NAME &other) {                                   \
            copyHelper(other);                                                 \
            return *this;                                                      \
        }                                                                      \
        bool operator==(const NAME &other) const {                             \
            return compareHelper(other);                                       \
        }                                                                      \
        const char *typeName() const override { return #NAME; }                \
                                                                               \
    public:                                                                    \
        __VA_ARGS__                                                            \
    };

#define FCITX_CONFIGURATION_CLASS(NAME, ...)                                   \
    FCITX_CONFIGURATION_CLASS_EXTEND(NAME, ::fcitx::Configuration, __VA_ARGS__)

namespace fcitx {

class ConfigurationPrivate;

class FCITXCONFIG_EXPORT Configuration {
    friend class OptionBase;

public:
    Configuration();
    virtual ~Configuration();

    /// Load configuration from RawConfig. If partial is true, non-exist option
    /// will be reset to default value, otherwise it will be untouched.
    void load(const RawConfig &config, bool partial = false);
    void save(RawConfig &config) const;
    void dumpDescription(RawConfig &config) const;
    FCITX_NODISCARD virtual const char *typeName() const = 0;

    /**
     * Set default value to current value.
     *
     * Sometimes, we need to customize the default value for the same type. This
     * function will set the default value to current value.
     */
    void syncDefaultValueToCurrent();

protected:
    bool compareHelper(const Configuration &other) const;
    void copyHelper(const Configuration &other);

private:
    void dumpDescriptionImpl(RawConfig &config,
                             const std::vector<std::string> &parentPaths) const;
    void addOption(fcitx::OptionBase *option);

    FCITX_DECLARE_PRIVATE(Configuration);
    std::unique_ptr<ConfigurationPrivate> d_ptr;
};
} // namespace fcitx

#endif // _FCITX_CONFIG_CONFIGURATION_H_
