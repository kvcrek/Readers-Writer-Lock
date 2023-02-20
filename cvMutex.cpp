#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

//----------------------------------------------------------------
// Implementation of the reader-writer lock using condition
// variable and a mutex.
// The implementation is writ-preferring.
//----------------------------------------------------------------

std::mutex m_mutex;
std::condition_variable cv;
int readerCount = 0;
int writersWaiting = 0;
bool writing = false;

void task(){
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // simulate the reader/writer task
}

void reader_lock(){
    std::unique_lock<std::mutex> lock(m_mutex);
    while (writersWaiting > 0 || writing) {
        cv.wait(lock);
    }
    readerCount++;
    lock.unlock();
}

void reader_unlock(){
    std::unique_lock<std::mutex> lock(m_mutex);
    readerCount--;
    if (readerCount == 0){
        cv.notify_all();
    }
    lock.unlock();
}

void writer_lock() {
    std::unique_lock<std::mutex> lock(m_mutex);
    writersWaiting++;
    while (readerCount > 0 || writing) {
        cv.wait(lock);
    }
    writersWaiting--;
    writing = true;
    lock.unlock();
}

void writer_unlock(){
    std::unique_lock<std::mutex> lock(m_mutex);
    writing = false;
    cv.notify_all();
    lock.unlock();
}


void read(int id){
    std::cout << "Reader " << id << " is attempting to acquire lock" << std::endl;
    reader_lock();
    std::cout << "Reader " << id << " acquired the lock" << std::endl;
    task();
    reader_unlock();
    std::cout << "Reader " << id << " has released the lock" << std::endl;
}

void write(int id){
    std::cout << "Writer " << id << " is attempting to acquire lock" << std::endl;
    writer_lock();
    std::cout << "Writer " << id << " acquired the lock" << std::endl;
    task();
    writer_unlock();
    std::cout << "Writer " << id << " released the lock" << std::endl;
}

int main()
{
    std::thread t1(read, 1);
    std::thread t2(read, 2);
    std::thread t3(read, 3);
    std::thread t4(write, 1);
    std::thread t5(write, 2);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();


    return 0;
}