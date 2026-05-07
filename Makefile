.PHONY: all cc_gen clean run

all: build

build:
	@bazel build --config=local //app:usage

build_bm:
	@bazel build --config=local --config=opt //bench:bench

build_pb:
	@bazel build --config=asan --config=local //app:parameter_pb

cc_gen:
	@bazel run --config=local //:cc_gen

clean:
	@bazel clean --expunge

run:
	@bazel run --config=asan --config=local //app:usage

gen:
	bazel run @bazel_cc_meta//cc_meta:refresh_all