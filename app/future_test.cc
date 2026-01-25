// #include "folly/futures/Future.h"

// #include <future>
// #include <memory>

// #include "cpr/api.h"
// #include "folly/Executor.h"
// #include "folly/coro/BlockingWait.h"
// #include "folly/executors/CPUThreadPoolExecutor.h"
// #include "folly/futures/Promise.h"
// #include "gtest/gtest.h"

// template <typename T>
// auto to_folly_future(folly::Executor* waiterExecutor, std::future<T> std_future)
//     -> folly::Future<T> {
//     folly::Promise<T> promise;
//     folly::Future<T> future = promise.getFuture();
//     waiterExecutor->add([p = std::move(promise), sf = std::move(std_future)]() mutable -> auto {
//         p.setValue(sf.get());
//     });
//     return future;
// }

// auto exec() -> std::shared_ptr<folly::CPUThreadPoolExecutor> {
//     static auto worker = std::make_shared<folly::CPUThreadPoolExecutor>(4);
//     return worker;
// }

// auto get_url_async(std::string url) -> std::future<cpr::Response> {
//     return std::async(
//         std::launch::async, [url = std::move(url)]() -> auto { return cpr::Get(cpr::Url{url});
//         });
// }

// TEST(coro, single) {
//     std::string url = "https://www.baidu.com";

//     // 使用 blockingWait 开启协程作用域
//     folly::coro::blockingWait([&]() -> folly::coro::Task<void> {
//         auto f = get_url_async(std::move(url));
//         auto ff = to_folly_future(exec().get(), std::move(f));
//         auto resp = co_await std::move(ff);
//         LOG(ERROR) << resp.text;
//         EXPECT_EQ(resp.status_code, 200);
//         co_return;
//     }());
// }