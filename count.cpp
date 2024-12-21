#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <map>
#include <set>
#include <ctime>
#include <array>
#include <cassert>
#include <algorithm>

using int_t = long long int;

char start_ch = '0', end_ch = '~';

/* state
 * 000ppp00
 * 0AAppp00 (idx = 0, upd = 1-2, ref = 0-3) 
 * 0aaAAp00 (idx = 1, upd = 3-4, ref = 2-5)
 * 0aaaaA00 (idx = 2, upd = 5-6, ref = 4-7)
 * 000aaaaa00
 */

std::set<int> flag_set;

std::string extract_state(std::string st){
    char cur = start_ch;
    std::map<char, char> mp;
    std::string ret = "";
    for(char c : st){
        if(!mp.count(c)){
            mp[c] = cur;
            cur++;
        }
        ret += mp[c];
    }
    return ret;
}

std::vector<std::pair<std::string, int>> next_states(std::string st, int idx, bool islast){
    // returns all possible (next state, corner type)
    // corner type: 0(60-degree), 1(120-degree), 2(180-degree)
    std::vector<std::pair<std::string, int>> ret;
    int l = st.size();
    int flag = (st[2*idx] != start_ch) + (st[2*idx+1] != start_ch) * 2 + (st[2*idx+2] != start_ch) * 4 + (st[2*idx+3] != start_ch) * 8;
    char p;
    bool hascomp;
    switch (flag){
    case 5:
    case 10:
        // impossible
        break;
    case 9:
    case 11:
    case 13:
        // 11 (must connect, check if ends were different component)
        // component change: merge two components
        if(islast) break;
        if(st[2*idx] != st[2*idx+3]){
            p = st[2*idx+3];
            for(int i = 0; i < l; i++) if(st[i] == p) st[i] = st[2*idx];
            st[2*idx+1] = st[2*idx+2] = st[2*idx];
            ret.push_back({extract_state(st), (flag == 9 ? 1 : 0)});
        }
        break;
    case 2:
    case 4:
    case 6:
        // 00 (no other component)
        // if component is removed, impossible (component always has to exist until the very last add)
        p = st[2*idx+1] | st[2*idx+2];
        hascomp = false;
        for(int i = 0; i < 2*idx; i++) if(st[i] == p) hascomp = true;
        for(int i = 2*idx+4; i < l; i++) if(st[i] == p) hascomp = true;
        if(hascomp){
            st[2*idx+1] = st[2*idx+2] = start_ch;
            ret.push_back({extract_state(st), (flag == 6 ? 1 : 0)});
        }
        break;
    case 0:
        // 01, 10, 11
        // component change: new component added
        st[2*idx+1] = end_ch;
        ret.push_back({extract_state(st), 0});
        if(!islast){
            st[2*idx+2] = end_ch;
            ret.push_back({extract_state(st), 1});
            st[2*idx+1] = start_ch;
            ret.push_back({extract_state(st), 0});
        }
        break;
    case 1:
    case 3:
    case 7:
        // 00, 10, 11
        // no component change
        st[2*idx+1] = st[2*idx+2] = start_ch;
        ret.push_back({extract_state(st), (flag == 1 ? 0 : (flag == 3 ? 1 : 2))});
        st[2*idx+1] = st[2*idx];
        ret.push_back({extract_state(st), (flag == 1 ? 1 : (flag == 3 ? 2 : 1))});
        if(!islast){
            st[2*idx+2] = st[2*idx];
            ret.push_back({extract_state(st), (flag == 1 ? 2 : (flag == 3 ? 1 : 0))});
        }
        break;
    case 8:
    case 12:
    case 14:
        // 00, 01, 11
        // no component change
        st[2*idx+1] = st[2*idx+2] = start_ch;
        ret.push_back({extract_state(st), (flag == 8 ? 0 : (flag == 12 ? 1 : 2))});
        if(!islast){
            st[2*idx+2] = st[2*idx+3];
            ret.push_back({extract_state(st), (flag == 8 ? 1 : (flag == 12 ? 2 : 1))});
            st[2*idx+1] = st[2*idx+3];
            ret.push_back({extract_state(st), (flag == 8 ? 2 : (flag == 12 ? 1 : 0))});
        }
        break;
    case 15:
        // 00, 01, 10
        // no component change
        st[2*idx+1] = st[2*idx+2] = start_ch;
        ret.push_back({extract_state(st), 1});
        st[2*idx+1] = st[2*idx];
        ret.push_back({extract_state(st), 0});
        if(!islast){
            st[2*idx+2] = st[2*idx];
            st[2*idx+1] = start_ch;
            ret.push_back({extract_state(st), 0});
        }
    }
    return ret;
}

int get_info(int x){
    if(x == 1 || x == 8) return 0;
    if(x == 3 || x == 12 || x == 15) return 1;
    return 2;
}

std::pair<std::string, int> next_empty_state(std::string st, int idx){
    int l = st.size();
    int flag = (st[2*idx] != start_ch) + (st[2*idx+1] != start_ch) * 2 + (st[2*idx+2] != start_ch) * 4 + (st[2*idx+3] != start_ch) * 8;
    // std::cout << st << " " << flag << " ";
    char p;
    bool hascomp;
    switch (flag){
    case 1:
    case 3:
    case 7:
    case 8:
    case 12:
    case 14:
    case 15:
        st[2*idx+1] = st[2*idx+2] = start_ch;
        return {extract_state(st), get_info(flag)};
        break;
    case 2:
    case 4:
    case 6:
        // if component is removed, impossible (component always has to exist until the very last add)
        p = st[2*idx+1] | st[2*idx+2];
        hascomp = false;
        for(int i = 0; i < 2*idx; i++) if(st[i] == p) hascomp = true;
        for(int i = 2*idx+4; i < l; i++) if(st[i] == p) hascomp = true;
        if(hascomp){
            st[2*idx+1] = st[2*idx+2] = start_ch;
            return {extract_state(st), (flag == 6 ? 1 : 0)};
        }
        break;
    }
    return {"", -1};
}

std::map<std::string, std::array<int, 6>> mp[2]; // {min(c1), max(c1), min(c2), max(c2), min(c3), max(c3)}
bool mp_idx;

void update_mp(std::string st, std::array<int, 6> val){
    if(!mp[mp_idx].count(st)){
        mp[mp_idx][st] = val;
        return;
    }
    mp[mp_idx][st][0] = std::min(mp[mp_idx][st][0], val[0]);
    mp[mp_idx][st][1] = std::max(mp[mp_idx][st][1], val[1]);
    mp[mp_idx][st][2] = std::min(mp[mp_idx][st][2], val[2]);
    mp[mp_idx][st][3] = std::max(mp[mp_idx][st][3], val[3]);
    mp[mp_idx][st][4] = std::min(mp[mp_idx][st][4], val[4]);
    mp[mp_idx][st][5] = std::max(mp[mp_idx][st][5], val[5]);
}

int main(){
    int n;
    clock_t time_old, time_new;
    std::cin >> n;
    time_old = clock();
    std::string init_state(6, start_ch);
    init_state[3] = start_ch+1;
    mp[mp_idx][init_state] = {1, 1, 0, 0, 0, 0};
    for(int i = 1; i < n; i++){
        for(int j = 0; j < i; j++){
            mp_idx = !mp_idx;
            mp[mp_idx].clear();
            for(auto [key, val] : mp[!mp_idx]){
                for(auto [st, typ] : next_states(key, j, false)){
                    auto new_val = val;
                    new_val[2*typ]++; new_val[2*typ+1]++;
                    update_mp(st, new_val);
                }
            }
        }
        mp_idx = !mp_idx;
        mp[mp_idx].clear();
        for(auto [key, val] : mp[!mp_idx]){
            for(auto [st, typ] : next_states(key, i, true)){
                st = std::string(2, start_ch) + st;
                auto new_val = val;
                new_val[2*typ]++; new_val[2*typ+1]++;
                update_mp(st, new_val);
            }
        }
        // std::cout << "------" << i << "------\n";
        // for(auto [key, val] : mp[mp_idx]){
        //     std::cout << key << " " << val << "\n";
        // }
    }
    for(int j = 0; j < n; j++){
        mp_idx = !mp_idx;
        mp[mp_idx].clear();
        for(auto [key, val] : mp[!mp_idx]){
            auto [st, typ] = next_empty_state(key, j);
            if(typ != -1){
                auto new_val = val;
                new_val[2*typ]++; new_val[2*typ+1]++;
                update_mp(st, new_val);
            }
        }
    }
    time_new = clock();
    // std::cout << "----------------------\n";
    assert(mp[mp_idx].size() == 1);
    for(auto [key, val] : mp[mp_idx]){
        std::cout << val[0]+1 << " " << val[1]+1 << " " << val[2] << " " << val[3] << " " << val[4] << " " << val[5] << "\n";
    }
    std::cout << "time : " << time_new - time_old << " / " << CLOCKS_PER_SEC << "(s)\n";
}