#include "delta_time.h"

using namespace std;

void gfg::delta_time::reset()
{
    delta_ = 0;
    last_ = chrono::system_clock::now();
}

void gfg::delta_time::update()
{
    auto now_tmp = chrono::system_clock::now();
    delta_ = chrono::duration<double>(now_tmp - last_).count();
    last_ = now_tmp;
}
