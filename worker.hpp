#include <vector>
#include <future>

struct Matrix {
    std::vector<int> data;
    unsigned width, height;
};

class WorkerInterface {
public:
    WorkerInterface() = default;

    WorkerInterface(WorkerInterface &&) = delete;
    WorkerInterface(const WorkerInterface &) = delete;
    WorkerInterface & operator=(WorkerInterface &&) = delete;
    WorkerInterface & operator=(const WorkerInterface &) = delete;
    
    virtual ~WorkerInterface() = default;
    
    virtual std::future<Matrix> AsyncProcess(Matrix) = 0;
};

std::shared_ptr<WorkerInterface> get_new_worker();