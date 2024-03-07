#include <thread>
#include <queue>
#include <unordered_map>

#include "worker.hpp"

Matrix MakeTranspose(const Matrix& matrix);

class TransposeWorker : public WorkerInterface {
public:
    TransposeWorker(int32_t threads_num=2) {
        threads.reserve(threads_num);
        for (size_t i = 0; i < 2; i++) {
            threads.emplace_back(&run, this);
        }
    };

    ~TransposeWorker() {
        quite = true;
        for (auto& thread : threads) {
            wake_up.notify_all();
            thread.join();
        }
    }

    std::future<Matrix> AsyncProcess(Matrix mtx) {
        int task_id = last_task++;

        AddTask(mtx, task_id);
        wake_up.notify_one();

        while(finished_tasks.count(task_id) == 0) {}
        return std::move(finished_tasks.at(task_id));
    }

private:
    std::queue<std::pair<Matrix, int>> tasks;
    std::vector<std::thread> threads;

    // flag if there's no tasks
    std::atomic<bool> quite{false};
    // num of last taken task
    std::atomic<int> last_task = 0;
    std::mutex task_mutex;
    // flag if all threads are asleep and new task has been got
    std::condition_variable wake_up;
    std::unordered_map<int, std::future<Matrix>> finished_tasks; 

    void run() {
        while (!quite) {
            std::unique_lock<std::mutex> lock(task_mutex);
            wake_up.wait(lock, [this]()->bool { return !tasks.empty() || quite; });
            
            if (!tasks.empty()) {
                auto cur_task = std::move(tasks.front());
                tasks.pop();
                lock.unlock();
                
                std::promise<Matrix> p;
                std::future<Matrix> f = p.get_future();
                finished_tasks[cur_task.second] = std::move(f);
                p.set_value(MakeTranspose(cur_task.first));
            }
        }
    }

    void AddTask(const Matrix& mtx, int task_id) {
        std::unique_lock q_lock(task_mutex);
        tasks.emplace(mtx, task_id);
    }
};

Matrix MakeTranspose(const Matrix& matrix) {
    Matrix transposed_matrix {
        std::vector<int>(matrix.data.size()),
        matrix.height,
        matrix.width
    };

    for (size_t i = 0; i < matrix.height; i++) {
        for (size_t j = 0; j < matrix.width; j++) {
            transposed_matrix.data[i+j*transposed_matrix.width] =
                matrix.data[i*matrix.width+j];
        }
    }
    
    return transposed_matrix;
}

std::shared_ptr<WorkerInterface> get_new_worker() {
    return std::shared_ptr<WorkerInterface>(new TransposeWorker());
}