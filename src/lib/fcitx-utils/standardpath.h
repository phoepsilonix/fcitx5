/*
 * SPDX-FileCopyrightText: 2016-2016 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#ifndef _FCITX_UTILS_STANDARDPATH_H_
#define _FCITX_UTILS_STANDARDPATH_H_

/// \addtogroup FcitxUtils
/// \{
/// \file
/// \brief Utility classes to handle XDG file path.
///
/// Example:
/// \code{.cpp}
/// auto files = path.multiOpenAll(StandardPath::Type::PkgData, "inputmethod",
///                                O_RDONLY, filter::Suffix(".conf"));
/// \endcode
/// Open all files under $XDG_CONFIG_{HOME,DIRS}/fcitx5/inputmethod/*.conf.

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <fcitx-utils/fcitxutils_export.h>
#include <fcitx-utils/log.h>
#include <fcitx-utils/macros.h>
#include <fcitx-utils/standardpaths.h>
#include <fcitx-utils/stringutils.h>
#include <fcitx-utils/unixfd.h>

namespace fcitx {

namespace filter {

/// \brief Filter class to chain sub filters together.
template <typename... Types>
class Chainer;

template <>
class Chainer<> {
public:
    bool operator()(const std::string & /*unused*/,
                    const std::string & /*unused*/, bool /*unused*/) {
        return true;
    }
};

template <typename First, typename... Rest>
class Chainer<First, Rest...> : Chainer<Rest...> {
    using super_class = Chainer<Rest...>;

public:
    Chainer(First first, Rest... rest)
        : super_class(std::move(rest)...), filter(std::move(first)) {}

    bool operator()(const std::string &path, const std::string &dir,
                    bool user) {
        if (!filter(path, dir, user)) {
            return false;
        }
        return super_class::operator()(path, dir, user);
    }

private:
    First filter;
};

/// \brief Filter class that revert the sub filter result.
template <typename T>
struct NotFilter {
    NotFilter(T filter_) : filter(filter_) {}

    bool operator()(const std::string &path, const std::string &dir,
                    bool isUser) {
        return !filter(path, dir, isUser);
    }

private:
    T filter;
};

template <typename T>
NotFilter<T> Not(T t) {
    return {t};
}

/// \brief Filter class that filters based on user file.
struct FCITXUTILS_DEPRECATED_EXPORT User {
    bool operator()(const std::string & /*unused*/,
                    const std::string & /*unused*/, bool isUser) {
        return isUser;
    }
};

/// \brief Filter class that filters file based on prefix
struct FCITXUTILS_DEPRECATED_EXPORT Prefix {
    Prefix(const std::string &prefix_) : prefix(prefix_) {}

    bool operator()(const std::string &path, const std::string & /*unused*/,
                    bool /*unused*/) const {
        return stringutils::startsWith(path, prefix);
    }

    std::string prefix;
};

/// \brief Filter class that filters file based on suffix
struct FCITXUTILS_DEPRECATED_EXPORT Suffix {
    Suffix(const std::string &suffix_) : suffix(suffix_) {}

    bool operator()(const std::string &path, const std::string & /*unused*/,
                    bool /*unused*/) const {
        return stringutils::endsWith(path, suffix);
    }

    std::string suffix;
};
} // namespace filter

/// \brief File descriptor wrapper that handles file descriptor and rename
/// automatically.
class FCITXUTILS_DEPRECATED_EXPORT StandardPathTempFile {
public:
    StandardPathTempFile(int fd = -1, const std::string &realFile = {},
                         const std::string &tempPath = {})
        : fd_(UnixFD::own(fd)), path_(realFile), tempPath_(tempPath) {}
    StandardPathTempFile(StandardPathTempFile &&other) = default;
    virtual ~StandardPathTempFile();

    int fd() const { return fd_.fd(); }
    bool isValid() const { return fd_.isValid(); }

    const std::string &path() const { return path_; }
    const std::string &tempPath() const { return tempPath_; }

    int release();
    void close();
    void removeTemp();

private:
    UnixFD fd_;
    std::string path_;
    std::string tempPath_;
};

/// \brief Utility class that wraps around UnixFD. It also contains the actual
/// file name information.
class FCITXUTILS_DEPRECATED_EXPORT StandardPathFile {
public:
    StandardPathFile(int fd = -1, const std::string &path = {})
        : fd_(UnixFD::own(fd)), path_(path) {}
    StandardPathFile(StandardPathFile &&other) = default;
    virtual ~StandardPathFile();

    StandardPathFile &operator=(StandardPathFile &&other) = default;

    int fd() const { return fd_.fd(); }
    bool isValid() const { return fd_.isValid(); }

    const std::string &path() const { return path_; }

    int release();

private:
    UnixFD fd_;
    std::string path_;
};

class StandardPathPrivate;

using StandardPathFileMap = std::map<std::string, StandardPathFile>;
using StandardPathFilesMap =
    std::map<std::string, std::vector<StandardPathFile>>;

/// \brief Utility class to open, locate, list files based on XDG standard.
class FCITXUTILS_DEPRECATED_EXPORT StandardPath {
public:
    /// \brief Enum for location type.
    enum class Type {
        /// Xdg Config dir
        Config,
        /// Xdg Config dir/fcitx5
        PkgConfig,
        /// Xdg data dir
        Data,
        /// Xdg cache dir
        Cache,
        /// Xdg runtime dir
        Runtime,
        /// addon shared library dir.
        Addon,
        /// Xdg data dir/fcitx5
        PkgData
    };

    /**
     * Allow to construct a StandardPath with customized internal value.
     *
     * @param packageName the sub directory under other paths.
     * @param builtInPath this will override the value from fcitxPath.
     * @param skipBuiltInPath skip built-in path
     * @param skipUserPath skip user path, useful when doing readonly-test.
     * @since 5.1.9
     */
    explicit StandardPath(
        const std::string &packageName,
        const std::unordered_map<std::string, std::string> &builtInPath,
        bool skipBuiltInPath, bool skipUserPath);

    explicit StandardPath(bool skipFcitxPath, bool skipUserPath);
    explicit StandardPath(bool skipFcitxPath = false);
    virtual ~StandardPath();

    /// \brief Return the global instance of StandardPath.
    ///
    /// return a global default so we can share it, C++11 static initialization
    /// is thread-safe
    static const StandardPath &global();

    /// \brief Return fcitx specific path defined at compile time.
    ///
    /// Currently, available value of fcitxPath are:
    /// datadir, pkgdatadir, libdir, bindir, localedir, addondir, libdatadir.
    /// Otherwise it will return nullptr.
    static const char *fcitxPath(const char *path);

    /// \brief Return a path under specific fcitxPath directory.
    /// path is required to be a valid value.
    static std::string fcitxPath(const char *path, const char *subPath);

    /// \brief Scan the directories of given type.
    ///
    /// Callback returns true to continue the scan.
    void scanDirectories(Type type,
                         const std::function<bool(const std::string &path,
                                                  bool user)> &scanner) const;

    /// \brief Scan the given directories.
    ///
    /// Callback returns true to continue the scan.
    /// @since 5.0.4
    void scanDirectories(
        const std::string &userDir, const std::vector<std::string> &directories,
        const std::function<bool(const std::string &path, bool user)> &scanner)
        const;

    /// \brief Scan files scan file under [directory]/[path]
    /// \param path sub directory name.
    void scanFiles(Type type, const std::string &path,
                   const std::function<bool(const std::string &path,
                                            const std::string &dir, bool user)>
                       &scanner) const;

    /// \brief Get user writable directory for given type.
    std::string userDirectory(Type type) const;

    /// \brief Get all directories in the order of priority.
    std::vector<std::string> directories(Type type) const;

    /// \brief Check if a file exists.
    std::string locate(Type type, const std::string &path) const;

    /// \brief list all matched files.
    std::vector<std::string> locateAll(Type type,
                                       const std::string &path) const;

    /// \brief Open the first matched and succeeded file.
    ///
    /// This function is preferred over locale if you just want to open the
    /// file. Then you can avoid the race condition.
    /// \see openUser()
    StandardPathFile open(Type type, const std::string &path, int flags) const;

    /// \brief Open the user file.
    StandardPathFile openUser(Type type, const std::string &path,
                              int flags) const;

    /**
     * \brief Open the non-user file.
     *
     * \since 5.0.6
     */
    StandardPathFile openSystem(Type type, const std::string &path,
                                int flags) const;

    /// \brief Open user file, but create file with mktemp.
    StandardPathTempFile openUserTemp(Type type,
                                      const std::string &pathOrig) const;

    /**
     * \brief Save the file safely with write and rename to make sure the
     * operation is atomic.
     *
     * Callback shall not close the file descriptor. If the API you are using
     * does cannot do that, you may use UnixFD to help you dup it first.
     *
     * \param callback Callback function that accept a file descriptor and
     * return whether the save if success or not.
     */
    bool safeSave(Type type, const std::string &pathOrig,
                  const std::function<bool(int)> &callback) const;

    /**
     * \brief Locate all files match the filter under first [directory]/[path].
     *
     * Prefer this function over multiOpenFilter, if there could be too many
     * files that exceeds the systems file descriptor limit.
     * @since 5.1.10
     * @see multiOpenFilter
     */
    std::map<std::string, std::string>
    locateWithFilter(Type type, const std::string &path,
                     std::function<bool(const std::string &path,
                                        const std::string &dir, bool user)>
                         filter) const;

    /**
     * \brief Locate all files match the filter under first [directory]/[path].
     *
     * You may pass multiple filter to it.
     * Prefer this function over multiOpen, if there could be too many
     * files that exceeds the systems file descriptor limit.
     * @since 5.1.10
     * @see multiOpen
     */
    template <typename Arg1, typename... Args>
    std::map<std::string, std::string>
    locate(Type type, const std::string &path, Arg1 arg1, Args... args) const {
        return locateWithFilter(type, path,
                                filter::Chainer<Arg1, Args...>(
                                    std::move(arg1), std::move(args)...));
    }

    /// \brief Open all files match the first [directory]/[path].
    std::vector<StandardPathFile> openAll(Type type, const std::string &path,
                                          int flags) const;

    /// \brief Open all files match the filter under first [directory]/[path].
    StandardPathFileMap
    multiOpenFilter(Type type, const std::string &path, int flags,
                    std::function<bool(const std::string &path,
                                       const std::string &dir, bool user)>
                        filter) const;

    /// \brief Open all files match the filter under first [directory]/[path].
    ///
    /// You may pass multiple filter to it.
    template <typename... Args>
    StandardPathFileMap multiOpen(Type type, const std::string &path, int flags,
                                  Args... args) const {
        return multiOpenFilter(type, path, flags,
                               filter::Chainer<Args...>(std::move(args)...));
    }

    /// \brief Open all files match the filter under all [directory]/[path].
    StandardPathFilesMap
    multiOpenAllFilter(Type type, const std::string &path, int flags,
                       std::function<bool(const std::string &path,
                                          const std::string &dir, bool user)>
                           filter) const;

    /// \brief Open all files match the filter under all [directory]/[path].
    ///
    /// You may pass multiple filter to it.
    template <typename... Args>
    StandardPathFilesMap multiOpenAll(Type type, const std::string &path,
                                      int flags, Args... args) const {
        return multiOpenAllFilter(type, path, flags,
                                  filter::Chainer<Args...>(args...));
    }

    int64_t timestamp(Type type, const std::string &path) const;

    /**
     * Check if certain executable presents in PATH.
     *
     * If the file presents, return the absolute PATH. If name is absolute path,
     * check the absolute path instead of using PATH.
     *
     * @since 5.0.18
     */
    static std::string findExecutable(const std::string &name);

    /**
     * Check if certain executable presents in PATH.
     *
     * @see findExecutable     *
     * @since 5.0.18
     */
    static bool hasExecutable(const std::string &name);

    /**
     * Sync system umask to internal state. This will affect the file
     * permission created by safeSave.
     *
     * @see safeSave
     * @since 5.1.2
     */
    void syncUmask() const;

    /**
     * Whether this StandardPath is configured to Skip built-in path.
     *
     * Built-in path is usually configured at build time, hardcoded.
     * In portable environment (Install prefix is not fixed), this should be
     * set to false.
     *
     * @since 5.1.9
     */
    bool skipBuiltInPath() const;

    /**
     * Whether this StandardPath is configured to Skip user path.
     *
     * @since 5.1.9
     */
    bool skipUserPath() const;

private:
    std::unique_ptr<StandardPathPrivate> d_ptr;
    FCITX_DECLARE_PRIVATE(StandardPath);
};

static inline LogMessageBuilder &operator<<(LogMessageBuilder &builder,
                                            const StandardPathFile &file) {
    builder << "StandardPathFile(fd=" << file.fd() << ",path=" << file.path()
            << ")";
    return builder;
}

template <>
class StandardPathsTypeConverter<StandardPath::Type> {
public:
    using self_type = StandardPath::Type;

    static constexpr StandardPathsType convert(StandardPath::Type type) {
        switch (type) {
        case StandardPath::Type::Config:
            return StandardPathsType::Config;
        case StandardPath::Type::PkgConfig:
            return StandardPathsType::PkgConfig;
        case StandardPath::Type::Data:
            return StandardPathsType::Data;
        case StandardPath::Type::Cache:
            return StandardPathsType::Cache;
        case StandardPath::Type::Runtime:
            return StandardPathsType::Runtime;
        case StandardPath::Type::Addon:
            return StandardPathsType::Addon;
        case StandardPath::Type::PkgData:
            return StandardPathsType::PkgData;
        default:
            return StandardPathsType::Config;
        }
    }
};

} // namespace fcitx

#endif // _FCITX_UTILS_STANDARDPATH_H_
