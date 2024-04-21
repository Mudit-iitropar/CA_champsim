#include "cache.h"

#define maxRRPV 3
uint32_t rrpv[2048][16];

// initialize replacement state
void CACHE::llc_initialize_replacement()
{
    cout<<"LLC "<<LLC_SET<<endl;
    cout<<maxRRPV<<endl;
    cout << "Initialize SRRIP state" << endl;
    cout<<"checkpoint\n";
    for (int i=0; i<LLC_SET-1; i++) {
        for (int j=0; j<LLC_WAY; j++) {
            if(i>=2047){
                cout<<"LLC_WAY "<<LLC_WAY<<" "<<LLC_SET<<endl;
                cout<<i<<" "<<j<<endl;
            }
            rrpv[i][j] = maxRRPV;
            if(i>=2047){
                cout<<"CCC\n";
            }
        }
        if(i>=2047){
            cout<<"BBB\n";
        }
    }
    cout<<"AAA\n";
}

// find replacement victim
uint32_t CACHE::llc_find_victim(uint32_t cpu, uint64_t instr_id, uint32_t set, const BLOCK *current_set, uint64_t ip, uint64_t full_addr, uint32_t type)
{
    temp_set = set;
    // look for the maxRRPV line
    while (1)
    {   
        int num_ways=LLC_WAY;
        if(cold[set] && cache_type==IS_LLC){
            num_ways=9;
        }

        for (int i=0; i<num_ways; i++)
            if (rrpv[set][i] == maxRRPV)
                return i;
        if(hot[set] && cache_type==IS_LLC){
            vector<int>temp= assigned[set];
        for(int j=0;j<temp.size();j++){
            int set_= temp[j];
            for(int i=9;i<num_ways;i++){
                    if(rrpv[set_][i] == maxRRPV){
                        temp_set= set_;
                        return i;
                    }
            }
        }
        }

        for (int i=0; i<LLC_WAY; i++)
            rrpv[set][i]++;
        if(hot[set] && cache_type==IS_LLC){
            vector<int>temp= assigned[set];
        for(int j=0;j<temp.size();j++){
            int set_= temp[j];
            for(int i=9;i<num_ways;i++){
                    rrpv[set_][i]++;
            }
        }
        }
    }

    // WE SHOULD NOT REACH HERE
    assert(0);
    return 0;
}

// called on every cache hit and cache fill
void CACHE::llc_update_replacement_state(uint32_t cpu, uint32_t set, uint32_t way, uint64_t full_addr, uint64_t ip, uint64_t victim_addr, uint32_t type, uint8_t hit)
{
    string TYPE_NAME;
    if (type == LOAD)
        TYPE_NAME = "LOAD";
    else if (type == RFO)
        TYPE_NAME = "RFO";
    else if (type == PREFETCH)
        TYPE_NAME = "PF";
    else if (type == WRITEBACK)
        TYPE_NAME = "WB";
    else
        assert(0);

    if (hit)
        TYPE_NAME += "_HIT";
    else
        TYPE_NAME += "_MISS";

    if ((type == WRITEBACK) && ip)
        assert(0);

    // uncomment this line to see the LLC accesses
    // cout << "CPU: " << cpu << "  LLC " << setw(9) << TYPE_NAME << " set: " << setw(5) << set << " way: " << setw(2) << way;
    // cout << hex << " paddr: " << setw(12) << paddr << " ip: " << setw(8) << ip << " victim_addr: " << victim_addr << dec << endl;
    
    if (hit)
        rrpv[set][way] = 0;
    else
        rrpv[set][way] = maxRRPV-1;
}

// use this function to print out your own stats at the end of simulation
void CACHE::llc_replacement_final_stats()
{

}
