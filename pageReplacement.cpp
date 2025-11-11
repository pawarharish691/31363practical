#include<bits/stdc++.h>
#include <chrono>
#include <thread>
using namespace std;

void fifo(vector<int>&pages,int framesize){ // pages // framesize = size memory holding

    unordered_set<int> frames;
    queue<int> pageOrder;
 
    int pageFaults = 0 ;

    for(int i = 0 ; i < pages.size();i++){
        int page = pages[i] ;
        
        if(frames.find(page) == frames.end()){
            pageFaults++ ;

            if(frames.size() < framesize){
                frames.insert(page);
                pageOrder.push(page);
            }
            else{
                int oldestPage = pageOrder.front(); 
                pageOrder.pop();
                frames.erase(oldestPage) ;
                frames.insert(page);
                pageOrder.push(page);
            }

        }
        cout<<"Frames :" ; 
        for(const auto &frame : frames){
            cout<<frame <<" ";
            // this_thread::sleep_for(chrono::milliseconds(500));

        }
        cout << endl;



    } 
    cout <<"total page faults: " << pageFaults <<endl;



}
int main(){
    int n , framesize ;
    cout << "Enter the number of pages " ;
    cin >> n ; 

    vector<int> pages(n) ;
    cout <<"Enter the page refernce string(space separeted):" ; 

    for(int i = 0; i< n ;i++ )
        cin>>pages[i];

        cout<<"Enter the number of frames(memory size):" ; 
        cin >> framesize;

        fifo(pages,framesize) ; 
        return 0 ; 
    
}