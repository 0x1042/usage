"""
https://github.com/jgaa/RESTinCurl
"""

load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "RESTinCurl",
    hdrs = ["include/restincurl/restincurl.h"],
    includes = ["include"],
    defines = [
        "RESTINCURL_ENABLE_ASYNC",
        "RESTINCURL_ENABLE_ASIO",
    ],
    visibility = ["//visibility:public"],
)
