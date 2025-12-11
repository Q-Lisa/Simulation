//
// Created by 86183 on 2025/3/19.
//

#ifndef BISHE_THREAD_POOL_H
#define BISHE_THREAD_POOL_H

#include<iostream>
#include"sys/time.h"

using namespace std;

class Timer{
public:
    //启动计时器
    void startTime(){
        gettimeofday(&start,nullptr);
    }
    //关闭计时器
    void endTime() {
        gettimeofday(&end, nullptr);
        float duration = (float) (end.tv_sec - start.tv_sec) + (float) (end.tv_usec - start.tv_usec) / 1000000.0;
        timeval += duration;
        onetimeval = duration;
    }
    void showTime(const char* content){
        cout<<content<<" time duration: "<<timeval<<"s."<<endl;
    }
public:
    float timeval=0;
    float onetimeval=0;
private:
    struct timeval start{};  //开始时间
    struct timeval end{}; //结束时间
};

#endif //BISHE_THREAD_POOL_H
