#include <iostream>
#include "UniqueThreadPool.h"

void Task1( )
{
        
    std::this_thread::sleep_for(1000ms);
    std::cout << "Hello, World!" << std::endl;
}

void Task2( )
{
    std::this_thread::sleep_for(100ms);
    std::cout << 25 - 15 << std::endl;
}

void Task3( )
{
    std::this_thread::sleep_for(500ms);
    std::cout << 25 * 10 << std::endl;
}


void Task4( )
{
    std::this_thread::sleep_for(50ms);
    std::cout << "Task4" << std::endl;
}


int main( )
{
 
    UniqueThreadPool* th1 = UniqueThreadPool::getpool( );
    th1->start( );
    for (size_t i = 0; i < 500; i++)
    {
        if( i % 2 == 0)
            th1->add_task( Task1 );
        else
            th1->add_task( Task2 );
    }

    for (size_t i = 0; i < 500; i++)
    {
        if( i % 2 == 0)
            th1->add_task( Task2 );
        else
            th1->add_task( Task3 );
    }

    for (size_t i = 0; i < 500; i++)
    {
        if( i % 2 == 0)
            th1->add_task( Task3 );
        else
            th1->add_task( Task1 );
    }
    

    for (size_t i = 0; i < 200; i++)
    {
        th1->add_task( Task4 );

    }
    auto start = std::chrono::high_resolution_clock::now();
    char k;
    std::cin >> k;
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end-start;
    std::cout << "Waited " << elapsed.count() << " ms\n";
    delete th1;
    return 0;
}
