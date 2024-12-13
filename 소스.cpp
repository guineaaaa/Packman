// PackMan_1.cpp : Defines the entry point for the console application.
// 
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

using namespace std;

const int map_row_size = 20;      // 가로줄이 20개 이고, 세로줄이 40개 이므로
const int map_column_size = 40;      // 옆으로 넓은 사각형 형태를 이룬다

char bfs_map[map_row_size][map_column_size];  // BFS 방법으로 path 찾을 때 사용함


char map[map_row_size][map_column_size] = {   // 맵
    // N-Gate @ 
    "##*####################################",
    "#    #                    #           #", //E-Gate @
    "#         dd   #          #           *",
    "####    ####   ####     ###     ## ####",
    "#        b b                          #",
    "#    ###       ####    h   ##     #   #",
    "#    ###       #           #####      #",
    "# #    #  #    #   ####        #      #",
    "# #    #  #     #     #    #   #    ###",
    "# ######  #     #     #    #   ###    #",
    "#            ####             #       #",
    "#  ####         ##         ####       #",
    "#                    ##               #",
    "####   ##     ##     ##       ###  ## #",
    "#      #         #        #     #     #",
    "##    #   ####   ##  #  ####    #     #",
    "#     ##  a                #     f f ##",
    "###    #  a  ######        ###        #",
    "*                 ##               ####", //W-Gate @
    "################################*######"
    //S-Gate @

};


// 화면에다 맵을 보여준다
void display_map() {
    int i;

    for (i = 0; i <= map_row_size - 1; i++) {
        printf("%s\n", map[i]);
    }
}


// 커서를 특정 위치로 이동한다
// 수정하지 말 것. !!!!!         이대로 사용할 것. !!!!!
// short 자료형 그대로 사용한다.  int 사용하니까 경고 뜬다
void gotoxy(short x, short y) {
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD position = { x, y };

    SetConsoleCursorPosition(hStdout, position);
}


// BFS 방법으로 길을 찾을 때 사용하는 노드의 구조체
struct path_node {
    int x;         // x 위치
    int y;         // y 위치
    int parent_id;   // 부모 id number
    // 이것을 사용하여 길을 찾을 때, 이전 위치로 이동하게 된다
};

// 위치를 나타내는 구조체
struct position {
    int x;
    int y;
};


vector<path_node> bfs_list;      // 시작위치부터 목적지까지의 길을 찾기 위해서
// BFS 방법으로 만들어지는 리스트
// 적의 위치에서 플레이어 위치 방향으로 저장한다
// Z 위치를 보면, bfs_list.push_back() 함수를 사용하므로
// 뒤에다 넣기 때문에  적의 위치에서 플레이어의 위치 방향으로 저장한다

vector<position> enemy_path;   // 적이 실제로 이동할 path
// 플레이어 위치에서 적 위치 방향으로 저장한다
// Y 위치를 보면, enemy_path.push_back() 함수를 사용하므로
// 뒤에다 넣기 때문에  플레이어의 위치에서 적의 위치 방향으로 저장한다
//
// main() 안에 있는 X 위치를 보면
// enemy_path가  back()와 pop_back()를 사용하므로
// 뒤에서 앞쪽으로 값을 읽어오기 때문에 
// 적의 위치에서 플레이어의 위치 방향 순서가 된다 
// vector <bomb> bombs;

struct bomb {
    int x, y;
    chrono::time_point<chrono::steady_clock>place_time;
    bool is_active;
};

// 시한 폭탄을 설치하는 함수
void place_bomb(int ex, int ey, vector<bomb>& bombs) {
    if (bombs.size() < 3) {
        bomb new_bomb;
        new_bomb.x = ex; new_bomb.y = ey;
        new_bomb.place_time = chrono::steady_clock::now();
        new_bomb.is_active = true;
        bombs.push_back(new_bomb);

        map[ey][ex] = 'T'; // 'T'로 시한 폭탄 표시
       
        gotoxy(ex, ey);
        system("cls");
        display_map();
    }
}

// 시한 폭탄 폭발 처리
void explode_bomb(bomb& b) {
    auto now = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(now - b.place_time).count();
    if (duration >=15 && b.is_active) {
        b.is_active = false;
        map[b.y][b.x] = ' ';  // 폭탄 제거
        system("cls");
        display_map();

        // 주변 아이템 제거
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                if (b.x + dx >= 0 && b.x + dx < map_column_size && b.y + dy >= 0 && b.y + dy < map_row_size) {
                    if (map[b.y + dy][b.x + dx] == '.' || map[b.y + dy][b.x + dx] == '#') {
                        map[b.y + dy][b.x + dx] = ' '; // 아이템과 벽 제거
                        system("cls");
                        display_map();
                    }
                }
            }
        }
    }
}

// 'I' 상태에서의 변경
void change_player_state(char& player, bool& bomb_enabled) {
    if (GetAsyncKeyState(0x49)) {  // 'I' 키를 눌렀을 경우
        if (player == 'P') {
            player = 'I';  // 플레이어 상태를 'I'로 변경
            bomb_enabled = true;

        }
        else if (player == 'I') {
            player = 'P';  // 'I'에서 'P'로 복귀
            bomb_enabled = false;
        }
    }
}

// 시한 폭탄 해제
void deactivate_bomb(int x, int y, vector<bomb>& bombs, int& score) {
    for (auto it = bombs.begin(); it != bombs.end(); ++it) {
        if (it->x == x && it->y == y && it->is_active) {
            it->is_active = false;
            map[it->y][it->x] = ' ';
            score += 10;  // 점수 추가
            system("cls");
            display_map();
            break;
        }
    }
}


// 해당 위치 (x, y)가 비어있는 경우에는 새로운 노드를 리스트에 추가한다
// x와 y는 현재 위치이고,  id는 부모 노드의 id 번호이다
// FindPath()에서 4번 호출한다
void add_new_node_to_bfs_list(int x, int y, int id) {
    path_node node;

    // 현재의 위치 (x,y)가  빈 공간이거나 아이템이어서  이동이 가능하다면
    if (bfs_map[y][x] == ' ' || bfs_map[y][x] == '.') {
        bfs_map[y][x] = '#';      // 현재 그 위치를 방문한 것으로 표시한다

        node.x = x;
        node.y = y;
        node.parent_id = id;
        bfs_list.push_back(node);   // Z 위치.  
        // bfs_list 끝에다 추가하므로
        // 적의 위치에서 플레이어 위치 방향으로 저장한다
    }
}


// 적 위치 (ex,ey) 부터 플레이어 위치인 내 위치 (x,y) 까지의 길을 찾는다
void FindPath(int ex, int ey, int x, int y) {
    path_node node;
    position pos;
    int j, k;
    int index;


    // 길을 찾기 위해서 map 정보를 bfs_map에 복사한다
    for (k = 0; k <= map_row_size - 1; k++) {
        for (j = 0; j <= map_column_size - 1; j++) {
            bfs_map[k][j] = map[k][j];
        }
    }

    // (A)  bfs_list를 새것으로 만든다
    bfs_list.clear();

    // (B)  bfs_list 첫번째 위치인 [0]에다  적의 현재 위치를 넣는다.
    // 아래 4줄이 모두 다 (B)에 해당한다
    node.x = ex;
    node.y = ey;
    node.parent_id = -1;      // 적의 현재 위치의 부모는 없어서 -1로 한다
    bfs_list.push_back(node);   // bfs_list 끝에다 추가한다

    // (C)  bfs_list에서 [0] 위치의 노드부터 실행한다
    index = 0;

    // bfs list의 노드들을 순서대로 하나씩 방문하면서
    while (index <= bfs_list.size() - 1) {   // (D)

        // 플레이어인 나의 위치 (x,y)까지 도달했다면
        if (bfs_list[index].x == x && bfs_list[index].y == y) {      // (E)

            enemy_path.clear();  // (P)  적의 이동경로를 저장하는 enemy_path를 비어있는 상태로 모두 지워서 초기화 한다

            // 적의 현재 위치가 저장되어 있는 [0] 위치에 도달하지 않은 동안에
            while (bfs_list[index].parent_id != -1) {   // (Q)

                // (R)  먼저, bfs list의 현재 노드의 위치 정보를 
                //      pos 변수에 저장한 후에            
                pos.x = bfs_list[index].x;
                pos.y = bfs_list[index].y;

                // (S)  pos 변수를  적의 이동경로인 enemy_path list 끝에다 넣는다
                enemy_path.push_back(pos);  // Y 위치.   
                // 적의 이동경로인 enemy_path list 끝에다 추가시킨다
                // 그래서, 플레이어의 위치에서 적의 위치 방향으로 저장한다

    // (T)  부모 노드로 이동함으로써  한단계 이전단계로 이동한다 !!!!!
                index = bfs_list[index].parent_id;
            }

            // (U)  첫번째 while문을 벗어나서  아래에 있는 A 위치로 간다
            goto U_Exit;
        }

        // (F)  아직 내 위치 (x,y)까지 도달하지 못한 경우에는 
        //      현재 위치의 상/하/좌/우 위치를 각각 조사해서 추가시킨다
        //      index는  bfs_list의 현재 노드의 인덱스이다
        //      아래 네 줄이 각각 F1, F2, F3, F4 이다
        add_new_node_to_bfs_list(bfs_list[index].x, bfs_list[index].y - 1, index);
        add_new_node_to_bfs_list(bfs_list[index].x, bfs_list[index].y + 1, index);
        add_new_node_to_bfs_list(bfs_list[index].x - 1, bfs_list[index].y, index);
        add_new_node_to_bfs_list(bfs_list[index].x + 1, bfs_list[index].y, index);

        // (G)  bfs_list의 다음번 노드로 이동한다
        index++;
    }

    // A 위치.  위의 break 문으로 while을 벗어나서 여기로 온다
U_Exit:
    bfs_list.clear();
}

int enemy_travelled = 0;
//===========================================
//  M A I N   P R O G R A M 
//===========================================
int main()
{
    bool game_is_running_now = true;
    bool bomb_enabled = false; //시한 폭탄 활성화 상태
    vector <bomb> bombs; //시한 폭탄 목록

    int x = 15; // 플레이어인 나의 시작 위치 x
    int y = 16; // 플레이어인 나의 시작 위치 y

    int old_x;  // 플레이어인 나의 과거 위치 x
    int old_y;  // 플레이어인 나의 과거 위치 y

    int ex = 2; // 적 시작 위치 x
    int ey = 2; // 적 시작 위치 y

    int game_speed = 7;  // 초기값으로 쉬운 경우임
    int score = 100;         // 점수 = 획득한 코인 갯수
    int count = 0;         // 게임이 매번 실행되는 횟수

    char player = 'P';      // 플레이어
    char enemy = 'E';      // enemy, 적

    int enemy_speed = game_speed; // 적의 기본 속도 (5)
    int original_enemy_speed = game_speed; // 적의 원래 속도


    system("cls");
    display_map();  // 화면에 맵을 보인다

    gotoxy(x, y); cout << player;  // player를 보인다

    FindPath(ex, ey, x, y);         // 게임 시작하면 enemy가 player에게로 길을 찾아서 접근한다

    /////////////////////////////////////
    //  G A M E   M A I N   P A R T 
    /////////////////////////////////////


    const position n_gate = { 2, 0 };
    const position s_gate = { 32, 19 }; // 문제
    const position w_gate = { 0, 18 };
    const position e_gate = { 38, 2 };

    while (game_is_running_now) {

        old_x = x;
        old_y = y;

        // 'I' 키를 눌렀을 때 플레이어 상태 변경
        change_player_state(player, bomb_enabled);

        if (player == 'I') {
            // 적 이동 처리 (예시로 적이 빈 공간에서 이동한다고 가정)
            if (map[ey][ex] == ' ') {
                enemy_travelled++;  // 적이 빈 공간을 지나갈 때마다 카운트 증가
            }
            // 적이 10칸을 지나갈 때마다 폭탄 배치
            if (enemy_travelled >= 10) {
                place_bomb(ex, ey, bombs);  // 현재 적의 위치에 폭탄 배치
                enemy_travelled = 0;  // 카운트를 초기화
            }

            // 시한폭탄 폭발 처리
            for (auto& b : bombs) {
                explode_bomb(b);
            }

            // 'I' 상태일 때 스페이스바를 눌러 폭탄 해제
            if (bomb_enabled && GetAsyncKeyState(VK_SPACE)) {
                if ((map[y][x - 1] == 'T') || (map[y][x + 1] == 'T')) {
                    if (map[y][x - 1] == 'T') {
                        // 왼쪽에 폭탄이 있으면 해당 폭탄 해제
                        deactivate_bomb(x - 1, y, bombs, score);
                    }
                    else if (map[y][x + 1] == 'T') {
                        // 오른쪽에 폭탄이 있으면 해당 폭탄 해제
                        deactivate_bomb(x + 1, y, bombs, score);
                    }
                }
                
            }
        }
        // S 키를 눌렀을 경우
        if (GetAsyncKeyState(0x53)) {
            player = 'S';
        }

        // R 키를 눌렀을 경우
        if (GetAsyncKeyState(0x52)) {
            player = 'R';
        }


        // Player를 이동시키는 방향 키
        if (GetAsyncKeyState(VK_UP)) {
            if (map[y - 1][x] == '.' || map[y - 1][x] == ' ') y--;
            else if (y - 1 == n_gate.y && x == n_gate.x) { y = s_gate.y; x = s_gate.x; } // N 게이트에서 S 게이트로 이동


            // 'p' 일 경우에만 벽을 움직일 수 있다.
            else if (player == 'P') {
                if ((map[y - 1][x] == '#') && (map[y - 2][x] == ' ') && (y - 2 > 0)) { // 배열 내부에 있는 '#' 이고, 외벽이 아닌 경우 (배열 범위 안 인 경우)s
                    y--; // 이동
                    map[y][x] = ' ';
                    map[y - 1][x] = '#'; // 이동 된 한칸 위로 '#'를 이동 시킨다.

                    system("cls");
                    display_map();
                }
                // 대문자 이동 시키기
                else if ((map[y - 1][x] == 'A') && (map[y - 2][x] == ' ') && (y - 2 > 0)) { // 글자 이동
                    y--; // 이동
                    map[y][x] = ' ';
                    map[y - 1][x] = 'A';

                    system("cls");
                    display_map();
                }
                else if ((map[y - 1][x] == 'B') && (map[y - 2][x] == ' ') && (y - 2 > 0)) { // 글자 이동
                    y--; // 이동
                    map[y][x] = ' ';
                    map[y - 1][x] = 'B';

                    system("cls");
                    display_map();
                }
                else if ((map[y - 1][x] == 'D') && (map[y - 2][x] == ' ') && (y - 2 > 0)) { // 글자 이동
                    y--; // 이동
                    map[y][x] = ' ';
                    map[y - 1][x] = 'D';

                    system("cls");
                    display_map();
                }
                else if ((map[y - 1][x] == 'F') && (map[y - 2][x] == ' ') && (y - 2 > 0)) { // 글자 이동
                    y--; // 이동
                    map[y][x] = ' ';
                    map[y - 1][x] = 'F';

                    system("cls");
                    display_map();
                }

                // 소문자 이동 시키기
                else if ((map[y - 1][x] == 'a') && (map[y - 2][x] == ' ') && (y - 2 > 0)) { // 글자 이동
                    y--; // 이동
                    map[y][x] = ' ';
                    map[y - 1][x] = 'a';

                    system("cls");
                    display_map();
                }

                else if ((map[y - 1][x] == 'b') && (map[y - 2][x] == ' ') && (y - 2 > 0)) { // 글자 이동
                    y--; // 이동
                    map[y][x] = ' ';
                    map[y - 1][x] = 'b';

                    system("cls");
                    display_map();
                }
                else if ((map[y - 1][x] == 'd') && (map[y - 2][x] == ' ') && (y - 2 > 0)) { // 글자 이동
                    y--; // 이동
                    map[y][x] = ' ';
                    map[y - 1][x] = 'd';

                    system("cls");
                    display_map();
                }
                else if ((map[y - 1][x] == 'f') && (map[y - 2][x] == ' ') && (y - 2 > 0)) { // 글자 이동
                    y--; // 이동
                    map[y][x] = ' ';
                    map[y - 1][x] = 'f';

                    system("cls");
                    display_map();
                }
                // 소문자 + 소문자 대문자
                else if ((map[y - 1][x] == 'a') && (map[y - 2][x] == 'a') && (y - 2 > 0)) {
                    y--;
                    map[y][x] = ' ';
                    map[y - 1][x] = 'A';
                    system("cls");
                    display_map();
                }
                else if ((map[y - 1][x] == 'b') && (map[y - 2][x] == 'b') && (y - 2 > 0)) {
                    y--; // 이동
                    map[y][x] = ' ';
                    map[y - 1][x] = 'B';
                    system("cls");
                    display_map();
                }
                else if ((map[y - 1][x] == 'd') && (map[y - 2][x] == 'd') && (y - 2 > 0)) {
                    y--; // 이동
                    map[y][x] = ' ';
                    map[y - 1][x] = 'D';
                    system("cls");
                    display_map();
                }
                else if ((map[y - 1][x] == 'f') && (map[y - 2][x] == 'f') && (y - 2 > 0)) {
                    y--; // 이동
                    map[y][x] = ' ';
                    map[y - 1][x] = 'F';
                    system("cls");
                    display_map();
                }

                // '#'을 넣기
                else if ((map[y - 1][x] == '#') && (map[y - 2][x] == '*')) {
                    y--;
                    map[y][x] = ' ';
                    map[y - 1][x] = '*';

                    system("cls");
                    display_map();

                    score += 2;

                }

                else if ((map[y - 1][x] == 'A') && (map[y - 2][x] == '*')) { y--; map[y][x] = ' '; map[y - 1][x] = '*'; system("cls"); display_map(); score += 4; }
                else if ((map[y - 1][x] == 'B') && (map[y - 2][x] == '*')) { y--; map[y][x] = ' '; map[y - 1][x] = '*'; system("cls"); display_map(); score += 4; }
                else if ((map[y - 1][x] == 'D') && (map[y - 2][x] == '*')) { y--; map[y][x] = ' '; map[y - 1][x] = '*'; system("cls"); display_map(); score += 4; }
                else if ((map[y - 1][x] == 'F') && (map[y - 2][x] == '*')) { y--; map[y][x] = ' '; map[y - 1][x] = '*'; system("cls"); display_map(); score += 4; }
            }
        }

        if (GetAsyncKeyState(VK_DOWN)) {
            if (map[y + 1][x] == '.' || map[y + 1][x] == ' ') y++;
            else if (y + 1 == s_gate.y && x == s_gate.x) { y = n_gate.y; x = n_gate.x; } // S 게이트에서 N 게이트로 이동

            // 'P'인 경우에만 벽을 움직일 수 있다.
            if (player == 'P') {
                if ((map[y + 1][x] == '#') && (map[y + 2][x] == ' ') && (y + 2 < 20)) { // 배열 내부에 있는 '#' 이고, 외벽이 아닌 경우 (배열 범위 안 인 경우)
                    y++; // 이동
                    map[y][x] = ' ';
                    map[y + 1][x] = '#'; // 이동 된 한칸 밑로 '#'를 이동 시킨다.

                    system("cls");
                    display_map();
                }
                // 대문자 이동 시키기
                else if ((map[y + 1][x] == 'A') && (map[y + 2][x] == ' ') && (y + 2 > 0)) { // 글자 이동
                    y++;// 이동
                    map[y][x] = ' ';
                    map[y + 1][x] = 'A';

                    system("cls");
                    display_map();
                }
                else if ((map[y + 1][x] == 'B') && (map[y + 2][x] == ' ') && (y + 2 > 0)) { // 글자 이동
                    y++; // 이동
                    map[y][x] = ' ';
                    map[y + 1][x] = 'B';

                    system("cls");
                    display_map();
                }
                else if ((map[y + 1][x] == 'D') && (map[y + 2][x] == ' ') && (y + 2 > 0)) { // 글자 이동
                    y++; // 이동
                    map[y][x] = ' ';
                    map[y + 1][x] = 'D';

                    system("cls");
                    display_map();
                }
                else if ((map[y + 1][x] == 'F') && (map[y + 2][x] == ' ') && (y + 2 > 0)) { // 글자 이동
                    y++; // 이동
                    map[y][x] = ' ';
                    map[y + 1][x] = 'F';

                    system("cls");
                    display_map();
                }

                // 소문자 이동 시키기
                else if ((map[y + 1][x] == 'a') && (map[y + 2][x] == ' ') && (y + 2 > 0)) { // 글자 이동
                    y++;
                    map[y][x] = ' ';
                    map[y + 1][x] = 'a';

                    system("cls");
                    display_map();
                }
                else if ((map[y + 1][x] == 'b') && (map[y + 2][x] == ' ') && (y + 2 > 0)) { // 글자 이동
                    y++;
                    map[y][x] = ' ';
                    map[y + 1][x] = 'b';

                    system("cls");
                    display_map();
                }
                else if ((map[y + 1][x] == 'd') && (map[y + 2][x] == ' ') && (y + 2 > 0)) { // 글자 이동
                    y++;
                    map[y][x] = ' ';
                    map[y + 1][x] = 'd';

                    system("cls");
                    display_map();
                }
                else if ((map[y + 1][x] == 'f') && (map[y + 2][x] == ' ') && (y + 2 > 0)) { // 글자 이동
                    y++;
                    map[y][x] = ' ';
                    map[y + 1][x] = 'f';

                    system("cls");
                    display_map();
                }
                // 소문자 + 소문자 대문자
                else if ((map[y + 1][x] == 'a') && (map[y + 2][x] == 'a') && (y + 2 > 0)) {
                    y++;
                    map[y][x] = ' ';
                    map[y + 1][x] = 'A';
                    system("cls");
                    display_map();
                }
                else if ((map[y + 1][x] == 'b') && (map[y + 2][x] == 'b') && (y + 2 > 0)) {
                    y++;
                    map[y][x] = ' ';
                    map[y + 1][x] = 'B';
                    system("cls");
                    display_map();
                }
                else if ((map[y + 1][x] == 'd') && (map[y + 2][x] == 'd') && (y + 2 > 0)) {
                    y++;
                    map[y][x] = ' ';
                    map[y + 1][x] = 'D';
                    system("cls");
                    display_map();
                }
                else if ((map[y + 1][x] == 'f') && (map[y + 2][x] == 'f') && (y + 2 > 0)) {
                    y++;
                    map[y][x] = ' ';
                    map[y + 1][x] = 'F';
                    system("cls");
                    display_map();
                }

                else if ((map[y + 1][x] == '#') && (map[y + 2][x] == '*')) {
                    y++;
                    map[y][x] = ' ';
                    map[y + 1][x] = '*';

                    system("cls");
                    display_map();

                    score += 2;

                }

                else if ((map[y + 1][x] == 'A') && (map[y + 2][x] == '*')) { y++; map[y][x] = ' '; map[y + 1][x] = '*'; system("cls"); display_map(); score += 4; }
                else if ((map[y + 1][x] == 'B') && (map[y + 2][x] == '*')) { y++; map[y][x] = ' '; map[y + 1][x] = '*'; system("cls"); display_map(); score += 4; }
                else if ((map[y + 1][x] == 'D') && (map[y + 2][x] == '*')) { y++; map[y][x] = ' '; map[y + 1][x] = '*'; system("cls"); display_map(); score += 4; }
                else if ((map[y + 1][x] == 'F') && (map[y + 2][x] == '*')) { y++; map[y][x] = ' '; map[y + 1][x] = '*'; system("cls"); display_map(); score += 4; }
            }
        }

        if (GetAsyncKeyState(VK_LEFT)) {

            // 이동하려는 위치가 빈 공간이거나 코인이라면
            if (map[y][x - 1] == '.' || map[y][x - 1] == ' ') {
                x--; // 플레이어 이동
            }


            // 'S' 일 경우 벽을 넘을 수 있다.
            else if (player == 'S') {
                int i = 1;

                // 왼쪽으로 벽(#)이 몇 개인지 계산
                while (x - 1 - i >= 0 && map[y][x - 1 - i] == '#') {
                    i++;
                }

                // 범위를 확인하고 이동
                if (x - i - 1 >= 0 && (map[y][x - 1 - i] == '.' || map[y][x - 1 - i] == ' ') && (score >= i * 2)) {
                    gotoxy(y, x - 1 - i);
                    x -= (i + 1);
                    cout << player;
                    score -= i * 2;

                    system("cls");
                    display_map();
                }
            }

            // 'p' 일 때만 벽을 밀 수 있다.
            else if (player == 'P') {
                if (x - 1 == w_gate.x && y == w_gate.y) { y = e_gate.y; x = e_gate.x; } // Teleport W to E

                else if ((map[y][x - 1] == '#') && (map[y][x - 2] == ' ') && (x - 2 > 0)) {
                    x--; // 이동
                    map[y][x] = ' ';
                    map[y][x - 1] = '#'; // 이동 된 한칸 옆로 '#'를 이동 시킨다.

                    system("cls");
                    display_map();
                }

                // 대문자 이동 시키기
                else if ((map[y][x - 1] == 'A') && (map[y][x - 2] == ' ') && (x - 2 > 0)) {
                    x--; // 이동
                    map[y][x] = ' ';
                    map[y][x - 1] = 'A';

                    system("cls");
                    display_map();
                }
                else if ((map[y][x - 1] == 'B') && (map[y][x - 2] == ' ') && (x - 2 > 0)) {
                    x--; // 이동
                    map[y][x] = ' ';
                    map[y][x - 1] = 'B';

                    system("cls");
                    display_map();
                }
                else if ((map[y][x - 1] == 'D') && (map[y][x - 2] == ' ') && (x - 2 > 0)) {
                    x--; // 이동
                    map[y][x] = ' ';
                    map[y][x - 1] = 'D';

                    system("cls");
                    display_map();
                }
                else if ((map[y][x - 1] == 'F') && (map[y][x - 2] == ' ') && (x - 2 > 0)) {
                    x--; // 이동
                    map[y][x] = ' ';
                    map[y][x - 1] = 'F';

                    system("cls");
                    display_map();
                }

                // '#'을 밀어 넣기
                else if ((map[y][x - 1] == '#') && (map[y][x - 2] == '*')) {
                    x--;
                    map[y][x] = ' ';
                    map[y][x - 1] = '*';

                    system("cls");
                    display_map();

                    score += 2;
                }

                // 소문자 이동 시키기
                else if ((map[y][x - 1] == 'a') && (map[y][x - 2] == ' ') && (x - 2 > 0)) { // 글자 이동
                    x--; // 이동
                    map[y][x] = ' ';
                    map[y][x - 1] = 'a';

                    system("cls");
                    display_map();
                }
                else if ((map[y][x - 1] == 'b') && (map[y][x - 2] == ' ') && (x - 2 > 0)) { // 글자 이동
                    x--; // 이동
                    map[y][x] = ' ';
                    map[y][x - 1] = 'b';

                    system("cls");
                    display_map();
                }
                else if ((map[y][x - 1] == 'd') && (map[y][x - 2] == ' ') && (x - 2 > 0)) { // 글자 이동
                    x--; // 이동
                    map[y][x] = ' ';
                    map[y][x - 1] = 'd';

                    system("cls");
                    display_map();
                }
                else if ((map[y][x - 1] == 'f') && (map[y][x - 2] == ' ') && (x - 2 > 0)) { // 글자 이동
                    x--; // 이동
                    map[y][x] = ' ';
                    map[y][x - 1] = 'f';

                    system("cls");
                    display_map();
                }

                // 소문자 + 소문자 대문자
                else if ((map[y][x - 1] == 'a') && (map[y][x - 2] == 'a') && (x - 2 > 0)) {
                    x--;
                    map[y][x] = ' ';
                    map[y][x - 1] = 'A';
                    system("cls");
                    display_map();
                }
                else if ((map[y][x - 1] == 'b') && (map[y][x - 2] == 'b') && (x - 2 > 0)) {
                    x--;
                    map[y][x] = ' ';
                    map[y][x - 1] = 'B';
                    system("cls");
                    display_map();
                }
                else if ((map[y][x - 1] == 'd') && (map[y][x - 2] == 'd') && (x - 2 > 0)) {
                    x--;
                    map[y][x] = ' ';
                    map[y][x - 1] = 'D';
                    system("cls");
                    display_map();
                }
                else if ((map[y][x - 1] == 'f') && (map[y][x - 2] == 'f') && (x - 2 > 0)) {
                    x--;
                    map[y][x] = ' ';
                    map[y][x - 1] = 'F';
                    system("cls");
                    display_map();
                }
                // 알파벳 대문자를 밀어 넣기
                else if ((map[y][x - 1] == 'A') && (map[y][x - 2] == '*')) { x--; map[y][x] = ' '; map[y][x - 1] = '*'; system("cls"); display_map(); score += 4; }
                else if ((map[y][x - 1] == 'B') && (map[y][x - 2] == '*')) { x--; map[y][x] = ' '; map[y][x - 1] = '*'; system("cls"); display_map(); score += 4; }
                else if ((map[y][x - 1] == 'D') && (map[y][x - 2] == '*')) { x--; map[y][x] = ' '; map[y][x - 1] = '*'; system("cls"); display_map(); score += 4; }
                else if ((map[y][x - 1] == 'F') && (map[y][x - 2] == '*')) { x--; map[y][x] = ' '; map[y][x - 1] = '*'; system("cls"); display_map(); score += 4; }
            }

        }

        if (GetAsyncKeyState(VK_RIGHT)) {
            if (map[y][x + 1] == '.' || map[y][x + 1] == ' ') x++;
            else if (x + 1 == e_gate.x && y == e_gate.y) { y = w_gate.y; x = w_gate.x; } // Teleport E to W

            // 'p' 일 때만 벽을 밀 수 있다.
            if (player == 'P') {
                if ((map[y][x + 1] == '#') && (map[y][x + 2] == ' ') && (x + 2 < 40)) { // 배열 내부에 있는 '#' 이고, 외벽이 아닌 경우 (배열 범위 안 인 경우)
                    x++; // 이동
                    map[y][x] = ' ';
                    map[y][x + 1] = '#'; // 이동 된 한 칸 오른쪽로 '#'를 이동 시킨다.

                    system("cls");
                    display_map();
                }
                // 대문자 이동 시키기
                else if ((map[y][x + 1] == 'A') && (map[y][x + 2] == ' ') && (x + 2 > 0)) {
                    x++; // 이동
                    map[y][x] = ' ';
                    map[y][x + 1] = 'A';

                    system("cls");
                    display_map();
                }
                else if ((map[y][x + 1] == 'B') && (map[y][x + 2] == ' ') && (x + 2 > 0)) {
                    x++; // 이동
                    map[y][x] = ' ';
                    map[y][x + 1] = 'B';

                    system("cls");
                    display_map();
                }
                else if ((map[y][x + 1] == 'D') && (map[y][x + 2] == ' ') && (x + 2 > 0)) {
                    x++; // 이동
                    map[y][x] = ' ';
                    map[y][x + 1] = 'D';

                    system("cls");
                    display_map();
                }
                else if ((map[y][x + 1] == 'F') && (map[y][x + 2] == ' ') && (x + 2 > 0)) {
                    x++; // 이동
                    map[y][x] = ' ';
                    map[y][x + 1] = 'F';

                    system("cls");
                    display_map();
                }

                // 소문자 이동 시키기
                else if ((map[y][x + 1] == 'a') && (map[y][x + 2] == ' ') && (x + 2 > 0)) { // 글자 이동
                    x++; // 이동
                    map[y][x] = ' ';
                    map[y][x + 1] = 'a';

                    system("cls");
                    display_map();
                }
                else if ((map[y][x + 1] == 'b') && (map[y][x + 2] == ' ') && (x + 2 > 0)) { // 글자 이동
                    x++; // 이동
                    map[y][x] = ' ';
                    map[y][x + 1] = 'b';

                    system("cls");
                    display_map();
                }
                else if ((map[y][x + 1] == 'd') && (map[y][x + 2] == ' ') && (x + 2 > 0)) { // 글자 이동
                    x++; // 이동
                    map[y][x] = ' ';
                    map[y][x + 1] = 'd';

                    system("cls");
                    display_map();
                }
                else if ((map[y][x + 1] == 'f') && (map[y][x + 2] == ' ') && (x + 2 > 0)) { // 글자 이동
                    x++; // 이동
                    map[y][x] = ' ';
                    map[y][x + 1] = 'f';

                    system("cls");
                    display_map();
                }

                // 소문자 + 소문자 대문자
                else if ((map[y][x + 1] == 'a') && (map[y][x + 2] == 'a') && (x + 2 > 0)) {
                    x++;
                    map[y][x] = ' ';
                    map[y][x + 1] = 'A';
                    system("cls");
                    display_map();
                }
                else if ((map[y][x + 1] == 'b') && (map[y][x + 2] == 'b') && (x + 2 > 0)) {
                    x++;
                    map[y][x] = ' ';
                    map[y][x + 1] = 'B';
                    system("cls");
                    display_map();
                }
                else if ((map[y][x + 1] == 'd') && (map[y][x + 2] == 'd') && (x + 2 > 0)) {
                    x++;
                    map[y][x] = ' ';
                    map[y][x + 1] = 'D';
                    system("cls");
                    display_map();
                }
                else if ((map[y][x + 1] == 'f') && (map[y][x + 2] == 'f') && (x + 2 > 0)) {
                    x++;
                    map[y][x] = ' ';
                    map[y][x + 1] = 'F';
                    system("cls");
                    display_map();
                }

                else if ((map[y][x + 1] == '#') && (map[y][x + 2] == '*')) {
                    x++;
                    map[y][x] = ' ';
                    map[y][x + 1] = '*';

                    system("cls");
                    display_map();

                    score += 2;

                }
                // 알파벳 대문자를 밀어 넣기
                else if ((map[y][x + 1] == 'A') && (map[y][x + 2] == '*')) { x++; map[y][x] = ' '; map[y][x + 1] = '*'; system("cls"); display_map(); score += 4; }
                else if ((map[y][x + 1] == 'B') && (map[y][x + 2] == '*')) { x++; map[y][x] = ' '; map[y][x + 1] = '*'; system("cls"); display_map(); score += 4; }
                else if ((map[y][x + 1] == 'D') && (map[y][x + 2] == '*')) { x++; map[y][x] = ' '; map[y][x + 1] = '*'; system("cls"); display_map(); score += 4; }
                else if ((map[y][x + 1] == 'F') && (map[y][x + 2] == '*')) { x++; map[y][x] = ' '; map[y][x + 1] = '*'; system("cls"); display_map(); score += 4; }
            }
        }

        // 플레이어인 내가 위치를 이동한 경우, 적의 이동 경로를 다시 계산한다
        // old_x, old_y로 전에 있는 위치를 저장해서 한칸 전 단계를 따라오도록 
        if (old_x != x || old_y != y) {
            FindPath(ex, ey, x, y);

            // 플레이어의 이전 위치를 지우고, 새 위치에 플레이어를 표시
            gotoxy(old_x, old_y);

            // 이전 위치가 '*'이면 다시 '*'을 표시
            if (map[old_y][old_x] == '*') {
                cout << "*";
            }

            else {
                cout << " "; // 빈 공간 출력
            }

            // 새로운 위치로 이동
            gotoxy(x, y);

            // 현재 위치가 '*'이면 '*'을 유지하며 플레이어를 그 위에 표시
            if (map[y][x] == '*') {
                cout << "*";
            }
            else {
                cout << player;
            }

            if (map[y][x] == '.') {
                score++;
            }

        }

        if (map[ey][ex] == 'T') {
            gotoxy(ex, ey);
            cout << 'T';
        }
        else {
            map[ey][ex] = '.';
            gotoxy(ex, ey);
            cout << ".";
        }

        /* 맵상의 적의 현재 위치에 코인을 놓는다
        map[ey][ex] = '.';
        gotoxy(ex, ey);
        cout << ".";
        */

        if (count % game_speed == 0 &&      // 게임 진행 속도를 조절하기 위해서임
            enemy_path.size() != 0) {      // enemy가 플레이어에게 다가올 경로가 아직 남아 있다는 뜻임

            ex = enemy_path.back().x;      // 적이 다음번에 이동할 x 위치
            ey = enemy_path.back().y;      // 적이 다음번에 이동할 y 위치

            enemy_path.pop_back();         // X 위치
            // enemy_path에서 마지막 원소를 제거한다
            // enemy_path가  back()와 pop_back()를 사용하므로
            // 뒤에서 앞쪽으로 값을 읽어오기 때문에 
            // 적의 위치에서 플레이어의 위치 방향 순서가 된다   
        }

        // 적의 새 위치에 적을 표시한다
        gotoxy(ex, ey);
        cout << enemy;

        // 적이 플레이어 위치까지 도달했다면
        if (ex == x && ey == y) {
            break;  // while문을 벗어나서 아래에 있는 B 위치로 이동한다
        }

        // 게임이 진행되고 있는 상태에서  현재 점수를 화면에 보여준다
        gotoxy(45, 1);
        cout << "SCORE = " << score;
        Sleep(100);

        count++;
        if (count == 10000) count = 0;  // 정수 한계를 넘을까봐 0으로 초기화 함
    }

    // B 위치.  위의 break문에 의해서 while문을 벗어나 여기로 온다
    system("cls");
    cout << "GAME OVER" << endl << endl;
    cout << "Your Score = " << score << endl << endl << endl << endl;

    system("pause");
    return 0;
}