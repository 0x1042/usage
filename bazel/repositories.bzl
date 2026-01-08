"""
thrid party of non bazel lib
"""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def data_dependency():
    http_archive(
        name = "taskflow",
        build_file = "//bazel/3rdparty:taskflow.bzl",
        strip_prefix = "taskflow-4.0.0",
        url = "https://github.com/taskflow/taskflow/archive/refs/tags/v4.0.0.zip",
    )
