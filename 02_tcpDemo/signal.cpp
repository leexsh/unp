#include <signal.h>
#include <sys/types.h>
using namespace std;

typedef void Sigfunc(int); 
// 信号处理函数
Sigfunc* signal(int singno, Sigfunc *func){
    struct sigaction act, oact;
    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if(singno == SIGALRM){
        #ifdef SA_INTERRUPT
            act.sa_flags |= SA_INTERRUPT;
        #endif
    }
    else{
        #ifdef SA_RESTART
            act.sa_flags |= SA_RESTART;
        #endif
    }
    if(sigaction(singno, &act, &oact) < 0){
        return (SIG_ERR);
    }
    return (oact.sa_handler);
}
