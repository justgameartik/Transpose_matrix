This code provides the algrorithm of multithreads asynchronius matrix transponing.
Worker.hpp has worker interface with opportunity for new features.
Worker.cpp is the realization of transponing algorithm as worker interface child.
The number of threads is set in TransformWorker() constructor, but for now there's no opportunity to change this value in this interface. It was done so as not change the header file worker.hpp
The Class TransformWorker use common threadpool the result of which is future<Matrix> type.

Here is example of usage this code:
```js
#include "worker.hpp"

Matrix matrix{
    std::vector<int>{
        1, 2, 3,
        4, 5, 6
    },
    3,
    2
};

auto worker = get_new_worker();
auto future = worker->AsyncProcess(matrix);

// future.get() returns transposed matrix
// Matrix transposed_matrix{
//    std::vector<int> {
//        1, 4,
//        2, 5,
//        3, 6
//    }
//    2,
//    3
// }
```
