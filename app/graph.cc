#include "absl/log/log.h"
#include "gtest/gtest.h"
#include "taskflow/taskflow.hpp"

TEST(graph, hello) {
    tf::Taskflow taskflow;
    tf::Executor executor;

    // create a task dependency graph
    tf::Task a
        = taskflow.emplace([]() -> void { LOG(INFO) << __PRETTY_FUNCTION__ << " Task A run"; });
    tf::Task b
        = taskflow.emplace([]() -> void { LOG(INFO) << __PRETTY_FUNCTION__ << " Task B run"; });
    tf::Task c
        = taskflow.emplace([]() -> void { LOG(INFO) << __PRETTY_FUNCTION__ << " Task C run"; });
    tf::Task d
        = taskflow.emplace([]() -> void { LOG(INFO) << __PRETTY_FUNCTION__ << " Task D run"; });

    a.precede(b);
    a.precede(c);
    b.precede(d);
    c.precede(d);

    taskflow.dump(std::cout);

    executor.run(taskflow).wait();
}
