#ifndef MEMORY_REASSIGN_H
#define MEMORY_REASSIGN_H

#include <unordered_map>
#include <iostream>
#define PAGE_COUNT 1000
#define SAMPLE_MAX_SIZE 8192
#define RTD_SIZE 100000
#define SAMPLE_ARR_SZ 10003
#define domain 256
#define STEP 10
#define MAX_PROGRESS_CNT 10
using namespace std;
typedef unsigned long long uint64_t;
struct history
{
    int sample_start;
    int start;
};

class memory_reassign
{
    public:
        memory_reassign(int, uint64_t);
        virtual ~memory_reassign();
        void access(uint64_t addr, int progress_id);
        void change_to_rtd(int progress_id);
        void clear_record();
        void get_mrc(int progress_id);
        void get_new_alloc();
        void show_mrc(int progress_id);
        void show_rtd(int progress_id);
        void show_new_alloc();
        void show_all();
        void state_switching();

    protected:
    private:
        uint64_t max_page;
        uint64_t alloc_page;
        int progress_cnt;
        int memory_size;
        uint64_t old_alloc[MAX_PROGRESS_CNT];
        uint64_t new_alloc[MAX_PROGRESS_CNT];
        int gap[MAX_PROGRESS_CNT];
        int sample_cnt[MAX_PROGRESS_CNT];
        int sample_tot_cnt[MAX_PROGRESS_CNT];
        int repeat_addr[MAX_PROGRESS_CNT];
        unordered_map<uint64_t, history> mp[MAX_PROGRESS_CNT];
        unordered_map<uint64_t, history>::iterator sample_list[MAX_PROGRESS_CNT][SAMPLE_ARR_SZ];
        int sample_rtd[MAX_PROGRESS_CNT][SAMPLE_ARR_SZ];
        int my_access[MAX_PROGRESS_CNT];
        int rtd[MAX_PROGRESS_CNT][SAMPLE_ARR_SZ];
        double mrc[MAX_PROGRESS_CNT][RTD_SIZE];

        int domain_value_to_index(int value)
        {
            int loc = 0,step = 1;
            int index = 0;
            while (loc+step*domain<value) {
            loc += step*domain;
            step *= 2;
            index += domain;
            }
            while (loc<value) index++,loc += step;
            return index;
        }
};

#endif // MEMORY_REASSIGN_H
