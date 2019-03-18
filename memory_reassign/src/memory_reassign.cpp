#include "memory_reassign.h"
using namespace std;

memory_reassign::memory_reassign(int progress_cnt, uint64_t memory_size)
{
    assert(progress_cnt <= memory_size);
    srand(time(NULL));
    memset(sample_cnt, 0, sizeof(sample_cnt));
    memset(sample_tot_cnt, 0, sizeof(sample_tot_cnt));
    memset(repeat_addr, 0, sizeof(repeat_addr));
    memset(my_access, 0, sizeof(my_access));
    this->progress_cnt = progress_cnt;
    this->memory_size = memory_size;

    memset(sample_rtd, 0, sizeof(sample_rtd));
    memset(rtd, 0, sizeof(rtd));
    for(int i = 0; i < progress_cnt; ++i)
    {
        gap[i] = rand() % (STEP * 2) + 1;
    }
    memset(mrc, 0, sizeof(mrc));
}

memory_reassign::~memory_reassign()
{
    //dtor

}


void memory_reassign::clear_record()
{
    for(int i = 0; i < progress_cnt; ++i)
        mp[i].clear();
    memset(sample_rtd, 0, sizeof(sample_rtd));
    memset(sample_cnt, 0, sizeof(sample_cnt));
    memset(sample_tot_cnt, 0, sizeof(sample_tot_cnt));
    memset(repeat_addr, 0, sizeof(repeat_addr));
    memset(my_access, 0, sizeof(my_access));
    memset(rtd, 0, sizeof(rtd));
    memset(mrc, 0, sizeof(mrc));
    for(int i = 0; i < progress_cnt; ++i)
    {
        gap[i] = rand() % (STEP * 2) + 1;
        new_alloc[i] = 0;
    }

}

void memory_reassign::access(uint64_t addr, int progress_id)  //Page Number
{
    ++my_access[progress_id];
    if(mp[progress_id].find(addr) != mp[progress_id].end())
    {
        sample_rtd[progress_id][mp[progress_id][addr].sample_start] = my_access[progress_id] - mp[progress_id][addr].start;
        mp[progress_id].erase(addr);

    }

    if(my_access[progress_id] == gap[progress_id])
    {
        sample_tot_cnt[progress_id]++;
        int loc = sample_cnt[progress_id];
        if(sample_cnt[progress_id] == SAMPLE_MAX_SIZE)
        {
            loc = rand() % sample_tot_cnt[progress_id];
            if(loc >= sample_cnt[progress_id])
            {
                gap[progress_id] += rand() % (STEP * 2) + 1;
                return;
            }
            unordered_map<uint64_t, history>::iterator iter = sample_list[progress_id][loc];
            sample_rtd[progress_id][loc] = 0;
            if(mp[progress_id].find(iter->first) != mp[progress_id].end())
                mp[progress_id].erase(iter->first);
        }
        else
        {
            sample_cnt[progress_id]++;
        }

        history his;
        his.sample_start = loc;
        his.start = my_access[progress_id];
        mp[progress_id][addr] = his;
        unordered_map<uint64_t, history>::iterator iter = mp[progress_id].find(addr);
        sample_list[progress_id][loc] = iter;
        gap[progress_id] += rand() % (STEP * 2) + 1;

    }
}


void memory_reassign::change_to_rtd(int progress_id)
{
    for(int i = 0; i < sample_cnt[progress_id]; ++i)
    {
        if(sample_rtd[progress_id][i] != 0)
        {
            repeat_addr[progress_id]++;
            rtd[progress_id][domain_value_to_index(sample_rtd[progress_id][i])] += 1;
        }
    }
}

void memory_reassign::get_mrc(int progress_id)
{
    int time = 0;
    double accum = 0;
    double sum = 0;
    int loc = 0;
    int dom = 0;
    int dt = 0;
    int step = 1;
    int total_uniq_pg = 1.0*(sample_cnt[progress_id] - repeat_addr[progress_id]) / sample_cnt[progress_id] * my_access[progress_id];
    int max_alloc_size = total_uniq_pg;
    for(int alloc_size = 1; alloc_size <= max_alloc_size; ++alloc_size)
    {
        while(time <= my_access[progress_id] && accum/sample_cnt[progress_id] < alloc_size)
        {
            accum += sample_cnt[progress_id] - sum;
            ++time;
            if (time>loc)
            {
                if (++dom>domain) dom = 1,step *= 2;
                loc += step;
                dt++;
            }
            sum += 1.0*rtd[progress_id][dt]/step;

        }

        mrc[progress_id][alloc_size] = 1 - sum / sample_cnt[progress_id];
    }
    mrc[progress_id][0] = 1;
}


void memory_reassign::get_new_alloc()
{
    double* f[MAX_PROGRESS_CNT];
    uint64_t* alloc_saver[MAX_PROGRESS_CNT];
    for(int i = 0; i <= progress_cnt; ++i)
    {
        f[i] = new double[memory_size + 1];
        alloc_saver[i] = new uint64_t[memory_size + 1];
        for(uint64_t j = 0; j <= memory_size; ++j)
        {
            f[i][j] = 1e11;
        }
    }

    for(int i = 0; i <= memory_size; ++i)
    {
        f[0][i] = 0;
    }
    for(int i = 1; i <= progress_cnt; ++i)
    {
        for(uint64_t j = 1; j <= memory_size; ++j)
        {

            for(uint64_t k = 0; k <= j; ++k)
            {
                double tmp = f[i - 1][j - k] + mrc[i - 1][k] * my_access[i - 1];
                if(tmp < f[i][j])
                {
                    f[i][j] = tmp;
                    alloc_saver[i][j] = k;
                }
            }
        }
    }

    uint64_t temp = memory_size;
    for(int i = progress_cnt; i > 0; --i)
    {
        new_alloc[i - 1] = alloc_saver[i][temp];
        temp -= alloc_saver[i][temp];
    }

    for(int i = 0; i <= progress_cnt; ++i)
    {
        delete [] f[i];
        delete [] alloc_saver[i];
    }
}

void memory_reassign::show_all()
{
    cout<<"mrc info:"<<endl;
    for(int i = 0; i < progress_cnt; ++i)
    {
        cout<<"progress id "<<i<<":"<<endl;
        for(int j = 1; j <= memory_size; ++j)
        {
            cout<<mrc[i][j]<<" ";
        }

        cout<<endl;
    }

    cout<<endl;
    cout<<endl;
    cout<<"new alloc info:"<<endl;
    for(int i = 0; i < progress_cnt; ++i)
    {
        cout<<new_alloc[i]<<" ";
    }
    cout<<endl;
    cout<<endl;
}

void memory_reassign::state_switching()
{
    for(int i = 0; i < progress_cnt; ++i)
    {
        change_to_rtd(i);
        get_mrc(i);
        get_new_alloc();
    }
}
