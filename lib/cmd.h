#pragma once

#include <string>
#include <vector>

#include <reproc++/drain.hpp>
#include <reproc++/reproc.hpp>

auto exec_cmd(const std::vector<std::string>& args) -> std::string;