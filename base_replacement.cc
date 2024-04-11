#include "cache.h"
// #include "cache.cc"

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
    if(cold[set]){
        num_way=9;
    }
    // fill invalid line first
    for (way=0; way<num_way; way++) {
        if (block[set][way].valid == false) {

            DP ( if (warmup_complete[cpu]) {
            cout << "[" << NAME << "] " << __func__ << " instr_id: " << instr_id << " invalid set: " << set << " way: " << way;
            cout << hex << " address: " << (full_addr>>LOG2_BLOCK_SIZE) << " victim address: " << block[set][way].address << " data: " << block[set][way].data;
            cout << dec << " lru: " << block[set][way].lru << endl; });

            break;
        }
    }
    if(hot[set]){
      for(uint32_t set_: rk1[set]){
        for (way=9; way<num_way; way++) {
            if (block[set_][way].valid == false) {

                DP ( if (warmup_complete[cpu]) {
                cout << "[" << NAME << "] " << __func__ << " instr_id: " << instr_id << " invalid set: " << set_ << " way: " << way;
                cout << hex << " address: " << (full_addr>>LOG2_BLOCK_SIZE) << " victim address: " << block[set][way].address << " data: " << block[set][way].data;
                cout << dec << " lru: " << block[set][way].lru << endl; });

                break;
            }
        }
      }
    }
    // LRU victim
    uint32_t max_=16;
    if(hot[set]){
        max_=16+rk1[set].size()*7;
    }
    else if(cold[set]){
        max_=9;
    }
    bool yes=false;
    if (way == num_way) {
        for (way=0; way<num_way; way++) {
            if (block[set][way].lru == max_-1) {

                DP ( if (warmup_complete[cpu]) {
                cout << "[" << NAME << "] " << __func__ << " instr_id: " << instr_id << " replace set: " << set << " way: " << way;
                cout << hex << " address: " << (full_addr>>LOG2_BLOCK_SIZE) << " victim address: " << block[set][way].address << " data: " << block[set][way].data;
                cout << dec << " lru: " << block[set][way].lru << endl; });
                yes=true;
                break;
            }
        }
        if(!yes){
            // it means no victim found in very hot set till now 
            for(uint32_t set_: rk1[set]){
                for (way=9; way<num_way; way++) {
                    if (block[set_][way].lru == max_-1) {

                        DP ( if (warmup_complete[cpu]) {
                        cout << "[" << NAME << "] " << __func__ << " instr_id: " << instr_id << " replace set: " << set_ << " way: " << way;
                        cout << hex << " address: " << (full_addr>>LOG2_BLOCK_SIZE) << " victim address: " << block[set_][way].address << " data: " << block[set_][way].data;
                        cout << dec << " lru: " << block[set_][way].lru << endl; });
                        // yes=true;
                        break;
                    }
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
    uint32_t num_way=NUM_WAY;
    if(cold[set]){
        num_way=9;
    }
    for (uint32_t i=0; i<num_way; i++) {
        if (block[set][i].lru < block[set][way].lru) {
            block[set][i].lru++;
        }
    }
    if(hot[set]){
        for(uint32_t set_: rk1[set]){
            for (uint32_t i=9; i<num_way; i++) {
                if (block[set_][i].lru < block[set_][way].lru) {
                    block[set_][i].lru++;
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
