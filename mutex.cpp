#include<iostream>
#include<chrono>
#include<thread>
#include<condition_variable>
#include<mutex>
#include<bits/stdc++.h>
using namespace std ;

queue<int>buffer ; 
const int buffersize = 5 ; 
mutex mtx ;
condition_variable notfull,notEmpty ;

void producer(){

    int item = 0 ;
    while(item < 10 ){
        unique_lock<mutex> lock(mtx) ; 
        notfull.wait(lock,[]{return buffer.size() < buffersize});

        item++ ;
        buffer.push(item) ; 
        cout<<"Producer produced: " <<item <<endl;

        lock.unlock();
        notEmpty.notify_one();
        this_thread::sleep_for(chrono::milliseconds(500));

    }

}
void consumer(){
    int item ; 
    while(true){
          unique_lock<mutex> lock(mtx) ; 
        notfull.wait(lock,[]{return buffer.size() < buffersize});

        item = buffer.front();
        buffer.pop() ; 
        cout<<"consumer produced : " << item << endl;

        lock.unlock();
        notEmpty.notify_one();
        this_thread::sleep_for(chrono::milliseconds(1000));

        if(item == 10) break ;

    } 
}

int main(){
    thread prod(producer) ;
    thread cons(consumer);

    prod.join();
    cons.join() ;
    cout<<"Simulation finished.\n" ; 
    return 0 ;
}






