#include <iostream>
#include <vector>
#include <array>
#include <set>
#include <utility>
#include <queue>
#include <ctime>
#include <map>
#include <string>

/*
 *           0
 *         0 0 0
 *       0 0 a 0 0
 *     0 0 b c d 0 0
 *   0 0 e f g h i 0 0
 * 0 0 0 0 0 0 0 0 0 0 0
 */

int n, center, area;
std::array<int, 3> counts;
std::vector<std::vector<int>> board;
std::set<std::array<int, 3>> all_counts;
long long total_count = 0;

int point_type[64];

std::map<std::string, int> mp;
std::vector<std::map<std::string, int>> mpp;

void init(){
    for(int i = 0; i < 64; i++) point_type[i] = -1;
    for(int i = 0; i < 6; i++){
        for(int j = i + 1; j < 6; j++){
            int x = (1<<j) - (1<<i);
            int t = (j - i < 3 ? j - i : 6 - (j - i)) - 1;
            point_type[x] = point_type[63-x] = t;
        }
    }
}

int get_enc(int x, int y){
    // (x, y), (x, y+1), (x, y+2), (x+1, y+2), (x+1, y+1), (x+1, y)
    return (
        (board[x][y])
        | (board[x][y+1] << 1)
        | (board[x][y+2] << 2)
        | (board[x+1][y+2] << 3)
        | (board[x+1][y+1] << 4)
        | (board[x+1][y] << 5)
    );
}

bool bfs(){
    std::vector<std::vector<int>> chkd(n+3, std::vector<int>(2*n+5));
    std::queue<std::pair<int,int>> q;
    q.emplace(2, n+2);
    chkd[2][n+2] = 1;
    int v_cnt = 0;
    while(!q.empty()){
        auto [x, y] = q.front(); q.pop();
        v_cnt++;
        int chk_cnt = 0;
        if(board[x][y-1]){
            if(chkd[x][y-1]) chk_cnt++;
            else{
                q.emplace(x, y-1);
                chkd[x][y-1] = 1;
            }
        }
        if(board[x][y+1]){
            if(chkd[x][y+1]) chk_cnt++;
            else{
                q.emplace(x, y+1);
                chkd[x][y+1] = 1;
            }
        }
        int nx = x + ((x + y + n) % 2 ? -1 : 1);
        if(board[nx][y]){
            if(chkd[nx][y]) chk_cnt++;
            else{
                q.emplace(nx, y);
                chkd[nx][y] = 1;
            }
        }
        if(chk_cnt > 1) return false;
    }
    if(v_cnt != (n+1) * (n+2) / 2 - 2) return false;
    return true;
}

void print_board(){
    std::cout << "--------------\n";
    for(int i = 0; i < n; i++){
        for(int j = 3; j < 2 * n + 2; j++){
            if(n+2-i <= j && j <= n+2+i) std::cout << board[2+i][j];
            else std::cout << " ";
        }
        std::cout << "\n";
    }
}

void process(int idx1, int idx2){
    // (0, 0), (1, 0), (1, 1), (1, 2), (2, 0), ..., (2, 4), (3, 0), ...
    if(area < 0) return;
    if(idx1 == n){
        if(area != 0) return;
        std::array<int, 3> tmp({0, 0, 0});
        for(int i = 0; i <= n; i++){
            int t = point_type[get_enc(1+n, 1+2*i)];
            if(t == -1) return;
            tmp[t]++;
        }
        if(!bfs()) return;
        // print_board();
        all_counts.insert({counts[0] + tmp[0], counts[1] + tmp[1], counts[2] + tmp[2]});
        total_count++;
        std::string no;
        for(int i = 0; i < 2 * n - 1; i++) no += (char)('0'+board[n+1][3+i]);
        if(!mp.count(no)) mp[no] = 0;
        mp[no]++;
        return;
    }
    if(idx2 == idx1 * 2){
        int t = point_type[get_enc(1+idx1, center+idx1-1)];
        if(t != -1){
            counts[t]++;
            process(idx1+1, 0);
            counts[t]--;
        }
        board[2+idx1][center-idx1+idx2] = 1;
        area--;
        t = point_type[get_enc(idx1+1, center+idx1-1)];
        if(t != -1){
            counts[t]++;
            process(idx1+1, 0);
            counts[t]--;
        }
        board[2+idx1][center-idx1+idx2] = 0;
        area++;
        return;
    }
    if(idx2 % 2 == 1){
        int t = point_type[get_enc(1+idx1, center-idx1+idx2-2)];
        if(t != -1){
            counts[t]++;
            process(idx1, idx2+1);
            counts[t]--;
        }
        board[2+idx1][center-idx1+idx2] = 1;
        area--;
        t = point_type[get_enc(1+idx1, center-idx1+idx2-2)];
        if(t != -1){
            counts[t]++;
            process(idx1, idx2+1);
            counts[t]--;
        }
        board[2+idx1][center-idx1+idx2] = 0;
        area++;
        return;
    }
    process(idx1, idx2+1);
    board[2+idx1][center-idx1+idx2] = 1;
    area--;
    process(idx1, idx2+1);
    board[2+idx1][center-idx1+idx2] = 0;
    area++;
    return;
}

int main(){
    init();
    clock_t time_old, time_new;
    std::cin >> n;
    mpp.resize(n);
    time_old = clock();
    board.resize(n+3, std::vector<int>(2*n+5));
    center = n+2;
    area = (n+1) * (n+2) / 2 - 2;
    process(0, 0);
    time_new = clock();
    std::cout << total_count << "\n";
    for(auto a : all_counts) std::cout << a[0] << " " << a[1] << " " << a[2] << "\n";
    for(auto [key, val] : mp) std::cout << key << " " << val << "\n";
    std::cout << "time : " << time_new - time_old << " / " << CLOCKS_PER_SEC << "(s)\n";
}