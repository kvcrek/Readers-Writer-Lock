#include <iostream>
#include <thread>
#include <mutex>

//----------------------------------------------------------------
// Implementation of the reader-writer lock using two mutexes.
// The implementation is read-preferring.
//----------------------------------------------------------------

std::mutex m_mutex;
std::mutex m_writerLock;
int readerCount = 0;


void task(){
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // simulate the reader/writer task
}

void reader_lock(){
    m_mutex.lock();
    readerCount++;
    if (readerCount == 1){
        m_writerLock.lock();
    }
    m_mutex.unlock();
}

void reader_unlock(){
    m_mutex.lock();
    readerCount--;
    if (readerCount == 0) {
        m_writerLock.unlock();
    }
    m_mutex.unlock();
}


void writer_lock() {
    m_writerLock.lock();
}

void writer_unlock(){
    m_writerLock.unlock();
}

void read(int id){
    std::cout << "Reader " << id << " is attempting to acquire lock" << std::endl;
    reader_lock();
    std::cout << "Reader " << id << " acquired the lock" << std::endl;
    task();
    reader_unlock();
    std::cout << "Reader " << id << " released the lock" << std::endl;
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