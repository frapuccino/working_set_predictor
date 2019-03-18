#include <iostream>
#include <fstream>
#include "memory_reassign.h"
using namespace std;

uint64_t trans2dec(string addr){
    uint64_t n=0;
    while(addr.size()>0){
        char ch=addr[0];
        uint64_t ch_n;
        addr=addr.substr(1,addr.size()-1);
        if(ch>57){ch_n=ch-97+10;}
        else{ch_n=ch-48;}
        n=n*16+ch_n;
    }
    return n;
}

int main()
{
    memory_reassign* mr = new memory_reassign(4, 1001);
    ifstream ifs("/Users/huangxing/Documents/code/tracein0.txt");
    string str1, str2;
    int odds = 0, evens = 0;
    while(ifs >> str1 >> str2)
    {
        uint64_t addr = trans2dec(str1);
        uint64_t id = trans2dec(str2) % 4;
        if(id & 1) ++odds;
        else ++evens;
        uint64_t page_num = addr >> 12;
        mr->access(page_num, id);
    }
    cout<<"end"<<endl;
    mr->state_switching();
    mr->show_all();
    mr->clear_record();
    mr->show_all();
    return 0;
}
