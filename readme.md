# usage

learn bazel && thrid library usage

## build unit test

```bash
bazel build --config=asan --config=local //app:usage
```

## build benchmark

```bash
bazel build --config=local --config=opt //bench:bench
```

