# cast-matcher

This is a basic example of C-style cast detection using `clang`'s AST Matchers
in less than 60 lines of C++ code.

## Usage

#### clone this repository in `clang`'s source tree

`git clone git@github.com:thegameg/cast-matcher.git llvm/tools/clang/extra/cast-matcher`

#### update the `CMakeLists.txt`

`echo 'add_subdirectory(cast-matcher)' >> llvm/tools/clang/extra/CMakeLists.txt`

#### build

Build using `ninja cast-matcher` or `make cast-matcher`.

## Example

`is_cast.c`:

```c
#include <stddef.h>

void foo(void)
{
  int *i = NULL;
  void *a = (void *)i;
}
```

```shell
$ cast-matcher is_cast.c
```

```
is_cast.c:6:13: error: explicit cast detected
  void *a = (void *)i;
            ^~~~~~~~~
1 error generated.
Error while processing is_cast.c.
```
