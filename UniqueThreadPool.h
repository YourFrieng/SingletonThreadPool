#include <thread>
#include <queue>
#include <mutex>
#include <functional>
#include <chrono>
#include <vector>
#include <atomic>

using std::chrono::operator""ms;

using TaskQueue = std::queue< std::function< void() > >;
using ThreadVector = std::vector< std::thread >;

class UniqueThreadPool
{
public:
    UniqueThreadPool( UniqueThreadPool &other ) = delete;
    ~UniqueThreadPool( );
    void operator=( const UniqueThreadPool & ) = delete;
    static UniqueThreadPool* getpool( );
    void add_task( std::function< void() > );
    void start( );
    void end( );
    
private:
    UniqueThreadPool( );
    static bool m_created;
    std::atomic<bool> m_stop = false;
    std::mutex m_mutex_queue;
    TaskQueue m_tasks;
    ThreadVector m_threads;
    void do_tasks( );

protected:
    static UniqueThreadPool* thread_pool;
};

UniqueThreadPool* UniqueThreadPool::thread_pool = nullptr;

UniqueThreadPool::UniqueThreadPool( )
:m_threads( std::thread::hardware_concurrency( ) )
{}

UniqueThreadPool::~UniqueThreadPool( )
{
    thread_pool->end( );
}


UniqueThreadPool* UniqueThreadPool::getpool( )
{
    if ( thread_pool == nullptr )
    {
        thread_pool = new UniqueThreadPool( );
    }
    
    return thread_pool;
}

void UniqueThreadPool::add_task( std::function< void() > function  )
{
    std::lock_guard< std::mutex > lock( m_mutex_queue );
    m_tasks.push( function );
}

void UniqueThreadPool::end( )
{
    m_stop = true;
    for (auto& th : m_threads)
    {
        if( th.joinable( ) )
        {
            th.join( );
        } 
        else
        {
            th.detach( );
        }
    }   
}
//implement into if c++17 functional
void UniqueThreadPool::do_tasks( )
{
    while ( !m_stop )
    {
        std::unique_lock locker( m_mutex_queue );
        if( m_tasks.empty( ) )
        {
            locker.unlock( );
            std::this_thread::sleep_for( 500ms );
            continue;
        }
        auto fun = m_tasks.front( );
        m_tasks.pop( );
        locker.unlock( );
        fun( );
    }
    
}

void UniqueThreadPool::start( )
{
    for (auto& th : m_threads)
    {
        th = std::thread( &UniqueThreadPool::do_tasks, this );
    }   
}
