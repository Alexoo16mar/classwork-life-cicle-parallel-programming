#include <thread>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <condition_variable>

std::atomic<bool> chopping(true);

void vegetable_chopper(const char* name) {
    unsigned int personal_count = 0;
    while (chopping) {
        personal_count++;
        std::this_thread::yield(); // Yield to give other thread a chance to run
    }
    printf("%s chopped %u vegetables.\n", name, personal_count);
}

std::mutex chop_mutex;
unsigned int shared_count = 0;
const unsigned int total_vegetables = 20;

void vegetable_chopper2(const char* name) {
    unsigned int personal_count = 0;

    while (true) {
        {
            std::lock_guard<std::mutex> lock(chop_mutex);
            if (!chopping) {
                chopping = false;
                break;
            }
            shared_count++;
            personal_count++;
            //std::cout << name << " chopped vegetable " << shared_count << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << name << " chopped " << personal_count << " vegetables in total.\n";
}

std::mutex cin_mutex;
void calculator() {
    int num1, num2;
    {
        std::lock_guard<std::mutex> lock(cin_mutex);
        std::cout << "Enter first number: ";
        std::cin >> num1;
        std::cout << "Enter second number: ";
        std::cin >> num2;
        std::cout << "The sum is: " << num1 + num2 << std::endl;
    }
}

std::mutex mtx;
std::condition_variable cv;
bool helloDone = false;
void say_hello() {
    while(true) {
        std::cout << "Hello" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        {
            std::lock_guard<std::mutex> lock(mtx);
            helloDone = true;
        }
        cv.notify_one();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void say_goodbye() {
    while(true) {
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, []{ return helloDone; });
            helloDone = false;
        }
        std::cout << "Goodbye" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main() {
    /*
    // Chopping vegetables
    std::thread olivia(vegetable_chopper2, "Olivia");
    std::thread barron(vegetable_chopper2, "Barron");
    printf("Barron and Olivia are chopping vegetables.. \n");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    chopping = false;
    barron.join();
    olivia.join();
    std::cout << "Total vegetables chopped: " << shared_count << std::endl;
    */

    /*
    // Calculator
    std::thread thread1(calculator);
    std::thread thread2(calculator);

    thread1.join();
    thread2.join();
    */

    /**/
    // Saying Hello and GoodBye
    std::thread hello_thread(say_hello);
    std::thread goodbye_thread(say_goodbye);

    hello_thread.join();
    goodbye_thread.join();
    /**/
}
