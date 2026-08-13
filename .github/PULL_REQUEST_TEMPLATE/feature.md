## Summary

<!-- Briefly describe the new feature and what it adds to jaclks. -->

## Motivation

<!-- Why is this feature needed? What problem does it solve? -->

## API design

<!-- Describe the public API (classes, interfaces, method signatures) following jaclks' Java-style conventions. -->

## Related issue

<!-- Link to the issue this PR implements, e.g. Closes #123. -->

## Test plan

<!-- How was this tested? List the commands you ran. -->

```shell
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_GTEST=ON
make
ctest --verbose --output-on-failure --build-config Debug
```

## Checklist

- [ ] Unit tests added for the new feature
- [ ] Code passes `cpplint` and follows `.clang-format`
- [ ] `CHANGELOG.md` updated (enforced by CI)
- [ ] Documentation updated if applicable
