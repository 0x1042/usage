#include "http.h"

#include "absl/log/log.h"
#include "cpr/api.h"
#include "httplib.h"

auto get_with_cpr(const std::string& url) -> std::string {
    cpr::Response rsp = cpr::Get(cpr::Url{url});
    return rsp.text;
}

auto get_with_httplib(const std::string& url) -> std::string {
    httplib::Client cli(url);

    LOG(ERROR) << "cli valid " << cli.is_valid();

    if (!cli.is_valid()) {
        return "server error";
    }

    if (auto rsp = cli.Get("/")) {
        return rsp->body;
    }

    return "";
}
