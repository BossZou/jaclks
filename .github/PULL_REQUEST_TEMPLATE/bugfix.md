## Bug description

<!-- What was the bug? Describe the incorrect behavior. -->

## Root cause

<!-- Why did it happen? Point to the code path that caused it. -->

## Fix

<!-- What does this PR change and how does it address the root cause? -->

## Related issue

<!-- Link to the issue this PR fixes, e.g. Fixes #123. -->

## Test plan

<!-- How was the fix verified? List the commands you ran. -->

```shell
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_GTEST=ON
make
ctest --verbose --output-on-failure --build-config Debug
```

## Checklist

- [ ] Regression test added that fails before this fix and passes after
- [ ] Code passes `cpplint` and follows `.clang-format`
- [ ] `CHANGELOG.md` updated (enforced by CI)
