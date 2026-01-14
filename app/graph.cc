#include <bit>

#include "gtest/gtest.h"
#include "log.h"
#include "taskflow/taskflow.hpp"

TEST(graph, hello) {
    tf::Taskflow taskflow;
    tf::Executor executor;

    // create a task dependency graph
    tf::Task a = taskflow.emplace([]() -> void { DEBUG("{} Task A run...", __PRETTY_FUNCTION__); });
    tf::Task b = taskflow.emplace([]() -> void { DEBUG("{} Task B run...", __PRETTY_FUNCTION__); });
    tf::Task c = taskflow.emplace([]() -> void { DEBUG("{} Task C run...", __PRETTY_FUNCTION__); });
    tf::Task d = taskflow.emplace([]() -> void { DEBUG("{} Task D run...", __PRETTY_FUNCTION__); });

    a.precede(b);
    a.precede(c);
    b.precede(d);
    c.precede(d);

    taskflow.dump(std::cout);

    executor.run(taskflow).wait();
}
