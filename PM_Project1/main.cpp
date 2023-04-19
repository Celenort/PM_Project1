#include <iostream>
#include <string>

//enum Stone { // Enum을 사용할 수 있는지 몰라서 int로 취급하고 0인 경우 빈공간, O는 1, X는 -1취급했습니다.
//    O=1,
//    X=-1
//};

using namespace std;
const int BOARD_SIZE = 15;

void visualize(int [][BOARD_SIZE]);
void wrongInput();
void outOfRange();
void placeOccupied();
void doubleThree();
// 승자가 결정되었을 때 호출되며, 마지막 수도 보여줘야하므로 visualize 호출한 뒤
// isGameOver=true를 반환, winner에 따라 문구 출력
void win(int [][BOARD_SIZE], bool& isGameOver, int winner);

bool isFive(int [][BOARD_SIZE], int r, int c);
// int direction= 0 ~ 7. 0 : North, 1 : NW, 2 : W ... (North기준 45도씩 반시계방향으로 회전.)
// assignDirection(r,c, (dir+4)%8) : revert changes (opposite direction)
void assignDirection(int& r, int& c, int direction);

// (r,c)에서 입력받은 direction에 대해 탐색을 수행, 해당 방향에서 3-3에 관여하는 값(depth)를 반환
// by default space = 0, cnt=0이며, space==2에서 탐색종료. 
int search(int [][BOARD_SIZE], int r, int c, int direction, int& space, int& cnt);

bool isDoubleThree(int [][BOARD_SIZE], int r, int c);
void place(int(&board)[][BOARD_SIZE], int r, int c, int stone);
bool isOccupied(int [][BOARD_SIZE], int r, int c);
// main()에서 do{} while();에서 while문 내부에 들어가 사용되며, 
// OutofBorder, WrongInput, Occupied, doubleThree의 4가지 실패조건 처리후 모두 만족시 place.
bool interpret_and_Place(int [][BOARD_SIZE], string userInput, int whosTurn, bool& isGameOver);

int main()
{

    string input;
    bool isGameOver = false;
    int turn = -1; // Stone turn = X;
    int board[BOARD_SIZE][BOARD_SIZE] = { 0 };

    cout << "Game Start" << endl;

    while (!isGameOver) {
        visualize(board);
        if (turn == -1) { // ~~ == X
            do {
                cout << "player " << "X" << " : ";
                cin >> input;
            } while (!interpret_and_Place(board, input, -1, isGameOver));
            turn = 1; // turn = O;
        }
        else {
            do {
                cout << "player " << "O" << " : ";
                cin >> input;
            } while (!interpret_and_Place(board, input, 1, isGameOver));
            turn = -1;
        }
    }
    return 0;
}


void visualize(int boardinput[][BOARD_SIZE]) {
    cout << "\t";
    for (int i = 0; i < BOARD_SIZE; i++) {
        char col_indexing = 'a';
        cout << (char)(col_indexing + i) << ' ';
    }
    cout << endl;
    for (int i = 0; i < BOARD_SIZE; i++) {
        cout << i + 1 << '\t';

        for (int j = 0; j < BOARD_SIZE; j++) {
            switch (boardinput[i][j]) {
            case -1: // case X: 
                cout << "X ";
                break;
            case 1: // case O:
                cout << "O ";
                break;
            default:
                cout << ". ";
                break;
            }
        }
        cout << endl;
    }
}

void wrongInput() {
    cout << "choose another position ( wrong input )" << endl;
    return;
}

void outOfRange() {
    cout << "choose another position ( out of border )" << endl;
    return;
}

void placeOccupied() {
    cout << "choose another position ( piece exist in that position )" << endl;
    return;
}

void doubleThree() {
    cout << "choose another position ( double three )" << endl;
    return;
}

void win(int board[][BOARD_SIZE], bool& isGameOver, int winner) { //Stone winner
    visualize(board);
    if (winner == -1) { // winner == X
        cout << "X wins!\n";
    }
    else if (winner == 1) { //winner == O
        cout << "O wins!\n";
    }
    isGameOver = true;
    // win()이 main() >> while >> interpret_and_place() 내에서 호출되므로, main()의 local var로 전달하기 위해
    // & 형식의 참조 이용.
    return;
}

bool isFive(int boardinput[][BOARD_SIZE], int row, int col) {
    // check near boardinput[row][column] whether 5-in-a-row (win condition) is satisfied.
    int curr_stone = boardinput[row][col];


    // code for checking row 5
    int row_low = (row - 4) < 0 ? 0 : (row - 4);
    int row_high = (row + 4) >= BOARD_SIZE ? BOARD_SIZE - 1 : row + 4;
    int stack = 0;
    for (int idx = row_low; idx <= row_high; idx++) {
        if (boardinput[idx][col] == curr_stone) {
            stack++;
        }
        else {
            stack = 0;
        }
        if (stack >= 5) {
            return true;
        }
    }


    // code for checking col 5
    stack = 0;
    int col_low = (col - 4) < 0 ? 0 : col - 4;
    int col_high = (col + 4) >= BOARD_SIZE ? BOARD_SIZE - 1 : col + 4;
    for (int idx = col_low; idx <= col_high; idx++) {
        if (boardinput[row][idx] == curr_stone) {
            stack++;
        }
        else {
            stack = 0;
        }
        if (stack >= 5) {
            return true;
        }
    }


    //code for checking left diagonal
    stack = 0;
    int upper_idx = -4;
    int lower_idx = 4;
    for (int i = 0; i > -5; i--) {
        if (col + i < 0 || row + i < 0) {
            // stop at i. 
            upper_idx = i + 1;
            break;
        }
    }
    for (int i = 0; i < 5; i++) {
        if (col + i >= BOARD_SIZE || row + i >= BOARD_SIZE) {
            // stop at i. 
            lower_idx = i - 1;
            break;
        }
    }
    // setted upper_idx, lower idx
    for (int idx = upper_idx; idx <= lower_idx; idx++) {
        if (boardinput[row + idx][col + idx] == curr_stone) {
            stack++;
        }
        else {
            stack = 0;
        }
        if (stack >= 5) {
            return true;
        }
    }


    // code for checking right diagonal
    stack = 0;
    upper_idx = 4;
    lower_idx = -4;
    for (int i = 0; i > -5; i--) {
        if (col - i < 0 || row + i >= BOARD_SIZE) {
            // stop at i. 
            lower_idx = i + 1;
            break;
        }
    }
    for (int i = 0; i < 5; i++) {
        if (col + i >= BOARD_SIZE || row - i < 0) {
            // stop at i. 
            upper_idx = i - 1;
            break;
        }
    }
    // setted upper_idx, lower idx
    for (int idx = lower_idx; idx <= upper_idx; idx++) {
        if (boardinput[row - idx][col + idx] == curr_stone) {
            stack++;
        }
        else {
            stack = 0;
        }
        if (stack >= 5) {
            return true;
        }
    }
    // divide : vertical / horizontal / left diagonal / right diagonal

    return false;
}

void assignDirection(int& row, int& col, int direction) {
    switch (direction) { // direction에 따라 row/col을 변화시킴
    case 0: //this means upward. +1 : 45' ccw
        row--;
        break;
    case 1:
        row--;
        col--;
        break;
    case 2:
        col--;
        break;
    case 3:
        col--;
        row++;
        break;
    case 4:
        row++;
        break;
    case 5:
        row++;
        col++;
        break;
    case 6:
        col++;
        break;
    case 7:
        col++;
        row--;
        break;
    default:
        return; //error
    }
}

int search(int boardinput[][BOARD_SIZE], int row, int col, int direction, int& space, int& cnt) {  // 초기에 space = 0을 넣어줌.
    int searchdepth = 0;
    bool doublespace = false;
    while (space <= 1) {

        assignDirection(row, col, direction);
        searchdepth++;
        //checks if the current row or col is out of board
        if (row >= BOARD_SIZE || row < 0 || col >= BOARD_SIZE || col < 0) {
            //revert changes
            if (doublespace) {
                space--;
            }
            assignDirection(row, col, (direction + 4) % 8);
            searchdepth--;
            break;
        }
        else if (boardinput[row][col] == 1) { // ~~ == O
            if (doublespace) {
                space--;
            }
            assignDirection(row, col, (direction + 4) % 8);
            searchdepth--;
            break;
        }
        else if (boardinput[row][col] == -1) {  // ~~ == X
            cnt++;
            doublespace = false;
        }
        else if (boardinput[row][col] == 0) { // space
            if (!doublespace) {
                space++;
                doublespace = true;
                if (space == 2) { // if two spaces encountered : exit.
                    space--;
                    searchdepth--;
                    assignDirection(row, col, (direction + 4) % 8);
                    break;
                }
            }
            else { // double space : exit
                space--;
                searchdepth--;
                assignDirection(row, col, (direction + 4) % 8);
                break;
            }
        }
    }
    return searchdepth;
}

bool isDoubleThree(int boardinput[][BOARD_SIZE], int row, int col) {
    int doublethreecount = 0;
    for (int i = 0; i < 4; i++) { // checking four directions
        int cnt = 0;
        int space = 0;
        int left = search(boardinput, row, col, i, space, cnt);
        int right = search(boardinput, row, col, (i + 4) % 8, space, cnt);
        bool blocked = false;
        if (cnt != 2) { // 33이 아니므로
            continue;
        }
        //check left boundary
        int r = row;
        int c = col;
        while (left >= 0) { // >=0이므로 최종 서치한 그 다음 자리에 대해 조사.
            assignDirection(r, c, i);
            left--;
        }
        if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE) {
            assignDirection(r, c, (i + 4) % 8);
            if (boardinput[r][c] != 0) {
                continue;
            }
            blocked = true;

            //boundary에 걸린 경우 open 3이 아님
        }
        else if (boardinput[r][c] == 1) { // ~~ == O
            assignDirection(r, c, (i + 4) % 8);
            if (boardinput[r][c] != 0) {
                continue;
            }
            blocked = true;
        }
        // now check right boundary
        r = row;
        c = col;
        while (right >= 0) { // >=0이므로 최종 서치한 그 다음 자리에 대해 조사.
            assignDirection(r, c, (i + 4) % 8);
            right--;
        }
        if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE) {
            assignDirection(r, c, (i + 4) % 8);
            if (boardinput[r][c] != 0) {
                continue;
            }
            if (blocked) {
                continue;
            }
        }
        else if (boardinput[r][c] == 1) { // ~~ == O
            assignDirection(r, c, (i + 4) % 8);
            if (boardinput[r][c] != 0) {
                continue;
            }
            if (blocked) {
                continue;
            }
        }
        doublethreecount++;
    }
    if (doublethreecount > 1) {
        return true;
    }
    else {
        return false;
    }
}

void place(int(&boardinput)[][BOARD_SIZE], int row, int column, int stone) { // Stone stone
    //Assume that every constriants are satisfied before entering this function.
    boardinput[row][column] = stone;
    return;
}

bool isOccupied(int boardinput[][BOARD_SIZE], int row, int column) {
    //if board[row][column] is occupied, return true. False if not.
    return (boardinput[row][column] == 0) ? false : true;
}

bool interpret_and_Place(int boardinput[][BOARD_SIZE], string userInput, int whosTurn, bool& isGameOver) { // Stone whosTurn
    //returns 1 if successfully interpreted and placed stone.
    //returns 0 if not. (wrong position, already occupied, or else)
    //interpret user input.
    if (userInput.size() > 3 || userInput.size() < 2) {
        wrongInput();
        return false;
    }
    int column = userInput[0] - 'a';
    int row;
    try {
        row = stoi(userInput.substr(1)) - 1;
    }
    catch (const exception& e) {
        wrongInput();
        return false;
    }
    if (column < 0 || column >26) { // char -> int로 변환하여 'a' ~ 'z' 벗어남.
        wrongInput();
        return false;
    }
    else if (column >= BOARD_SIZE || row >= BOARD_SIZE || row < 0) {
        outOfRange();
        return false;
    }
    else if (isOccupied(boardinput, row, column)) {
        placeOccupied();
        return false;
    }
    else if (whosTurn == -1 && isDoubleThree(boardinput, row, column)) {
        doubleThree();
        return false;
    }
    boardinput[row % BOARD_SIZE][column % BOARD_SIZE] = whosTurn;
    if (isFive(boardinput, row, column)) {
        win(boardinput, isGameOver, whosTurn);
        return true;
    }
    return true;
    //if the flow reached here, it means you can place the stone. 
    //but beware : check 5 condition.
}
