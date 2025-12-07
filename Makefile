.PHONY: all cc_gen clean run

all: cc_gen
	@bazel build --config=asan //app:usage

cc_gen:
	@bazel run --config=asan //:cc_gen

clean:
	@bazel clean --expunge

run:
	@bazel run --config=asan //app:usage