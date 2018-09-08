// This was just to test conditional variables.
// Error handling is missing.

#include <iostream>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

class MyThreadClass
{
public:
   MyThreadClass() {/* empty */}
   virtual ~MyThreadClass() {/* empty */}

   bool runChecker() {
      return (pthread_create(&m_CheckerThread, NULL, checkerThreadEntryFunc, this) == 0);
   }
   bool runModifier() {
      return (pthread_create(&m_ModifierThread, NULL, modifierThreadEntryFunc, this) == 0);
   }
   void WaitForCheckerThreadToExit() {
       (void) pthread_join(m_CheckerThread, NULL);
   }
   void WaitForModifierThreadToExit() {
       (void) pthread_join(m_ModifierThread, NULL);
   }

protected:
   virtual void checkerThreadEntry() = 0;
   virtual void modifierThreadEntry() = 0;

private:
   static void * checkerThreadEntryFunc(void * This) {
       ((MyThreadClass *)This)->checkerThreadEntry();
       return NULL;
   }
   static void * modifierThreadEntryFunc(void * This) {
       ((MyThreadClass *)This)->modifierThreadEntry();
       return NULL;
   }

   pthread_t m_CheckerThread;
   pthread_t m_ModifierThread;
};

class testConditional : public MyThreadClass{
public:
    void checkerThreadEntry();
    void modifierThreadEntry();

private:
    std::string m_Done;
    bool m_ready = false;
};


void testConditional::modifierThreadEntry()
{
    std::string test = "foo";
//    std::cout << "enter value:" << std::endl;
//    std::cin >> test;

    pthread_mutex_lock( &mutex );
    std::cout << "[thread modifier] done:" << m_Done << std::endl;
    m_Done = test;
    m_ready = true;
    std::cout << "[thread modifier] done:" << m_Done << std::endl;
    pthread_cond_signal( &cond );
    std::cout << "[thread modifier] signalling!" << std::endl;
    pthread_mutex_unlock( & mutex );
}

void testConditional::checkerThreadEntry()
{
    pthread_mutex_lock( &mutex );
    std::cout << "[thread checker] done:" << m_Done << std::endl;
    while(!m_ready) {
        pthread_cond_wait( & cond, & mutex );
        std::cout << "[thread checker] signal received!" << std::endl;
    }
    std::cout << "[thread checker] done:" << m_Done << std::endl;
    pthread_mutex_unlock( & mutex );
}

int main( int argc, char** argv )
{
    puts( "[thread main] started");
    testConditional c;
    c.runChecker();
    c.runModifier();

    c.WaitForModifierThreadToExit();
    c.WaitForCheckerThreadToExit();

    return 0;
}
