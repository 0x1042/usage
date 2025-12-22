.PHONY: all cc_gen clean run

all: cc_gen
	@bazel build --config=asan --config=local --config=dbg //app:usage

build:
	@bazel build --config=asan --config=local --config=dbg //app:usage

cc_gen:
	@bazel run --config=asan --config=local --config=dbg //:cc_gen

clean:
	@bazel clean --expunge

run:
	@bazel run --config=asan --config=local --config=dbg //app:usage