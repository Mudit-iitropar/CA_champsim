#include "cache.h"

uint32_t CACHE::find_victim(uint32_t cpu, uint64_t instr_id, uint32_t set, const BLOCK *current_set, uint64_t ip, uint64_t full_addr, uint32_t type)
{
    // baseline LRU replacement policy for other caches 
    return lru_victim(cpu, instr_id, set, current_set, ip, full_addr, type); 
}

void CACHE::update_replacement_state(uint32_t cpu, uint32_t set, uint32_t way, uint64_t full_addr, uint64_t ip, uint64_t victim_addr, uint32_t type, uint8_t hit)
{
    if (type == WRITEBACK) {
        if (hit) // wrietback hit does not update LRU state
            return;
    }

    return lru_update(set, way);
}

uint32_t CACHE::lru_victim(uint32_t cpu, uint64_t instr_id, uint32_t set, const BLOCK *current_set, uint64_t ip, uint64_t full_addr, uint32_t type)
{
    uint32_t way = 0;

    // fill invalid line first
    int num_way=NUM_WAY;
    if(cold[set] && cache_type== IS_LLC){
        num_way=9;
    }
    for (way=0; way<num_way; way++) {
        if (block[set][way].valid == false) {
            DP ( if (warmup_complete[cpu]) {
            cout << "[" << NAME << "] " << __func__ << " instr_id: " << instr_id << " invalid set: " << set << " way: " << way;
            cout << hex << " address: " << (full_addr>>LOG2_BLOCK_SIZE) << " victim address: " << block[set][way].address << " data: " << block[set][way].data;
            cout << dec << " lru: " << block[set][way].lru << endl; });
            break;
        }
    }
    temp_set=set;
    // map<int,vector<pair<int,int>>>
    if(hot[set] && cache_type== IS_LLC){
            vector<pair<int,int>> set_=rk1[set];
                for(auto it= set_.begin();it!= set_.end();it++){
                    int temp= it->first;
                    int temp2= it->second;
                    for(int i= NUM_WAY-temp2-1;i>=10;i--){
                        if(!block[temp][i].valid){
                            way =i;
                            temp_set= temp;
                        }
                    }
                }
            
        }
    // LRU victim
    uint32_t max_=num_way-1;
    if(hot[set] && cache_type== IS_LLC){
        // upper limit=6 in each cold set
        max_=num_way+6*5-1;
    }
    if (way == num_way) {
        for (way=0; way<num_way; way++) {
            if (block[set][way].lru == max_) {
                DP ( if (warmup_complete[cpu]) {
                cout << "[" << NAME << "] " << __func__ << " instr_id: " << instr_id << " replace set: " << set << " way: " << way;
                cout << hex << " address: " << (full_addr>>LOG2_BLOCK_SIZE) << " victim address: " << block[set][way].address << " data: " << block[set][way].data;
                cout << dec << " lru: " << block[set][way].lru << endl; });
                break;
            }
        }   
    }
    if(hot[set] && cache_type== IS_LLC){
        vector<pair<int,int>> set_=rk1[set];
        for(auto x:set_){
            uint32_t rrr=x.first;
            for (way=10; way<num_way; way++) {
                if (block[rrr][way].lru == max_) {
                    DP ( if (warmup_complete[cpu]) {
                    cout << "[" << NAME << "] " << __func__ << " instr_id: " << instr_id << " replace set: " << rrr << " way: " << way;
                    cout << hex << " address: " << (full_addr>>LOG2_BLOCK_SIZE) << " victim address: " << block[rrr][way].address << " data: " << block[rrr][way].data;
                    cout << dec << " lru: " << block[rrr][way].lru << endl; });
                    temp_set=rrr;
                    break;
                }
            }
        }
    }
    if (way == NUM_WAY) {
        cerr << "[" << NAME << "] " << __func__ << " no victim! set: " << set << endl;
        assert(0);
    }

    return way;
}

void CACHE::lru_update(uint32_t set, uint32_t way)
{
    // update lru replacement state
    // hot_set tells if a hot set has sent it or not
    uint32_t num_way=NUM_WAY,start=0;
    if(cold[set] && cache_type== IS_LLC && hot_set){
        start=10;
    }
    else if(cold[set] && cache_type== IS_LLC){
        num_way=9;
    }
    for (uint32_t i=start; i<num_way; i++) {
        if (block[set][i].lru < block[set][way].lru) {
            block[set][i].lru++;
        }
    }
    if(hot_set){
        for (uint32_t i=0; i<NUM_WAY; i++) {
            if (block[real_set][i].lru < block[set][way].lru) {
                block[real_set][i].lru++;
            }
        }
        if(rk1.find(real_set)==rk1.end()){
            block[set][way].lru = 0;
            return;
        }
        vector<pair<int,int>> set_=rk1[real_set];
        for(auto x:set_){
        uint32_t rrr=x.first,temp=x.second;
        if(rrr==set){
            continue;
        }
        for(way=num_way-1;way>=num_way-temp;way--){
            if(block[rrr][way].lru<block[set][way].lru){
                block[rrr][way].lru++;
            }
        }
    }
    }
    block[set][way].lru = 0;
    return;
    // if(rk1.find(set)==rk1.end()){
    //     block[set][way].lru = 0;
    //     return;
    // }
    // vector<pair<int,int>> set_=rk1[real_set];
    // for(auto x:set_){
    //     uint32_t rrr=x.first,temp=x.second;
    //     for(way=num_way-1;way>=num_way-temp;way--){
    //         if(block[rrr][way].lru<block[set][way].lru){
    //             block[rrr][way].lru++;
    //         }
    //     }
    // }
}

void CACHE::replacement_final_stats()
{

}

#ifdef NO_CRC2_COMPILE
void InitReplacementState()
{
    
}

uint32_t GetVictimInSet (uint32_t cpu, uint32_t set, const BLOCK *current_set, uint64_t PC, uint64_t paddr, uint32_t type)
{
    return 0;
}

void UpdateReplacementState (uint32_t cpu, uint32_t set, uint32_t way, uint64_t paddr, uint64_t PC, uint64_t victim_addr, uint32_t type, uint8_t hit)
{
    
}

void PrintStats_Heartbeat()
{
    
}

void PrintStats()
{

}
#endif
