#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

//----------------------------------------------------------------
// Implementation of the reader-writer lock using condition
// variable and a mutex.
// The implementation is write-preferring.
//----------------------------------------------------------------

std::condition_variable cv;
int readerCount = 0;
int writersWaiting = 0;
bool writing = false;

void task(){
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // simulate the reader/writer task
}

template<typename Mutex>
class reader_lock{
public:
    explicit reader_lock(Mutex &m){
        std::unique_lock<std::mutex> lock(m);
        while (writersWaiting > 0 || writing) {
            cv.wait(lock);
        }
        readerCount++;
        lock.unlock();
    }
    ~reader_lock(){
        readerCount--;
        if (readerCount == 0){
            cv.notify_all();
        }
    }
};
template<typename Mutex>
class writer_lock{
public:
    explicit writer_lock(Mutex &m){
        std::unique_lock<std::mutex> lock(m);
        writersWaiting++;
        while (readerCount > 0 || writing) {
            cv.wait(lock);
        }
        writersWaiting--;
        writing = true;
        lock.unlock();
    }
    ~writer_lock(){
        writing = false;
        cv.notify_all();
    }
};


std::mutex someMtx;

void read(int id){
    reader_lock<std::mutex> rd_lock(someMtx);
    std::cout << "Reader " << id << " acquired the lock" << std::endl;
    task();
    std::cout << "Reader " << id << " has released the lock" << std::endl;
}

void write(int id){
    writer_lock<std::mutex> wr_lock(someMtx);
    std::cout << "Writer " << id << " acquired the lock" << std::endl;
    task();
    std::cout << "Writer " << id << " released the lock" << std::endl;
}

int main()
{
    std::thread t1(read, 1);
    std::thread t2(read, 2);
    std::thread t3(read, 3);
    std::thread t4(write, 1);
    std::thread t5(write, 2);
    std::thread t6(write, 3);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();

    return 0;
}
