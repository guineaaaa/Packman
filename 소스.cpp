// PackMan_1.cpp : Defines the entry point for the console application.
// 
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <string>
#include <vector>

using namespace std;

const int map_row_size = 20;      // �������� 20�� �̰�, �������� 40�� �̹Ƿ�
const int map_column_size = 40;      // ������ ���� �簢�� ���¸� �̷��

char bfs_map[map_row_size][map_column_size];  // BFS ������� path ã�� �� �����


char map[map_row_size][map_column_size] = {   // ��
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


// ȭ�鿡�� ���� �����ش�
void display_map() {
    int i;

    for (i = 0; i <= map_row_size - 1; i++) {
        printf("%s\n", map[i]);
    }
}


// Ŀ���� Ư�� ��ġ�� �̵��Ѵ�
// �������� �� ��. !!!!!         �̴�� ����� ��. !!!!!
// short �ڷ��� �״�� ����Ѵ�.  int ����ϴϱ� ��� ���
void gotoxy(short x, short y) {
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD position = { x, y };

    SetConsoleCursorPosition(hStdout, position);
}


// BFS ������� ���� ã�� �� ����ϴ� ����� ����ü
struct path_node {
    int x;         // x ��ġ
    int y;         // y ��ġ
    int parent_id;   // �θ� id number
    // �̰��� ����Ͽ� ���� ã�� ��, ���� ��ġ�� �̵��ϰ� �ȴ�
};


// ��ġ�� ��Ÿ���� ����ü
struct position {
    int x;
    int y;
};


vector<path_node> bfs_list;      // ������ġ���� ������������ ���� ã�� ���ؼ�
// BFS ������� ��������� ����Ʈ
// ���� ��ġ���� �÷��̾� ��ġ �������� �����Ѵ�
// Z ��ġ�� ����, bfs_list.push_back() �Լ��� ����ϹǷ�
// �ڿ��� �ֱ� ������  ���� ��ġ���� �÷��̾��� ��ġ �������� �����Ѵ�

vector<position> enemy_path;   // ���� ������ �̵��� path
// �÷��̾� ��ġ���� �� ��ġ �������� �����Ѵ�
// Y ��ġ�� ����, enemy_path.push_back() �Լ��� ����ϹǷ�
// �ڿ��� �ֱ� ������  �÷��̾��� ��ġ���� ���� ��ġ �������� �����Ѵ�
//
// main() �ȿ� �ִ� X ��ġ�� ����
// enemy_path��  back()�� pop_back()�� ����ϹǷ�
// �ڿ��� �������� ���� �о���� ������ 
// ���� ��ġ���� �÷��̾��� ��ġ ���� ������ �ȴ� 


// �ش� ��ġ (x, y)�� ����ִ� ��쿡�� ���ο� ��带 ����Ʈ�� �߰��Ѵ�
// x�� y�� ���� ��ġ�̰�,  id�� �θ� ����� id ��ȣ�̴�
// FindPath()���� 4�� ȣ���Ѵ�
void add_new_node_to_bfs_list(int x, int y, int id) {
    path_node node;

    // ������ ��ġ (x,y)��  �� �����̰ų� �������̾  �̵��� �����ϴٸ�
    if (bfs_map[y][x] == ' ' || bfs_map[y][x] == '.') {
        bfs_map[y][x] = '#';      // ���� �� ��ġ�� �湮�� ������ ǥ���Ѵ�


        node.x = x;
        node.y = y;
        node.parent_id = id;
        bfs_list.push_back(node);   // Z ��ġ.  
        // bfs_list ������ �߰��ϹǷ�
        // ���� ��ġ���� �÷��̾� ��ġ �������� �����Ѵ�
    }
}


// �� ��ġ (ex,ey) ���� �÷��̾� ��ġ�� �� ��ġ (x,y) ������ ���� ã�´�
void FindPath(int ex, int ey, int x, int y) {
    path_node node;
    position pos;
    int j, k;
    int index;


    // ���� ã�� ���ؼ� map ������ bfs_map�� �����Ѵ�
    for (k = 0; k <= map_row_size - 1; k++) {
        for (j = 0; j <= map_column_size - 1; j++) {
            bfs_map[k][j] = map[k][j];
        }
    }

    // (A)  bfs_list�� �������� �����
    bfs_list.clear();

    // (B)  bfs_list ù��° ��ġ�� [0]����  ���� ���� ��ġ�� �ִ´�.
    // �Ʒ� 4���� ��� �� (B)�� �ش��Ѵ�
    node.x = ex;
    node.y = ey;
    node.parent_id = -1;      // ���� ���� ��ġ�� �θ�� ��� -1�� �Ѵ�
    bfs_list.push_back(node);   // bfs_list ������ �߰��Ѵ�

    // (C)  bfs_list���� [0] ��ġ�� ������ �����Ѵ�
    index = 0;

    // bfs list�� ������ ������� �ϳ��� �湮�ϸ鼭
    while (index <= bfs_list.size() - 1) {   // (D)

        // �÷��̾��� ���� ��ġ (x,y)���� �����ߴٸ�
        if (bfs_list[index].x == x && bfs_list[index].y == y) {      // (E)

            enemy_path.clear();  // (P)  ���� �̵���θ� �����ϴ� enemy_path�� ����ִ� ���·� ��� ������ �ʱ�ȭ �Ѵ�

            // ���� ���� ��ġ�� ����Ǿ� �ִ� [0] ��ġ�� �������� ���� ���ȿ�
            while (bfs_list[index].parent_id != -1) {   // (Q)

                // (R)  ����, bfs list�� ���� ����� ��ġ ������ 
                //      pos ������ ������ �Ŀ�            
                pos.x = bfs_list[index].x;
                pos.y = bfs_list[index].y;

                // (S)  pos ������  ���� �̵������ enemy_path list ������ �ִ´�
                enemy_path.push_back(pos);  // Y ��ġ.   
                // ���� �̵������ enemy_path list ������ �߰���Ų��
                // �׷���, �÷��̾��� ��ġ���� ���� ��ġ �������� �����Ѵ�

    // (T)  �θ� ���� �̵������ν�  �Ѵܰ� �����ܰ�� �̵��Ѵ� !!!!!
                index = bfs_list[index].parent_id;
            }

            // (U)  ù��° while���� �����  �Ʒ��� �ִ� A ��ġ�� ����
            goto U_Exit;
        }

        // (F)  ���� �� ��ġ (x,y)���� �������� ���� ��쿡�� 
        //      ���� ��ġ�� ��/��/��/�� ��ġ�� ���� �����ؼ� �߰���Ų��
        //      index��  bfs_list�� ���� ����� �ε����̴�
        //      �Ʒ� �� ���� ���� F1, F2, F3, F4 �̴�
        add_new_node_to_bfs_list(bfs_list[index].x, bfs_list[index].y - 1, index);
        add_new_node_to_bfs_list(bfs_list[index].x, bfs_list[index].y + 1, index);
        add_new_node_to_bfs_list(bfs_list[index].x - 1, bfs_list[index].y, index);
        add_new_node_to_bfs_list(bfs_list[index].x + 1, bfs_list[index].y, index);

        // (G)  bfs_list�� ������ ���� �̵��Ѵ�
        index++;
    }

    // A ��ġ.  ���� break ������ while�� ����� ����� �´�
U_Exit:
    bfs_list.clear();
}


// ���� ���� ����
bool enemy_frozen = false;
DWORD freeze_start_time; // ���� ȿ�� ���� �ð� (�и���)

void handleEnemyFreeze(int& game_speed, int ex, int ey) {
    if (enemy_frozen) {
        // ���� �ð��� ���� �ð��� ���̸� ��
        DWORD now = GetTickCount();
        if (now - freeze_start_time >= 5000) {
            // 5�ʰ� ������ �� ���� ����
            enemy_frozen = false;
            game_speed = 5; // ���� �ӵ��� ����
        }
    }
    else if (map[ey][ex] == '0'||map[ey][ex]=='-') {
        // ���� '0'�� ������ ���
        enemy_frozen = true;
        game_speed = 30000; // �� �ӵ� ����
        freeze_start_time = GetTickCount(); // ���� �ð� ���
        map[ey][ex] = '.'; // ���� ����
        gotoxy(ex, ey);
        cout << '.';
    }
    
}

//===========================================
//  M A I N   P R O G R A M 
//===========================================
int main()
{
    bool game_is_running_now = true;

    int x = 15; // �÷��̾��� ���� ���� ��ġ x
    int y = 16; // �÷��̾��� ���� ���� ��ġ y

    int old_x;  // �÷��̾��� ���� ���� ��ġ x
    int old_y;  // �÷��̾��� ���� ���� ��ġ y

    int ex = 2; // �� ���� ��ġ x
    int ey = 2; // �� ���� ��ġ y

    int game_speed = 5;  // �ʱⰪ���� ���� �����
    int score = 100;         // ���� = ȹ���� ���� ����
    int count = 0;         // ������ �Ź� ����Ǵ� Ƚ��

    char player = 'P';      // �÷��̾�
    char enemy = 'E';      // enemy, ��
    
    // char hostage = 'H'; //���� h ��ġ: {23,5}
    // int hx = 23;
    // int hy = 5;
    

    bool enemy_frozen = false; // ���� ����ִ��� ����
    int freeze_time = 0;       // ���� ȿ�� ���� �ð� (5�� ���� ���ߴ� �ð�)

    int enemy_speed = game_speed; // ���� �⺻ �ӵ� (5)
    int original_enemy_speed = game_speed; // ���� ���� �ӵ�


    system("cls");
    display_map();  // ȭ�鿡 ���� ���δ�

    gotoxy(x, y); cout << player;  // player�� ���δ�

    FindPath(ex, ey, x, y);         // ���� �����ϸ� enemy�� player���Է� ���� ã�Ƽ� �����Ѵ�

    /////////////////////////////////////
    //  G A M E   M A I N   P A R T 
    /////////////////////////////////////


    const position n_gate = { 2, 0 };
    const position s_gate = { 32, 19 }; // ����
    const position w_gate = { 0, 18 };
    const position e_gate = { 38, 2 };

    while (game_is_running_now) {
        old_x = x;
        old_y = y;

        // S Ű�� ������ ���
        if (GetAsyncKeyState(0x53)) {
            player = 'S';
        }

        // R Ű�� ������ ���
        if (GetAsyncKeyState(0x52)) {
            player = 'R';
        }

        // Player�� �̵���Ű�� ���� Ű
        if (GetAsyncKeyState(VK_UP)) {
            if (map[y - 1][x] == '.' || map[y - 1][x] == ' ') y--;
            else if (y - 1 == n_gate.y && x == n_gate.x) { y = s_gate.y; x = s_gate.x; } // N ����Ʈ���� S ����Ʈ�� �̵�
            

            // 'p' �� ��쿡�� ���� ������ �� �ִ�.
            else if (player == 'P') {
                if ((map[y - 1][x] == '#') && (map[y - 2][x] == ' ') && (y - 2 > 0)) { // �迭 ���ο� �ִ� '#' �̰�, �ܺ��� �ƴ� ��� (�迭 ���� �� �� ���)s
                    y--; // �̵�
                    map[y][x] = ' ';
                    map[y - 1][x] = '#'; // �̵� �� ��ĭ ���� '#'�� �̵� ��Ų��.

                    system("cls");
                    display_map();
                }
                // �빮�� �̵� ��Ű��
                else if ((map[y - 1][x] == 'A') && (map[y - 2][x] == ' ') && (y - 2 > 0)) { // ���� �̵�
                    y--; // �̵�
                    map[y][x] = ' ';
                    map[y - 1][x] = 'A';

                    system("cls");
                    display_map();
                }
                else if ((map[y - 1][x] == 'B') && (map[y - 2][x] == ' ') && (y - 2 > 0)) { // ���� �̵�
                    y--; // �̵�
                    map[y][x] = ' ';
                    map[y - 1][x] = 'B';

                    system("cls");
                    display_map();
                }
                else if ((map[y - 1][x] == 'D') && (map[y - 2][x] == ' ') && (y - 2 > 0)) { // ���� �̵�
                    y--; // �̵�
                    map[y][x] = ' ';
                    map[y - 1][x] = 'D';

                    system("cls");
                    display_map();
                }
                else if ((map[y - 1][x] == 'F') && (map[y - 2][x] == ' ') && (y - 2 > 0)) { // ���� �̵�
                    y--; // �̵�
                    map[y][x] = ' ';
                    map[y - 1][x] = 'F';

                    system("cls");
                    display_map();
                }

                // �ҹ��� �̵� ��Ű��
                else if ((map[y - 1][x] == 'a') && (map[y - 2][x] == ' ') && (y - 2 > 0)) { // ���� �̵�
                    y--; // �̵�
                    map[y][x] = ' ';
                    map[y - 1][x] = 'a';

                    system("cls");
                    display_map();
                }

                else if ((map[y - 1][x] == 'b') && (map[y - 2][x] == ' ') && (y - 2 > 0)) { // ���� �̵�
                    y--; // �̵�
                    map[y][x] = ' ';
                    map[y - 1][x] = 'b';

                    system("cls");
                    display_map();
                }
                else if ((map[y - 1][x] == 'd') && (map[y - 2][x] == ' ') && (y - 2 > 0)) { // ���� �̵�
                    y--; // �̵�
                    map[y][x] = ' ';
                    map[y - 1][x] = 'd';

                    system("cls");
                    display_map();
                }
                else if ((map[y - 1][x] == 'f') && (map[y - 2][x] == ' ') && (y - 2 > 0)) { // ���� �̵�
                    y--; // �̵�
                    map[y][x] = ' ';
                    map[y - 1][x] = 'f';

                    system("cls");
                    display_map();
                }
                // �ҹ��� + �ҹ��� �빮��
                else if ((map[y - 1][x] == 'a') && (map[y - 2][x] == 'a') && (y - 2 > 0)) {
                    y--;
                    map[y][x] = ' ';
                    map[y - 1][x] = 'A';
                    system("cls");
                    display_map();
                }
                else if ((map[y - 1][x] == 'b') && (map[y - 2][x] == 'b') && (y - 2 > 0)) {
                    y--; // �̵�
                    map[y][x] = ' ';
                    map[y - 1][x] = 'B';
                    system("cls");
                    display_map();
                }
                else if ((map[y - 1][x] == 'd') && (map[y - 2][x] == 'd') && (y - 2 > 0)) {
                    y--; // �̵�
                    map[y][x] = ' ';
                    map[y - 1][x] = 'D';
                    system("cls");
                    display_map();
                }
                else if ((map[y - 1][x] == 'f') && (map[y - 2][x] == 'f') && (y - 2 > 0)) {
                    y--; // �̵�
                    map[y][x] = ' ';
                    map[y - 1][x] = 'F';
                    system("cls");
                    display_map();
                }

                // '#'�� �ֱ�
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

        if (GetAsyncKeyState(VK_DOWN) ) {
            if (map[y + 1][x] == '.' || map[y + 1][x] == ' ') y++;
            else if (y + 1 == s_gate.y && x == s_gate.x) { y = n_gate.y; x = n_gate.x; } // S ����Ʈ���� N ����Ʈ�� �̵�

            // 'P'�� ��쿡�� ���� ������ �� �ִ�.
            if (player == 'P') {
                if ((map[y + 1][x] == '#') && (map[y + 2][x] == ' ') && (y + 2 < 20)) { // �迭 ���ο� �ִ� '#' �̰�, �ܺ��� �ƴ� ��� (�迭 ���� �� �� ���)
                    y++; // �̵�
                    map[y][x] = ' ';
                    map[y + 1][x] = '#'; // �̵� �� ��ĭ �ط� '#'�� �̵� ��Ų��.

                    system("cls");
                    display_map();
                }
                // �빮�� �̵� ��Ű��
                else if ((map[y + 1][x] == 'A') && (map[y + 2][x] == ' ') && (y + 2 > 0)) { // ���� �̵�
                    y++;// �̵�
                    map[y][x] = ' ';
                    map[y + 1][x] = 'A';

                    system("cls");
                    display_map();
                }
                else if ((map[y + 1][x] == 'B') && (map[y + 2][x] == ' ') && (y + 2 > 0)) { // ���� �̵�
                    y++; // �̵�
                    map[y][x] = ' ';
                    map[y + 1][x] = 'B';

                    system("cls");
                    display_map();
                }
                else if ((map[y + 1][x] == 'D') && (map[y + 2][x] == ' ') && (y + 2 > 0)) { // ���� �̵�
                    y++; // �̵�
                    map[y][x] = ' ';
                    map[y + 1][x] = 'D';

                    system("cls");
                    display_map();
                }
                else if ((map[y + 1][x] == 'F') && (map[y + 2][x] == ' ') && (y + 2 > 0)) { // ���� �̵�
                    y++; // �̵�
                    map[y][x] = ' ';
                    map[y + 1][x] = 'F';

                    system("cls");
                    display_map();
                }

                // �ҹ��� �̵� ��Ű��
                else if ((map[y + 1][x] == 'a') && (map[y + 2][x] == ' ') && (y + 2 > 0)) { // ���� �̵�
                    y++;
                    map[y][x] = ' ';
                    map[y + 1][x] = 'a';

                    system("cls");
                    display_map();
                }
                else if ((map[y + 1][x] == 'b') && (map[y + 2][x] == ' ') && (y + 2 > 0)) { // ���� �̵�
                    y++;
                    map[y][x] = ' ';
                    map[y + 1][x] = 'b';

                    system("cls");
                    display_map();
                }
                else if ((map[y + 1][x] == 'd') && (map[y + 2][x] == ' ') && (y + 2 > 0)) { // ���� �̵�
                    y++;
                    map[y][x] = ' ';
                    map[y + 1][x] = 'd';

                    system("cls");
                    display_map();
                }
                else if ((map[y + 1][x] == 'f') && (map[y + 2][x] == ' ') && (y + 2 > 0)) { // ���� �̵�
                    y++;
                    map[y][x] = ' ';
                    map[y + 1][x] = 'f';

                    system("cls");
                    display_map();
                }
                // �ҹ��� + �ҹ��� �빮��
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

        // 'h' ���ʿ��� ������ ��ư�� ������ ���
        // 'h'�� 'H'�� �ٲ�� player�� ���� �;� �Ѵ�.
        // 'h' ���ʿ��� ������ ��ư�� ������ ���
       
        if (GetAsyncKeyState(VK_RBUTTON)) {
            if (map[y][x - 1] == 'h') {
                map[y][x - 1] = 'H'; // 'h'�� 'H'�� ����
                system("cls");
                display_map();
            }
        }

        if (GetAsyncKeyState(VK_LEFT)) {
            // ���� ��ġ ����
            old_x = x;
            old_y = y;

            // �̵��Ϸ��� ��ġ�� �� �����̰ų� �����̶��
            if (map[y][x - 1] == '.' || map[y][x - 1] == ' ') {
                x--; // �÷��̾� �̵�
            }
            // �̵��Ϸ��� ��ġ�� 'H'���
            else if (map[y][x - 1] == 'H') {
                x--;
                gotoxy(old_x, old_y);
                cout << 'H';

                // ���� ��ġ�� 'H'�� ��ġ
                map[old_y][old_x] = ' ';

                // �ʰ� ȭ�� ����
                system("cls");
                display_map();
            }

       

            // 'S' �� ��� ���� ���� �� �ִ�.
            else if (player == 'S') {
                    int i = 1;

                    // �������� ��(#)�� �� ������ ���
                    while (x - 1 - i >= 0 && map[y][x - 1 - i] == '#') {
                        i++;
                    }

                    // ������ Ȯ���ϰ� �̵�
                    if (x - i - 1 >= 0 && (map[y][x - 1 - i] == '.' || map[y][x - 1 - i] == ' ')&&(score>=i*2)) {
                        gotoxy(y, x - 1 - i); 
                        x -= (i + 1); 
                        cout << player;
                        score -= i*2;

                        system("cls");
                        display_map();
                    }
                }

            // 'p' �� ���� ���� �� �� �ִ�.
            else if (player == 'P') {
            if (x - 1 == w_gate.x && y == w_gate.y) { y = e_gate.y; x = e_gate.x; } // Teleport W to E

            else if ((map[y][x - 1] == '#') && (map[y][x - 2] == ' ') && (x - 2 > 0)) {
                x--; // �̵�
                map[y][x] = ' ';
                map[y][x - 1] = '#'; // �̵� �� ��ĭ ���� '#'�� �̵� ��Ų��.

                system("cls");
                display_map();
            }

            // �빮�� �̵� ��Ű��
            else if ((map[y][x - 1] == 'A') && (map[y][x - 2] == ' ') && (x - 2 > 0)) {
                x--; // �̵�
                map[y][x] = ' ';
                map[y][x - 1] = 'A';

                system("cls");
                display_map();
            }
            else if ((map[y][x - 1] == 'B') && (map[y][x - 2] == ' ') && (x - 2 > 0)) {
                x--; // �̵�
                map[y][x] = ' ';
                map[y][x - 1] = 'B';

                system("cls");
                display_map();
            }
            else if ((map[y][x - 1] == 'D') && (map[y][x - 2] == ' ') && (x - 2 > 0)) {
                x--; // �̵�
                map[y][x] = ' ';
                map[y][x - 1] = 'D';

                system("cls");
                display_map();
            }
            else if ((map[y][x - 1] == 'F') && (map[y][x - 2] == ' ') && (x - 2 > 0)) {
                x--; // �̵�
                map[y][x] = ' ';
                map[y][x - 1] = 'F';

                system("cls");
                display_map();
            }

            // '#'�� �о� �ֱ�
            else if ((map[y][x - 1] == '#') && (map[y][x - 2] == '*')) {
                x--;
                map[y][x] = ' ';
                map[y][x - 1] = '*';

                system("cls");
                display_map();

                score += 2;
            }

            // �ҹ��� �̵� ��Ű��
            else if ((map[y][x - 1] == 'a') && (map[y][x - 2] == ' ') && (x - 2 > 0)) { // ���� �̵�
                x--; // �̵�
                map[y][x] = ' ';
                map[y][x - 1] = 'a';

                system("cls");
                display_map();
            }
            else if ((map[y][x - 1] == 'b') && (map[y][x - 2] == ' ') && (x - 2 > 0)) { // ���� �̵�
                x--; // �̵�
                map[y][x] = ' ';
                map[y][x - 1] = 'b';

                system("cls");
                display_map();
            }
            else if ((map[y][x - 1] == 'd') && (map[y][x - 2] == ' ') && (x - 2 > 0)) { // ���� �̵�
                x--; // �̵�
                map[y][x] = ' ';
                map[y][x - 1] = 'd';

                system("cls");
                display_map();
            }
            else if ((map[y][x - 1] == 'f') && (map[y][x - 2] == ' ') && (x - 2 > 0)) { // ���� �̵�
                x--; // �̵�
                map[y][x] = ' ';
                map[y][x - 1] = 'f';

                system("cls");
                display_map();
            }

            // �ҹ��� + �ҹ��� �빮��
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
            // ���ĺ� �빮�ڸ� �о� �ֱ�
            else if ((map[y][x - 1] == 'A') && (map[y][x - 2] == '*')) { x--; map[y][x] = ' '; map[y][x - 1] = '*'; system("cls"); display_map(); score += 4; }
            else if ((map[y][x - 1] == 'B') && (map[y][x - 2] == '*')) { x--; map[y][x] = ' '; map[y][x - 1] = '*'; system("cls"); display_map(); score += 4; }
            else if ((map[y][x - 1] == 'D') && (map[y][x - 2] == '*')) { x--; map[y][x] = ' '; map[y][x - 1] = '*'; system("cls"); display_map(); score += 4; }
            else if ((map[y][x - 1] == 'F') && (map[y][x - 2] == '*')) { x--; map[y][x] = ' '; map[y][x - 1] = '*'; system("cls"); display_map(); score += 4; }
            }
            
        }

        if (GetAsyncKeyState(VK_RIGHT) ) {
            if (map[y][x + 1] == '.' || map[y][x + 1] == ' ') x++;
            else if (x + 1 == e_gate.x && y == e_gate.y) { y = w_gate.y; x = w_gate.x; } // Teleport E to W

            // 'p' �� ���� ���� �� �� �ִ�.
            if (player == 'P') {
                if ((map[y][x + 1] == '#') && (map[y][x + 2] == ' ') && (x + 2 < 40)) { // �迭 ���ο� �ִ� '#' �̰�, �ܺ��� �ƴ� ��� (�迭 ���� �� �� ���)
                    x++; // �̵�
                    map[y][x] = ' ';
                    map[y][x + 1] = '#'; // �̵� �� �� ĭ �����ʷ� '#'�� �̵� ��Ų��.

                    system("cls");
                    display_map();
                }
                // �빮�� �̵� ��Ű��
                else if ((map[y][x + 1] == 'A') && (map[y][x + 2] == ' ') && (x + 2 > 0)) {
                    x++; // �̵�
                    map[y][x] = ' ';
                    map[y][x + 1] = 'A';

                    system("cls");
                    display_map();
                }
                else if ((map[y][x + 1] == 'B') && (map[y][x + 2] == ' ') && (x + 2 > 0)) {
                    x++; // �̵�
                    map[y][x] = ' ';
                    map[y][x + 1] = 'B';

                    system("cls");
                    display_map();
                }
                else if ((map[y][x + 1] == 'D') && (map[y][x + 2] == ' ') && (x + 2 > 0)) {
                    x++; // �̵�
                    map[y][x] = ' ';
                    map[y][x + 1] = 'D';

                    system("cls");
                    display_map();
                }
                else if ((map[y][x + 1] == 'F') && (map[y][x + 2] == ' ') && (x + 2 > 0)) {
                    x++; // �̵�
                    map[y][x] = ' ';
                    map[y][x + 1] = 'F';

                    system("cls");
                    display_map();
                }

                // �ҹ��� �̵� ��Ű��
                else if ((map[y][x + 1] == 'a') && (map[y][x + 2] == ' ') && (x + 2 > 0)) { // ���� �̵�
                    x++; // �̵�
                    map[y][x] = ' ';
                    map[y][x + 1] = 'a';

                    system("cls");
                    display_map();
                }
                else if ((map[y][x + 1] == 'b') && (map[y][x + 2] == ' ') && (x + 2 > 0)) { // ���� �̵�
                    x++; // �̵�
                    map[y][x] = ' ';
                    map[y][x + 1] = 'b';

                    system("cls");
                    display_map();
                }
                else if ((map[y][x + 1] == 'd') && (map[y][x + 2] == ' ') && (x + 2 > 0)) { // ���� �̵�
                    x++; // �̵�
                    map[y][x] = ' ';
                    map[y][x + 1] = 'd';

                    system("cls");
                    display_map();
                }
                else if ((map[y][x + 1] == 'f') && (map[y][x + 2] == ' ') && (x + 2 > 0)) { // ���� �̵�
                    x++; // �̵�
                    map[y][x] = ' ';
                    map[y][x + 1] = 'f';

                    system("cls");
                    display_map();
                }

                // �ҹ��� + �ҹ��� �빮��
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
                // ���ĺ� �빮�ڸ� �о� �ֱ�
                else if ((map[y][x + 1] == 'A') && (map[y][x + 2] == '*')) { x++; map[y][x] = ' '; map[y][x + 1] = '*'; system("cls"); display_map(); score += 4; }
                else if ((map[y][x + 1] == 'B') && (map[y][x + 2] == '*')) { x++; map[y][x] = ' '; map[y][x + 1] = '*'; system("cls"); display_map(); score += 4; }
                else if ((map[y][x + 1] == 'D') && (map[y][x + 2] == '*')) { x++; map[y][x] = ' '; map[y][x + 1] = '*'; system("cls"); display_map(); score += 4; }
                else if ((map[y][x + 1] == 'F') && (map[y][x + 2] == '*')) { x++; map[y][x] = ' '; map[y][x + 1] = '*'; system("cls"); display_map(); score += 4; }
            }
        }


        // freeze
        if (GetAsyncKeyState(VK_SPACE) && score >= 5) {
            if (map[y][x] != '0') {
               
                if (map[y - 1][x] == ' ') {
                    map[y - 1][x] = '0';  
                    
                    gotoxy(x, y - 1);
                    cout << '0';
                    system("cls"); display_map();
                }
                
                score -= 5; 
            }
        }

        if ((map[ey][ex] == '0') || (map[ey][ex] == '-')) {
            game_speed = 30000;
            map[ey][ex] = '.'; 
            gotoxy(ex, ey);
            cout << '.';
        }

        // �������� �� ���
        if (GetAsyncKeyState(0x41)) {
            if (score >= 5) { // ���� Ȯ��
                int i = 1;

                // y ��ǥ �˻�
                for(int i=0;i<20;i++)

                // x ��ǥ �˻�
                for (int i = 0; i < 40; i++) {
                    if (map[y][i] == 'E') {
                        map[ey][ex] = '-';

                    }
                }

                
            }
            else {
                // ������ ������ �� �޽��� ǥ��
                gotoxy(0, 0);
                cout << "Not enough score to shoot!";
                Sleep(1000);
            }
        }


        // �÷��̾��� ���� ��ġ�� �̵��� ���, ���� �̵� ��θ� �ٽ� ����Ѵ�
        // old_x, old_y�� ���� �ִ� ��ġ�� �����ؼ� ��ĭ �� �ܰ踦 ��������� 
        if (old_x != x || old_y != y) {
            FindPath(ex, ey, x, y);

            // �÷��̾��� ���� ��ġ�� �����, �� ��ġ�� �÷��̾ ǥ��
            gotoxy(old_x, old_y);

            // ���� ��ġ�� '*'�̸� �ٽ� '*'�� ǥ��
            if (map[old_y][old_x] == '*') {
                cout << "*";
            }
           
            else {
                cout << " "; // �� ���� ���
            }

            // ���ο� ��ġ�� �̵�
            gotoxy(x, y);

            // ���� ��ġ�� '*'�̸� '*'�� �����ϸ� �÷��̾ �� ���� ǥ��
            if (map[y][x] == '*') {
                cout << "*";
            }
            else {
                cout << player;
            }

            if (map[y][x] == '.') {
                score++;
            }

            if (map[old_x][old_y] == 'H') {
                cout << ' ';

                system("cls");
                display_map();
            }

        }

        gotoxy(old_x, old_y);
        cout << 'H';

        // �ʻ��� ���� ���� ��ġ�� ������ ���´�
        map[ey][ex] = '.';
        gotoxy(ex, ey);
        cout << ".";

        
        if (count % game_speed == 0 &&      // ���� ���� �ӵ��� �����ϱ� ���ؼ���
            enemy_path.size() != 0) {      // enemy�� �÷��̾�� �ٰ��� ��ΰ� ���� ���� �ִٴ� ����

            ex = enemy_path.back().x;      // ���� �������� �̵��� x ��ġ
            ey = enemy_path.back().y;      // ���� �������� �̵��� y ��ġ

            enemy_path.pop_back();         // X ��ġ
            // enemy_path���� ������ ���Ҹ� �����Ѵ�
            // enemy_path��  back()�� pop_back()�� ����ϹǷ�
            // �ڿ��� �������� ���� �о���� ������ 
            // ���� ��ġ���� �÷��̾��� ��ġ ���� ������ �ȴ�   
        }


        // handleEnemyFreeze(game_speed, ex, ey);

        // 'h' �����ʿ��� ����ڰ� RButton�� ������ 'h'�� 'H'�� ����ȴ�.
        //'H'�� player�� ���� ��ġ�� ǥ�õǾ� ������°�ó�� ���̵��� �����Ѵ� 


        // ���� �� ��ġ�� ���� ǥ���Ѵ�
        gotoxy(ex, ey);
        cout << enemy;

        // ���� �÷��̾� ��ġ���� �����ߴٸ�
        if (ex == x && ey == y) {
            break;  // while���� ����� �Ʒ��� �ִ� B ��ġ�� �̵��Ѵ�
        }

        // ������ ����ǰ� �ִ� ���¿���  ���� ������ ȭ�鿡 �����ش�
        gotoxy(45, 1);
        cout << "SCORE = " << score;
        Sleep(100);

        count++;
        if (count == 10000) count = 0;  // ���� �Ѱ踦 ������� 0���� �ʱ�ȭ ��
    }

    // B ��ġ.  ���� break���� ���ؼ� while���� ��� ����� �´�
    system("cls");
    cout << "GAME OVER" << endl << endl;
    cout << "Your Score = " << score << endl << endl << endl << endl;

    system("pause");
    return 0;
}
