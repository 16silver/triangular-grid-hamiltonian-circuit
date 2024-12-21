#include <iostream>
#include <vector>
#include <array>
#include <queue>

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
std::vector<std::vector<int>> typ;

int point_type[64];

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

int main(){
    init();
    std::cin >> n;
    board.resize(n+3, std::vector<int>(2*n+5));
    typ.resize(n+1, std::vector<int>(n+1));
    center = n+2;
    area = (n+1) * (n+2) / 2 - 2;
    for(int i = 0; i < n; i++){
        for(int j = 0; j <= 2*i; j++){
            std::cin >> board[2+i][center-i+j];
            if(board[2+i][center-i+j]) area--;
        }
    }
    if(area){
        std::cout << "area does not match\n";
        return 0;
    }
    if(!bfs()){
        std::cout << "not a connected tree\n";
        return 0;
    }
    for(int i = 0; i <= n; i++){
        for(int j = 0; j <= i; j++){
            int t = point_type[get_enc(1+i, center-i+2*j-1)];
            if(t == -1){
                std::cout << "point (" << i << ", " << j << ") is invalid\n";
                return 0;
            }
            counts[t]++;
            typ[i][j] = t;
        }
    }
    std::cout << counts[0] << " " << counts[1] << " " << counts[2] << "\n";
    for(int i = 0; i <= n; i++){
        for(int j = 0; j <= i; j++) std::cout << typ[i][j]+1 << " ";
        std::cout << "\n";
    }
}