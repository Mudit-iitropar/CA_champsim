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
    uint32_t num_way=NUM_WAY;
    if(cold[set]&& cache_type==IS_LLC){
        num_way=9;
    }
    // fill invalid line first
    
    for (way=0; way<num_way; way++) {
        if (block[set][way].valid == false) {
            DP ( if (warmup_complete[cpu]) {
            cout << "[" << NAME << "] " << __func__ << " instr_id: " << instr_id << " invalid set: " << set << " way: " << way;
            cout << hex << " address: " << (full_addr>>LOG2_BLOCK_SIZE) << " victim address: " << block[set][way].address << " data: " << block[set][way].data;
            cout << dec << " lru: " << block[set][way].lru << endl; });
            return way;
        }
    }
    if(hot[set] && cache_type== IS_LLC){
        vector<int>temp;
        for(int i=0;i<temp.size();i++){
            for(int j=9;j<16;j++){
                if(!block[temp[i]][j].valid){
                    temp_set= temp[i];
                    real_set= set;
                    hot_set= true;
                    return j;
                }
            }
        }
    }

    int max_=num_way-1;
    // LRU victim
    if (way == num_way) {
        for (way=0; way<num_way; way++) {
            if (block[set][way].lru == max_) {

                DP ( if (warmup_complete[cpu]) {
                cout << "[" << NAME << "] " << __func__ << " instr_id: " << instr_id << " replace set: " << set << " way: " << way;
                cout << hex << " address: " << (full_addr>>LOG2_BLOCK_SIZE) << " victim address: " << block[set][way].address << " data: " << block[set][way].data;
                cout << dec << " lru: " << block[set][way].lru << endl; });
                return way;
                break;
            }
        }
    }
    
    if(hot[set] && cache_type==IS_LLC){
        max_=max_+7*7;
        int start=9;
        for(int i=0;i<assigned[set].size();i++){
            for (int j=start; i<NUM_WAY; j++) {
                int ii=assigned[set][i];
                if (block[ii][j].lru == max_) {
                    DP ( if (warmup_complete[cpu]) {
                    cout << "[" << NAME << "] " << __func__ << " instr_id: " << instr_id << " replace set: " << ii << " way: " << j;
                    cout << hex << " address: " << (full_addr>>LOG2_BLOCK_SIZE) << " victim address: " << block[ii][j].address << " data: " << block[ii][j].data;
                    cout << dec << " lru: " << block[ii][j].lru << endl; });
                    real_set=set;
                    temp_set= ii;
                    hot_set=true;
                    break;
                }
            }
        }
    }
    if (way == num_way) {
        cerr << "[" << NAME << "] " << __func__ << " no victim! set: " << set << endl;
        assert(0);
    }

    return way;
}

void CACHE::lru_update(uint32_t set, uint32_t way)
{
    // update lru replacement state
    int num_val= NUM_WAY;
    if(cold[set] && !hot_set && cache_type== IS_LLC){
        num_val= 9;
    }
    if(!hot_set){
    for (uint32_t i=0; i<NUM_WAY; i++) {
        if (block[set][i].lru < block[set][way].lru) {
            block[set][i].lru++;
        }
    }
    }
    if(hot[set] && cache_type==IS_LLC){
        vector<int>temp= assigned[set];
        for(int i=0;i<temp.size();i++){
            for(int j=9;j<16;j++){
                if(block[temp[i]][j].lru < block[set][way].lru){
                    block[temp[i]][j].lru++;
                }
            }
        }
    }
    if(cold[set] && hot_set && cache_type==IS_LLC){
        for(int i=0;i<16;i++){
            if(block[real_set][i].lru< block[set][way].lru ){
                block[real_set][i].lru++;
            }
        }
        vector<int>temp= assigned[real_set];
        for(int j=0;j<temp.size();j++){
            if(temp[j]==set)continue;

            for(int i=9;i<16;i++){
                if(block[temp[j]][i].lru < block[set][way].lru){
                    block[temp[j]][i].lru++;
                }
            }
        }
    }
    block[set][way].lru = 0; // promote to the MRU position
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
