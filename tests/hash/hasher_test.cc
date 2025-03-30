#include "hash/hasher.h"

#include <cstdio>

namespace jaclks {

int test_hasher() {

printf("Hash value is : %zu\n", Hasher<int>{}(100));

{
    auto data = new int(1);

    printf("Hash value is : %zu\n", Hasher<int*>{}(data));

    delete data;
}

return 0;
}

}
