#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <string>
#include <system_error>

#include "absl/log/log.h"
#include "gtest/gtest.h"
#include "meta.h"

namespace {
namespace fs = std::filesystem;
}

TEST(fs, createdir1) {
    fs::path dir = "./log";

    auto rm = defer([&]() -> void {
        if (fs::remove(dir)) {
            LOG(INFO) << "remove " << dir << " success!";
        } else {
            LOG(ERROR) << "remove " << dir << " failed!";
        }
    });

    if (fs::create_directory(dir)) {
        LOG(INFO) << "create " << dir << " success!";
    } else {
        LOG(ERROR) << "create " << dir << " failed!";
    }
}

TEST(fs, createdir2) {
    fs::path dir = "/log";
    std::error_code ec{};

    auto rm = defer([&]() -> void {
        if (fs::remove(dir)) {
            LOG(INFO) << "remove " << dir << " success!";
        } else {
            LOG(ERROR) << "remove " << dir << " failed!";
        }
    });

    if (fs::create_directory(dir, ec)) {
        LOG(INFO) << "create " << dir << " success!";
    } else {
        LOG(ERROR) << "create " << dir << " failed!" << ec.message();
    }
}

TEST(fs, createdir3) {
    fs::path nested = "./log/run";
    std::error_code ec{};

    auto rm = defer([&]() -> void {
        if (fs::remove(nested)) {
            LOG(INFO) << "remove " << nested << " success!";
        } else {
            LOG(ERROR) << "remove " << nested << " failed!";
        }
    });

    if (fs::create_directories(nested, ec)) {
        LOG(INFO) << "create " << nested << " success!";
    } else {
        LOG(ERROR) << "create " << nested << " failed!" << ec.message();
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

        LOG(INFO) << "entry info:" << buf;
    }
}

TEST(fs, tmp) {
    const auto& tmp_dir = fs::temp_directory_path();
    LOG(INFO) << "tmp dir is " << tmp_dir;
    fs::path mytmp_dir = tmp_dir / "logs";

    std::error_code ec;

    if (fs::create_directory(mytmp_dir, ec)) {
        LOG(INFO) << "create " << mytmp_dir << " success";
    } else {
        LOG(ERROR) << "create " << mytmp_dir << " fail >>> " << ec.message();
    }
}

TEST(fs, copyfile) {
    std::string src = "MODULE.bazel";
    std::string dst = fs::temp_directory_path() / "app/MODULE.bazel";

    try {
        if (fs::copy_file(src, dst)) {
            LOG(INFO) << "copy file success";
        } else {
            LOG(ERROR) << "copy file error";
        }
    } catch (const std::exception& ex) {
        LOG(ERROR) << "copy exception " << ex.what();
    }
}

TEST(fs, copyfiles) {
    std::string src = "app";
    std::string dst = fs::temp_directory_path() / "app";

    try {
        if (fs::copy_file(src, dst, fs::copy_options::recursive)) {
            LOG(INFO) << "copy dir success";
        } else {
            LOG(ERROR) << "copy dir error";
        }
    } catch (const std::exception& ex) {
        LOG(ERROR) << "copy exception " << ex.what();
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
        LOG(INFO) << "tmp dir >>>" << entry;
    }
}

TEST(fs, exist) {
    LOG(INFO) << "app/BUILD exit: " << fs::exists("app/BUILD");
    LOG(INFO) << "app/BUILD.bazel exit: " << fs::exists("app/BUILD.bazel");
}

TEST(fs, symlink) {
    fs::path external = "external";
    LOG(INFO) << "external is symlink " << fs::is_symlink(external);

    if (fs::is_symlink(external)) {
        const auto& real = fs::read_symlink(external);
        LOG(INFO) << "external point to " << real;
    }
}

TEST(fs, absolute) {
    fs::path external = "external";
    const auto& external_absolute = fs::absolute(external);
    LOG(INFO) << "external absolute path: " << external_absolute;
    LOG(INFO) << "external absolute path is_symlink : " << fs::is_symlink(external_absolute);

    fs::path app = "app";
    LOG(INFO) << "app app path: " << fs::absolute(app);
}

TEST(fs, relative) {
    fs::path external = fs::absolute("external");
    fs::path current_path = fs::current_path();
    LOG(INFO) << "external " << external << " cur:" << current_path;

    LOG(INFO) << "relative is: " << fs::relative(current_path, current_path);
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

    LOG(INFO) << "space info:" << buf;
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

    LOG(INFO) << "file:" << file << " permissions:" << buf;
}