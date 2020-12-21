# mattsep::random

A simple and easy-to-use random number library.

## Examples

### Generate random integers
```cpp
#include <iostream>
#include <mattsep/random/random.hpp>

int main() {
    namespace random = mattsep::random;

    auto rng = random::rng{};
    for (int i = 0; i < 5; ++i) {
        std::cout << rng.random<int>() << '\n';
    }
}
```

Possible output:
```
1377258420
1252685607
858753384
1453711669
2108368629
```

### Generate random floating point numbers in [0, 1)
```cpp
#include <iostream>
#include <mattsep/random/random.hpp>

int main() {
    namespace random = mattsep::random;

    auto rng = random::rng{};
    for (int i = 0; i < 5; ++i) {
        std::cout << rng.random<double>() << '\n';
    }
}
```

Possible output:
```
0.83726
0.562652
0.184881
0.680933
0.270382
```