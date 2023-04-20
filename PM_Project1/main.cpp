#include <iostream>
#include <string>

//enum Stone { // Enum�� ����� �� �ִ��� ���� int�� ����ϰ� 0�� ��� �����, O�� 1, X�� -1����߽��ϴ�.
//    O=1,
//    X=-1
//};

using namespace std;
const int BOARD_SIZE = 15;
const bool VISUAL_ENABLED = false;

void visualize(int [][BOARD_SIZE]);
void wrongInput();
void outOfRange();
void placeOccupied();
void doubleThree();
// ���ڰ� �����Ǿ��� �� ȣ��Ǹ�, ������ ���� ��������ϹǷ� visualize ȣ���� ��
// isGameOver=true�� ��ȯ, winner�� ���� ���� ���
void win(int [][BOARD_SIZE], bool& isGameOver, int winner);

bool isFive(int [][BOARD_SIZE], int r, int c);
// int direction= 0 ~ 7. 0 : North, 1 : NW, 2 : W ... (North���� 45���� �ݽð�������� ȸ��.)
// assignDirection(r,c, (dir+4)%8) : revert changes (opposite direction)
void assignDirection(int& r, int& c, int direction);

// (r,c)���� �Է¹��� direction�� ���� Ž���� ����, �ش� ���⿡�� 3-3�� �����ϴ� ��(depth)�� ��ȯ
// by default space = 0, cnt=0�̸�, space==2���� Ž������. 
int search(int [][BOARD_SIZE], int r, int c, int direction, int& space, int mystone);

bool isDoubleThree(int [][BOARD_SIZE], int r, int c);
void place(int board[][BOARD_SIZE], int r, int c, int stone);
bool isOccupied(int [][BOARD_SIZE], int r, int c);
// main()���� do{} while();���� while�� ���ο� �� ���Ǹ�, 
// OutofBorder, WrongInput, Occupied, doubleThree�� 4���� �������� ó���� ��� ������ place.
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
                if (VISUAL_ENABLED) {
                    cout << "player " << "X" << " : ";
                }
                cin >> input;
            } while (!interpret_and_Place(board, input, -1, isGameOver));
            turn = 1; // turn = O;
        }
        else {
            do {
                if (VISUAL_ENABLED) {
                    cout << "player " << "O" << " : ";
                }
                cin >> input;
            } while (!interpret_and_Place(board, input, 1, isGameOver));
            turn = -1;
        }
    }
    return 0;
}


void visualize(int boardinput[][BOARD_SIZE]) {
    if (!VISUAL_ENABLED) {
        return;
    }
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
    // win()�� main() >> while >> interpret_and_place() ������ ȣ��ǹǷ�, main()�� local var�� �����ϱ� ����
    // & ������ ���� �̿�.
    return;
}

bool isFive(int boardinput[][BOARD_SIZE], int row, int col) {
    // check near boardinput[row][column] whether 5-in-a-row (win condition) is satisfied.
    int curr_stone = boardinput[row][col];
    for (int i = 0; i < 4; i++) { // checking four directions
        int cnt = 0;
        int space = 1;
        cnt+=search(boardinput, row, col, i, space, curr_stone);
        space = 1;
        cnt+=search(boardinput, row, col, (i + 4) % 8, space, curr_stone);
        if (cnt >=4) {
            return true;
        }
    }
    return false;
}

void assignDirection(int& row, int& col, int direction) {
    switch (direction) { // direction�� ���� row/col�� ��ȭ��Ŵ
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

int search(int boardinput[][BOARD_SIZE], int row, int col, int direction, int& space, int mystone) {  // �ʱ⿡ space = 0�� �־���.
    bool doublespace = false;
    int cnt = 0;
    while (space <= 1) {

        assignDirection(row, col, direction);
        //checks if the current row or col is out of board
        if (row >= BOARD_SIZE || row < 0 || col >= BOARD_SIZE || col < 0) {
            if (doublespace) {
                space--;
            }
            break;
        }
        else if (boardinput[row][col] == -mystone) { // ~~ == O
            if (doublespace) {
                space--;
            }
            break;
        }
        else if (boardinput[row][col] == mystone) {  // ~~ == X
            cnt++;
            doublespace = false;
        }
        else { // space (boardinput[row][col] == 0) 
            if (!doublespace) {
                doublespace = true;
            }
            else { // double space : exit
                space--;
                break;
            }
            if (space == 0) {
                space++;
            }
            else {
                break;
            }
        }
    }
    return cnt;
}

bool isDoubleThree(int boardinput[][BOARD_SIZE], int row, int col) {
    int doublethreecount = 0;
    for (int i = 0; i < 4; i++) { // checking four directions
        int leftspace = 0;
        int rightspace = 0;
        int left = search(boardinput, row, col, i, leftspace, -1);
        int right = search(boardinput, row, col, (i + 4) % 8, rightspace, -1);

        if (leftspace == 1 && rightspace == 1) { // ���� ������� ���� 
            if (right>left) {
                leftspace = 1;
                rightspace = 0;
                right = search(boardinput, row, col, (i + 4) % 8, rightspace, -1);
                left = search(boardinput, row, col, i, leftspace, -1);
            }
            else {
                leftspace = 0;
                rightspace = 1;
                left = search(boardinput, row, col, i, leftspace, -1);
                right = search(boardinput, row, col, (i + 4) % 8, rightspace, -1);
            }
        }

        if (left + right != 2) { // 33�� �ƴϹǷ�
            continue;
        }


        //check left boundary
        int le = left + leftspace;
        int ri = right + rightspace;
        int r = row;
        int c = col;
        bool blocked = false;
        while (le >= 0) { // >=0�̹Ƿ� ���� ��ġ�� �� ���� �ڸ��� ���� ����.
            assignDirection(r, c, i);
            le--;
        }
        if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE) {
            continue;
            //boundary�� �ɸ� ��� open 3�� �ƴ�
        }
        else if (boardinput[r][c] == 1) { // ~~ == O or X : X�� ������ ���� ������ �ɷ��� ���
            continue;
        }
        else if (boardinput[r][c]==0 ) { // ������� ���
            assignDirection(r, c, i);
            if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE) { // X - B
                blocked = true;
            }
            else if (boardinput[r][c] == -1) { // - X - X ������ space�� ����ϰ� �� ���
                continue;
            }
            else if (boardinput[r][c] == 1) {
                blocked = true;
            }
        }
        // now check right boundary
        r = row;
        c = col;
        while (ri >= 0) { // >=0�̹Ƿ� ���� ��ġ�� �� ���� �ڸ��� ���� ����.
            assignDirection(r, c, (i + 4) % 8);
            ri--;
        }
        if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE) {
            continue;
        }
        else if (boardinput[r][c] == 1 ) { // ~~ == O
            continue;
        }
        else if (boardinput[r][c]==0) { // ������� ���
            assignDirection(r, c, (i + 4) % 8);
            if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE) { // X - B
                if (blocked) {
                    continue;
                }
            }
            else if (boardinput[r][c] == -1) { // - X - X ������ space�� ����ϰ� �� ���
                continue;
            }
            else if (boardinput[r][c] == 1) {
                if (blocked) {
                    continue;
                }
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

void place(int boardinput[][BOARD_SIZE], int row, int col, int stone) { // Stone stone
    //Assume that every constriants are satisfied before entering this function.
    boardinput[row][col] = stone;
    return;
}

bool isOccupied(int boardinput[][BOARD_SIZE], int row, int col) {
    //if board[row][column] is occupied, return true. False if not.
    return (boardinput[row][col] == 0) ? false : true;
}

bool interpret_and_Place(int boardinput[][BOARD_SIZE], string userInput, int whosTurn, bool& isGameOver) { // Stone whosTurn
    //returns 1 if successfully interpreted and placed stone.
    //returns 0 if not. (wrong position, already occupied, or else)
    //interpret user input.
    if (userInput.size() > 3 || userInput.size() < 2) {
        wrongInput();
        return false;
    }
    int col = userInput[0] - 'a';
    int row;
    try {
        row = stoi(userInput.substr(1)) - 1;
    }
    catch (const exception& e) {
        wrongInput();
        return false;
    }
    if (col < 0 || col >26) { // char -> int�� ��ȯ�Ͽ� 'a' ~ 'z' ���.
        wrongInput();
        return false;
    }
    else if (col >= BOARD_SIZE || row >= BOARD_SIZE || row < 0) {
        outOfRange();
        return false;
    }
    else if (isOccupied(boardinput, row, col)) {
        placeOccupied();
        return false;
    }
    else if (whosTurn == -1 && isDoubleThree(boardinput, row, col)) {
        doubleThree();
        return false;
    }
    place(boardinput, row, col, whosTurn);
    if (isFive(boardinput, row, col)) {
        win(boardinput, isGameOver, whosTurn);
        return true;
    }
    return true;
    //if the flow reached here, it means you can place the stone. 
    //but beware : check 5 condition.
}
