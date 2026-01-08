"""
https://github.com/taskflow/taskflow 
"""

load("@rules_cc//cc:cc_library.bzl", "cc_library")

cc_library(
    name = "taskflow",
    hdrs = glob([
        "taskflow/algorithm/*.hpp",
        "taskflow/core/*.hpp",
        "taskflow/utility/*.hpp",
        "taskflow/*.hpp",
    ]),
    includes = ["."],
    visibility = [
        "//visibility:public",
    ],
)
