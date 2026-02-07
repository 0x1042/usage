# build unit test

```
bazel build --config=asan --config=local //app:usage
```

# build benchmark

```
@bazel build --config=local --config=opt //bench:bench
```