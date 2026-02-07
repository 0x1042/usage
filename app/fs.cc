#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <string>
#include <system_error>

#include "gtest/gtest.h"
#include "lib/log.h"
#include "lib/meta.h"

namespace {
namespace fs = std::filesystem;
}

#define CONCAT_INNER(a, b, c, d)        a##b##c##d
#define CONCAT_EXPAND(prefix, mid, num) CONCAT_INNER(prefix, mid, _, num)
#define UNIQUE_TMP_VAR(prefix)          CONCAT_EXPAND(prefix, _tmp, __COUNTER__)

#define ON_EXIT(dir)                                    \
    auto UNIQUE_TMP_VAR(exit) = defer([&]() -> void {   \
        if (fs::remove(dir)) {                          \
            INFO("remove {} success.", (dir).string()); \
        } else {                                        \
            ERROR("remove {} failed!", (dir).string()); \
        }                                               \
    })

TEST(fs, createdir1) {
    fs::path dir = "./log";
    ON_EXIT(dir);
    if (fs::create_directory(dir)) {
        INFO("create {} success.", dir.string());
    } else {
        ERROR("create {} failed!", dir.string());
    }
}

TEST(fs, createdir2) {
    fs::path dir = "/log";
    std::error_code ec{};
    ON_EXIT(dir);

    if (fs::create_directory(dir, ec)) {
        INFO("create {} success.", dir.string());
    } else {
        ERROR("create {} failed! error {}", dir.string(), ec.message());
    }
}

TEST(fs, createdir3) {
    fs::path nested = "./log/run";
    std::error_code ec{};
    ON_EXIT(nested);

    if (fs::create_directories(nested, ec)) {
        INFO("create {} success.", nested.string());
    } else {
        ERROR("create {} failed! error {}", nested.string(), ec.message());
    }
}

TEST(fs, ls) {
    fs::path nested = "app";

    for (const auto& entry : fs::recursive_directory_iterator(nested)) {
        std::string buf;
        buf.reserve(1024);

        buf.append("path:");
        buf.append(entry.path().string());

        if (entry.is_regular_file()) {
            buf.append(" extension:");
            buf.append(entry.path().extension());
            buf.append(" size:");
            buf.append(std::to_string(entry.file_size()));
        }

        buf.append(" modify time:");
        buf.append(std::format("{0:%X}", fs::last_write_time(entry)));

        INFO("entry info {}", buf);
    }
}

TEST(fs, tmp) {
    const auto& tmp_dir = fs::temp_directory_path();
    INFO("tmp dir is {}", tmp_dir.string());
    fs::path mytmp_dir = tmp_dir / "logs";

    std::error_code ec;

    if (fs::create_directory(mytmp_dir, ec)) {
        INFO("create {} success.", mytmp_dir.string());
    } else {
        ERROR("create {} failed! error {}", mytmp_dir.string(), ec.message());
    }
}

TEST(fs, copyfile) {
    std::string src = "MODULE.bazel";
    std::string dst = fs::temp_directory_path() / "app/MODULE.bazel";
    std::error_code ec;

    try {
        if (!(fs::copy_file(src, dst, ec))) {
            ERROR("copy file error {}", ec.message());
        }
    } catch (const std::exception& ex) {
        ERROR("copy exception {}", ex.what());
    }
}

TEST(fs, copyfiles) {
    std::string src = "app";
    std::string dst = fs::temp_directory_path() / "app";
    std::error_code ec;

    try {
        if (!(fs::copy_file(src, dst, fs::copy_options::recursive), ec)) {
            ERROR("copy dir error {}", ec.message());
        }
    } catch (const std::exception& ex) {
        ERROR("copy exception {}", ex.what());
    }
}

TEST(fs, renameormove) {
    fs::create_directory("tmp");
    std::ofstream("tmp/old_file.txt") << "This is file 1";
    fs::path old_name = "tmp/old_file.txt";
    fs::path new_name = "tmp/new_file.txt";

    auto rm = defer([&]() -> void { fs::remove_all("tmp"); });

    std::error_code ec;

    fs::rename(old_name, new_name);

    for (const auto& entry : fs::directory_iterator("tmp")) {
        INFO("tmp dir >>> {}", entry.path().string());
    }
}

TEST(fs, exist) {
    INFO("app/BUILD exit {}", fs::exists("app/BUILD"));
    INFO("app/BUILD.bazel exit exit {}", fs::exists("app/BUILD.bazel exit"));
}

TEST(fs, symlink) {
    fs::path external = "external";
    INFO("external is symlink {}", fs::is_symlink(external));

    if (fs::is_symlink(external)) {
        const auto& real = fs::read_symlink(external);
        INFO("external point to {}", real.string());
    }
}

TEST(fs, absolute) {
    fs::path external = "external";
    const auto& external_absolute = fs::absolute(external);
    INFO(
        "external absolute path {} is_symlink {}",
        external_absolute.string(),
        fs::is_symlink(external_absolute));

    fs::path app = "app";
    INFO("app path {}", fs::absolute(app).string());
}

TEST(fs, relative) {
    fs::path external = fs::absolute("external");
    fs::path current_path = fs::current_path();

    INFO("external {} cur {} ", external.string(), current_path.string());
    INFO("relative is {} ", fs::relative(current_path, current_path).string());
}

TEST(fs, space) {
    fs::path p = "/";
    const auto& space_info = fs::space(p);

    std::string buf;
    buf.reserve(128);

    buf.append("capacity:");
    buf.append(std::to_string(space_info.capacity));
    buf.append("\tfree:");
    buf.append(std::to_string(space_info.free));
    buf.append("\tcapacity:");
    buf.append(std::to_string(space_info.available));

    INFO("space info {}", buf);
}

TEST(fs, perm) {
    fs::path file = "MODULE.bazel.lock";
    const auto& perm = fs::status(file).permissions();

    std::string buf;
    buf.reserve(128);

    auto perm_fn = [&](const auto& perms, const std::string& id) -> void {
        if ((perm & perms) != fs::perms::none) {
            buf.append(id);
        } else {
            buf.append("-");
        }
    };

    perm_fn(fs::perms::owner_read, "r");
    perm_fn(fs::perms::owner_write, "w");
    perm_fn(fs::perms::owner_exec, "x");

    perm_fn(fs::perms::group_read, "r");
    perm_fn(fs::perms::group_write, "w");
    perm_fn(fs::perms::group_exec, "x");

    perm_fn(fs::perms::others_read, "r");
    perm_fn(fs::perms::others_write, "w");
    perm_fn(fs::perms::others_exec, "x");

    INFO("file {} permissions {}", file.string(), buf);
}