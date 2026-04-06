#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

bool isWhitePiece(char board[][8], int sr, int sc);
bool isBlackPiece(char board[][8], int sr, int sc);
bool isOwnPieceAtDestination(char board[][8], int dr, int dc, char player);
void find_white_king(char board[][8], int& kr, int& kc);
void find_black_king(char board[][8], int& kr, int& kc);
bool isCheck(char board[][8], int kr, int kc, char currentPlayer);
bool isValidMove(char board[][8], int sr, int sc, int dr, int dc, char player);
void display_all_moves(char board[][8], int sr, int sc, char player);
void make_move(char board[][8], int sr, int sc, int dr, int dc);
bool hasAnyLegalMove(char board[][8], char player);
bool isCheckmate(char board[][8], char player);
bool isStalemate(char board[][8], char player);
void handlePawnPromotion(char board[][8], int dr, int dc, char player);
bool canCastle(char board[][8], int sr, int sc, int dr, int dc, char player);
bool whiteKingMoved = false, blackKingMoved = false;
bool whiteRookLeftMoved = false, whiteRookRightMoved = false;
bool blackRookLeftMoved = false, blackRookRightMoved = false;
char undo_board[8][8];
char redo_board[8][8];
bool undo_available = false;
bool redo_available = false;
char undoPlayer;
char redoPlayer;
char lastMovedPiece;
bool undo_whiteKingMoved, undo_blackKingMoved;
bool undo_whiteRookLeftMoved, undo_whiteRookRightMoved;
bool undo_blackRookLeftMoved, undo_blackRookRightMoved;
bool redo_whiteKingMoved, redo_blackKingMoved;
bool redo_whiteRookLeftMoved, redo_whiteRookRightMoved;
bool redo_blackRookLeftMoved, redo_blackRookRightMoved;

void loadboard(char board[][8], int rows, int columns)
{
    ifstream file("chess.txt");
    if (!file)
    {
        cout << "Chess file not found" << endl;
        return;
    }
    char ch;
    int row = 0, col = 0;
    while (file >> ch)
    {
        if (ch == '1' || ch == '2' || ch == '3' || ch == '4' || ch == '5' || ch == '6' || ch == '7' || ch == '8')
        {
            continue;
        }
        if (ch == '.')
        {
            board[row][col] = '.';
            col++;
        }
        else if (ch >= 'A' && ch <= 'Z')
        {
            board[row][col] = ch;
            col++;
        }
        else if (ch >= 'a' && ch <= 'z')
        {
            board[row][col] = ch;
            col++;
        }
        if (col == 8)
        {
            col = 0;
            row++;
        }
        if (row == 8)
        {
            break;
        }
    }
}
void displayBoard(char board[][8], int rows, int columns, char currentPlayer)
{
    cout << "   1 2 3 4 5 6 7 8\n";
    for (int i = 0; i < rows; i++)
    {
        cout << i + 1 << "  ";
        for (int j = 0; j < columns; j++)
        {
            cout << board[i][j] << " ";
        }
        cout << endl;
    }
    if (currentPlayer == 'W')
    {
        cout << "Whites turn" << endl;
    }
    else
    {
        cout << "Blacks turn" << endl;
    }
}
void takeInput(int& sr, int& sc)
{
    cout << "Enter source row (1-8) = ";
    cin >> sr;
    cout << "Enter source column (1-8) = ";
    cin >> sc;
    sr--;
    sc--;
}
bool isValidInput(int sr, int sc)
{
    if (sr >= 0 && sr < 8 && sc >= 0 && sc < 8)
    {
        return true;
    }
    return false;
}
bool isWhitePiece(char board[][8], int r, int c)
{
    return (board[r][c] >= 'a' && board[r][c] <= 'z');
}
bool isBlackPiece(char board[][8], int r, int c)
{
    return (board[r][c] >= 'A' && board[r][c] <= 'Z');
}
bool isValidSource(char board[][8], int sr, int sc, char currentPlayer)
{
    if (board[sr][sc] == '.')
    {
        return false;
    }
    if (currentPlayer == 'W' && isWhitePiece(board, sr, sc))
    {
        return true;
    }
    if (currentPlayer == 'B' && isBlackPiece(board, sr, sc))
    {
        return true;
    }
    return false;
}
void takeDestination(int& dr, int& dc)
{
    cout << "Enter destination row (1-8) = ";
    cin >> dr;
    cout << "Enter destination column (1-8) = ";
    cin >> dc;
    dr--;
    dc--;
}
bool isValidPawn(char board[][8], int sr, int sc, int dr, int dc, char player)
{
    if (player == 'W')
    {
        if (dr == sr - 1 && dc == sc && board[dr][dc] == '.')
        {
            return true;
        }
        if (sr == 6 && dr == sr - 2 && dc == sc && board[sr - 1][sc] == '.' && board[dr][dc] == '.')
        {
            return true;
        }
        if (dr == sr - 1 && (dc == sc - 1 || dc == sc + 1) && isBlackPiece(board, dr, dc))
        {
            return true;
        }
    }
    else
    {
        if (dr == sr + 1 && dc == sc && board[dr][dc] == '.')
        {
            return true;
        }
        if (sr == 1 && dr == sr + 2 && dc == sc && board[sr + 1][sc] == '.' && board[dr][dc] == '.')
        {
            return true;
        }
        if (dr == sr + 1 && (dc == sc - 1 || dc == sc + 1) && isWhitePiece(board, dr, dc))
        {
            return true;
        }
    }
    return false;
}
bool isValidRook(char board[][8], int sr, int sc, int dr, int dc)
{
    if (sr != dr && sc != dc)
    {
        return false;
    }
    if (sr == dr)
    {
        int step = (dc > sc) ? 1 : -1;
        for (int c = sc + step; c != dc; c += step)
        {
            if (board[sr][c] != '.')
            {
                return false;
            }
        }
    }
    else
    {
        int step = (dr > sr) ? 1 : -1;
        for (int r = sr + step; r != dr; r += step)
        {
            if (board[r][sc] != '.')
            {
                return false;
            }
        }
    }
    return true;
}

bool isValidKnight(int sr, int sc, int dr, int dc)
{
    int r = dr - sr;
    int c = dc - sc;
    if ((r == 2 || r == -2) && (c == 1 || c == -1))
    {
        return true;
    }
    if ((r == 1 || r == -1) && (c == 2 || c == -2))
    {
        return true;
    }
    return false;
}
bool isValidBishop(char board[][8], int sr, int sc, int dr, int dc)
{
    int rDiff = dr - sr;
    int cDiff = dc - sc;
    if (rDiff != cDiff && rDiff != -cDiff)
    {
        return false;
    }
    int rStep = (rDiff > 0) ? 1 : -1;
    int cStep = (cDiff > 0) ? 1 : -1;
    int r = sr + rStep;
    int c = sc + cStep;
    while (r != dr && c != dc)
    {
        if (board[r][c] != '.')
        {
            return false;
        }
        r = r + rStep;
        c = c + cStep;
    }
    return true;
}
bool isValidKing(int sr, int sc, int dr, int dc)
{
    int r = dr - sr;
    int c = dc - sc;
    if (r >= -1 && r <= 1 && c >= -1 && c <= 1)
    {
        return true;
    }
    return false;
}
bool isValidQueen(char board[][8], int sr, int sc, int dr, int dc)
{
    return isValidRook(board, sr, sc, dr, dc) || isValidBishop(board, sr, sc, dr, dc);
}
bool isOwnPieceAtDestination(char board[][8], int dr, int dc, char player)
{
    if (player == 'W' && isWhitePiece(board, dr, dc))
    {
        return true;
    }
    if (player == 'B' && isBlackPiece(board, dr, dc))
    {
        return true;
    }
    return false;
}

void find_white_king(char board[][8], int& kr, int& kc)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (board[i][j] == 'k')
            {
                kr = i;
                kc = j;
                return;
            }
        }
    }
}
void find_black_king(char board[][8], int& kr, int& kc)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (board[i][j] == 'K')
            {
                kr = i;
                kc = j;
                return;
            }
        }
    }
}
bool isCheck(char board[][8], int kr, int kc, char currentPlayer)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (currentPlayer == 'W' && isBlackPiece(board, i, j))
            {
                if (isValidMove(board, i, j, kr, kc, 'B'))
                {
                    return true;
                }
            }
            if (currentPlayer == 'B' && isWhitePiece(board, i, j))
            {
                if (isValidMove(board, i, j, kr, kc, 'W'))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool isValidMove(char board[][8], int sr, int sc, int dr, int dc, char player)
{
    char piece = board[sr][sc];
    if (isOwnPieceAtDestination(board, dr, dc, player))
    {
        return false;
    }
    bool valid = false;
    if (piece == 'p' || piece == 'P')
    {
        valid = isValidPawn(board, sr, sc, dr, dc, player);
    }
    else if (piece == 'r' || piece == 'R')
    {
        valid = isValidRook(board, sr, sc, dr, dc);
    }
    else if (piece == 'n' || piece == 'N')
    {
        valid = isValidKnight(sr, sc, dr, dc);
    }
    else if (piece == 'b' || piece == 'B')
    {
        valid = isValidBishop(board, sr, sc, dr, dc);
    }
    else if (piece == 'q' || piece == 'Q')
    {
        valid = isValidQueen(board, sr, sc, dr, dc);
    }
    else if (piece == 'k' || piece == 'K')
    {
        valid = isValidKing(sr, sc, dr, dc);
    }
    if ((piece == 'k' && player == 'W') || (piece == 'K' && player == 'B'))
    {
        if (canCastle(board, sr, sc, dr, dc, player))
        {
            valid = true;
        }
    }
    if (!valid)
    {
        return false;
    }
    char temp = board[dr][dc];
    board[dr][dc] = board[sr][sc];
    board[sr][sc] = '.';
    int kr, kc;
    if (player == 'W')
    {
        find_white_king(board, kr, kc);
    }
    else
    {
        find_black_king(board, kr, kc);
    }
    bool inCheck = isCheck(board, kr, kc, player);
    board[sr][sc] = board[dr][dc];
    board[dr][dc] = temp;
    return !inCheck;
}

void make_move(char board[][8], int sr, int sc, int dr, int dc)
{
    char piece = board[sr][sc];
    board[dr][dc] = board[sr][sc];
    board[sr][sc] = '.';
    if (piece == 'k')
    {
        whiteKingMoved = true;
    }
    if (piece == 'K')
    {
        blackKingMoved = true;
    }
    if (sr == 7 && sc == 0)
    {
        whiteRookLeftMoved = true;
    }
    if (sr == 7 && sc == 7)
    {
        whiteRookRightMoved = true;
    }
    if (sr == 0 && sc == 0)
    {
        blackRookLeftMoved = true;
    }
    if (sr == 0 && sc == 7)
    {
        blackRookRightMoved = true;
    }
    if (piece == 'k')
    {
        if (sc == 4 && dc == 6)
        {
            board[7][5] = 'r'; board[7][7] = '.'; whiteRookRightMoved = true;
        }
        if (sc == 4 && dc == 2)
        {
            board[7][3] = 'r'; board[7][0] = '.';
            whiteRookLeftMoved = true;
        }
    }
    if (piece == 'K')
    {
        if (sc == 4 && dc == 6)
        {
            board[0][5] = 'R'; board[0][7] = '.';
            blackRookRightMoved = true;
        }
        if (sc == 4 && dc == 2)
        {
            board[0][3] = 'R'; board[0][0] = '.';
            blackRookLeftMoved = true;
        }
    }
}
void display_all_moves(char board[][8], int sr, int sc, char player)
{
    cout << "Legal moves for piece at (" << sr + 1 << ", " << sc + 1 << "):\n";
    for (int dr = 0; dr < 8; dr++)
    {
        for (int dc = 0; dc < 8; dc++)
        {
            if (sr == dr && sc == dc)
            {
                continue;
            }
            if (isValidMove(board, sr, sc, dr, dc, player))
            {
                cout << " (" << dr + 1 << ", " << dc + 1 << ")\n";
            }
        }
    }
}
bool hasAnyLegalMove(char board[][8], char player)
{
    for (int sr = 0; sr < 8; sr++)
    {
        for (int sc = 0; sc < 8; sc++)
        {
            if ((player == 'W' && isWhitePiece(board, sr, sc)) || (player == 'B' && isBlackPiece(board, sr, sc)))
            {
                for (int dr = 0; dr < 8; dr++)
                {
                    for (int dc = 0; dc < 8; dc++)
                    {
                        if (isValidMove(board, sr, sc, dr, dc, player))
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}
bool isCheckmate(char board[][8], char player)
{
    int kr, kc;
    if (player == 'W')
    {
        find_white_king(board, kr, kc);
    }
    else
    {
        find_black_king(board, kr, kc);
    }
    if (!isCheck(board, kr, kc, player))
    {
        return false;
    }
    if (hasAnyLegalMove(board, player))
    {
        return false;
    }
    return true;
}
bool isStalemate(char board[][8], char player)
{
    int kr, kc;
    if (player == 'W')
    {
        find_white_king(board, kr, kc);
    }
    else
    {
        find_black_king(board, kr, kc);
    }
    if (isCheck(board, kr, kc, player))
    {
        return false;
    }
    if (hasAnyLegalMove(board, player))
    {
        return false;
    }
    return true;
}
void handlePawnPromotion(char board[][8], int dr, int dc, char player)
{
    if (player == 'W' && board[dr][dc] == 'p' && dr == 0)
    {
        char choice;
        cout << "\nPawn Promotion Choose (q, r, b, n): ";
        cin >> choice;
        if (choice == 'q')
        {
            board[dr][dc] = 'q';
        }
        else if (choice == 'r')
        {
            board[dr][dc] = 'r';
        }
        else if (choice == 'b')
        {
            board[dr][dc] = 'b';
        }
        else if (choice == 'n')
        {
            board[dr][dc] = 'n';
        }
        else
        {
            cout << "Invalid choice promoted to queen by default\n";
            board[dr][dc] = 'q';
        }
    }
    if (player == 'B' && board[dr][dc] == 'P' && dr == 7)
    {
        char choice;
        cout << "\nPawn Promotion choose (Q, R, B, N): ";
        cin >> choice;

        if (choice == 'Q')
        {
            board[dr][dc] = 'Q';
        }
        else if (choice == 'R')
        {
            board[dr][dc] = 'R';
        }
        else if (choice == 'B')
        {
            board[dr][dc] = 'B';
        }
        else if (choice == 'N')
        {
            board[dr][dc] = 'N';
        }
        else
        {
            cout << "Invalid choice promoted to queen by default.\n";
            board[dr][dc] = 'Q';
        }
    }
}
bool canCastle(char board[][8], int sr, int sc, int dr, int dc, char player)
{
    char temp[8][8];
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            temp[i][j] = board[i][j];
        }
    }
    if (player == 'W')
    {
        if (whiteKingMoved || sr != 7 || sc != 4)
        {
            return false;
        }
        if (dr == 7 && dc == 6 && !whiteRookRightMoved && board[7][7] == 'r' && board[7][5] == '.' && board[7][6] == '.')
        {
            if (isCheck(board, 7, 4, 'W'))
            {
                return false;
            }
            temp[7][5] = 'k'; temp[7][4] = '.';
            if (isCheck(temp, 7, 5, 'W'))
            {
                return false;
            }
            temp[7][6] = 'k'; temp[7][5] = '.';
            if (isCheck(temp, 7, 6, 'W'))
            {
                return false;
            }
            return true;
        }
        if (dr == 7 && dc == 2 && !whiteRookLeftMoved && board[7][0] == 'r' && board[7][1] == '.' && board[7][2] == '.' && board[7][3] == '.')
        {
            if (isCheck(board, 7, 4, 'W'))
            {
                return false;
            }
            temp[7][3] = 'k'; temp[7][4] = '.';
            if (isCheck(temp, 7, 3, 'W'))
            {
                return false;
            }
            temp[7][2] = 'k'; temp[7][3] = '.';
            if (isCheck(temp, 7, 2, 'W'))
            {
                return false;
            }
            return true;
        }
    }
    else
    {
        if (blackKingMoved || sr != 0 || sc != 4)
        {
            return false;
        }
        if (dr == 0 && dc == 6 && !blackRookRightMoved && board[0][7] == 'R' && board[0][5] == '.' && board[0][6] == '.')
        {
            if (isCheck(board, 0, 4, 'B'))
            {
                return false;
            }
            temp[0][5] = 'K'; temp[0][4] = '.';
            if (isCheck(temp, 0, 5, 'B'))
            {
                return false;
            }
            temp[0][6] = 'K'; temp[0][5] = '.';
            if (isCheck(temp, 0, 6, 'B'))
            {
                return false;
            }
            return true;
        }
        if (dr == 0 && dc == 2 && !blackRookLeftMoved && board[0][0] == 'R' && board[0][1] == '.' && board[0][2] == '.' && board[0][3] == '.')
        {
            if (isCheck(board, 0, 4, 'B'))
            {
                return false;
            }
            temp[0][3] = 'K'; temp[0][4] = '.';
            if (isCheck(temp, 0, 3, 'B'))
            {
                return false;
            }
            temp[0][2] = 'K'; temp[0][3] = '.';
            if (isCheck(temp, 0, 2, 'B'))
            {
                return false;
            }
            return true;
        }
    }
    return false;
}

void copyBoard(char src[8][8], char dest[8][8])
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            dest[i][j] = src[i][j];
        }
    }
}
void SaveGame(char board[8][8], char currentPlayer)
{
    ofstream file("save.txt");
    if (!file)
    {
        cout << "Error saving game\n";
        return;
    }
    file << currentPlayer << "\n";
    file << whiteKingMoved << " " << blackKingMoved << " " << whiteRookLeftMoved << " " << whiteRookRightMoved << " " << blackRookLeftMoved << " " << blackRookRightMoved << "\n";
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            file << board[i][j] << " ";
        }
        file << "\n";
    }
    file.close();
    cout << "Game Saved\n";
}

bool LoadGame(char board[8][8], char& currentPlayer)
{
    ifstream file("save.txt");
    if (!file)
    {
        cout << "No saved game found\n";
        return false;
    }
    file >> currentPlayer;
    file >> whiteKingMoved >> blackKingMoved >> whiteRookLeftMoved >> whiteRookRightMoved >> blackRookLeftMoved >> blackRookRightMoved;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            file >> board[i][j];
        }
    }
    file.close();
    return true;
}
bool isSafeCapture(char board[8][8], int sr, int sc, int dr, int dc, char player)
{
    char temp1 = board[dr][dc];
    char temp2 = board[sr][sc];
    board[dr][dc] = board[sr][sc];
    board[sr][sc] = '.';
    bool safe = true;
    char opponent = (player == 'W') ? 'B' : 'W';
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            if ((opponent == 'W' && isWhitePiece(board, r, c)) || (opponent == 'B' && isBlackPiece(board, r, c)))
            {
                if (isValidMove(board, r, c, dr, dc, opponent))
                {
                    safe = false;
                }
            }
        }
    }
    board[sr][sc] = temp2;
    board[dr][dc] = temp1;
    return safe;
}
void getAIMove(char board[8][8], char player, int& sr, int& sc, int& dr, int& dc)
{
    for (sr = 0; sr < 8; sr++)
    {
        for (sc = 0; sc < 8; sc++)
        {
            if ((player == 'W' && isWhitePiece(board, sr, sc)) || (player == 'B' && isBlackPiece(board, sr, sc)))
            {
                for (dr = 0; dr < 8; dr++)
                {
                    for (dc = 0; dc < 8; dc++)
                    {
                        if (isValidMove(board, sr, sc, dr, dc, player) && board[dr][dc] != '.' && isSafeCapture(board, sr, sc, dr, dc, player))
                        {
                            return;
                        }
                    }
                }
            }
        }
    }
    for (sr = 0; sr < 8; sr++)
    {
        for (sc = 0; sc < 8; sc++)
        {
            if ((player == 'W' && isWhitePiece(board, sr, sc)) || (player == 'B' && isBlackPiece(board, sr, sc)))
            {
                for (dr = 0; dr < 8; dr++)
                {
                    for (dc = 0; dc < 8; dc++)
                    {
                        if (isValidMove(board, sr, sc, dr, dc, player) && board[dr][dc] != '.')
                        {
                            return;
                        }
                    }
                }
            }
        }
    }
    for (sr = 0; sr < 8; sr++)
    {
        for (sc = 0; sc < 8; sc++)
        {
            if ((player == 'W' && board[sr][sc] == 'k') || (player == 'B' && board[sr][sc] == 'K'))
            {
                for (dr = 0; dr < 8; dr++)
                {
                    for (dc = 0; dc < 8; dc++)
                    {
                        if (canCastle(board, sr, sc, dr, dc, player))
                        {
                            return;
                        }
                    }
                }
            }
        }
    }
    for (sr = 0; sr < 8; sr++)
    {
        for (sc = 0; sc < 8; sc++)
        {
            if ((player == 'W' && isWhitePiece(board, sr, sc)) || (player == 'B' && isBlackPiece(board, sr, sc)))
            {
                for (dr = 0; dr < 8; dr++)
                {
                    for (dc = 0; dc < 8; dc++)
                    {
                        if (isValidMove(board, sr, sc, dr, dc, player))
                        {
                            if ((player == 'W' && dr <= 3) || (player == 'B' && dr >= 4))
                            {
                                return;
                            }
                        }
                    }
                }
            }
        }
    }
    for (sr = 0; sr < 8; sr++)
    {
        for (sc = 0; sc < 8; sc++)
        {
            if ((player == 'W' && isWhitePiece(board, sr, sc)) || (player == 'B' && isBlackPiece(board, sr, sc)))
            {
                for (dr = 0; dr < 8; dr++)
                {
                    for (dc = 0; dc < 8; dc++)
                    {
                        if (isValidMove(board, sr, sc, dr, dc, player))
                        {
                            return;
                        }
                    }
                }
            }
        }
    }
}
int main()
{
    char board[8][8];
    char currentPlayer = 'W';
    bool playWithAI = false;
    cout << "1 = New Game (Human vs Human)\n";
    cout << "2 = New Game (Human vs AI)\n";
    cout << "3 = Load Previous Game\n";
    cout << "4 = Exit\n";
    cout << "Choice: ";
    int startChoice;
    cin >> startChoice;
    if (startChoice == 1)
    {
        loadboard(board, 8, 8);
        playWithAI = false;
    }
    else if (startChoice == 2)
    {
        loadboard(board, 8, 8);
        playWithAI = true;
    }
    else if (startChoice == 3)
    {
        if (!LoadGame(board, currentPlayer))
        {
            loadboard(board, 8, 8);
            currentPlayer = 'W';
        }
        cout << "Play with AI? (1 = Yes, 0 = No): ";
        cin >> playWithAI;
    }
    else
    {
        return 0;
    }
    while (true)
    {
        system("cls");
        displayBoard(board, 8, 8, currentPlayer);
        int kr, kc;
        if (currentPlayer == 'W')
        {
            find_white_king(board, kr, kc);
        }
        else
        {
            find_black_king(board, kr, kc);
        }
        if (isCheck(board, kr, kc, currentPlayer))
        {
            cout << "You are in check\n";
        }
        if (!isCheck(board, kr, kc, currentPlayer) && !hasAnyLegalMove(board, currentPlayer))
        {
            cout << "Stalemate! No legal moves.\n";
        }
        if (currentPlayer == 'W')
        {
            if (canCastle(board, 7, 4, 7, 6, 'W'))
            {
                cout << " Kingside castling available!\n";
            }
            if (canCastle(board, 7, 4, 7, 2, 'W'))
            {
                cout << " Queenside castling available!\n";
            }
        }
        else
        {
            if (canCastle(board, 0, 4, 0, 6, 'B'))
            {
                cout << " Kingside castling available!\n";
            }
            if (canCastle(board, 0, 4, 0, 2, 'B'))
            {
                cout << "? Queenside castling available!\n";
            }
        }
        char opponent = (currentPlayer == 'W') ? 'B' : 'W';
        if (playWithAI && currentPlayer == 'B')
        {
            int sr, sc, dr, dc;
            getAIMove(board, currentPlayer, sr, sc, dr, dc);
            copyBoard(board, undo_board);
            undoPlayer = currentPlayer;
            undo_whiteKingMoved = whiteKingMoved;
            undo_blackKingMoved = blackKingMoved;
            undo_whiteRookLeftMoved = whiteRookLeftMoved;
            undo_whiteRookRightMoved = whiteRookRightMoved;
            undo_blackRookLeftMoved = blackRookLeftMoved;
            undo_blackRookRightMoved = blackRookRightMoved;
            make_move(board, sr, sc, dr, dc);
            handlePawnPromotion(board, dr, dc, currentPlayer);
            undo_available = true;
            redo_available = false;
        }
        else
        {
            cout << "\n1 = Move   2 = Undo   3 = Redo   4 = Save Game   5 = Exit\nChoice: ";
            int choice;
            cin >> choice;
            if (choice == 2 && undo_available)
            {
                copyBoard(board, redo_board);
                redoPlayer = currentPlayer;
                redo_whiteKingMoved = whiteKingMoved;
                redo_blackKingMoved = blackKingMoved;
                redo_whiteRookLeftMoved = whiteRookLeftMoved;
                redo_whiteRookRightMoved = whiteRookRightMoved;
                redo_blackRookLeftMoved = blackRookLeftMoved;
                redo_blackRookRightMoved = blackRookRightMoved;
                copyBoard(undo_board, board);
                currentPlayer = undoPlayer;
                whiteKingMoved = undo_whiteKingMoved;
                blackKingMoved = undo_blackKingMoved;
                whiteRookLeftMoved = undo_whiteRookLeftMoved;
                whiteRookRightMoved = undo_whiteRookRightMoved;
                blackRookLeftMoved = undo_blackRookLeftMoved;
                blackRookRightMoved = undo_blackRookRightMoved;
                undo_available = false;
                redo_available = true;
                continue;
            }
            if (choice == 3 && redo_available)
            {
                copyBoard(board, undo_board);
                undoPlayer = currentPlayer;
                undo_whiteKingMoved = whiteKingMoved;
                undo_blackKingMoved = blackKingMoved;
                undo_whiteRookLeftMoved = whiteRookLeftMoved;
                undo_whiteRookRightMoved = whiteRookRightMoved;
                undo_blackRookLeftMoved = blackRookLeftMoved;
                undo_blackRookRightMoved = blackRookRightMoved;
                copyBoard(redo_board, board);
                currentPlayer = redoPlayer;
                whiteKingMoved = redo_whiteKingMoved;
                blackKingMoved = redo_blackKingMoved;
                whiteRookLeftMoved = redo_whiteRookLeftMoved;
                whiteRookRightMoved = redo_whiteRookRightMoved;
                blackRookLeftMoved = redo_blackRookLeftMoved;
                blackRookRightMoved = redo_blackRookRightMoved;
                redo_available = false;
                undo_available = true;
                continue;
            }
            if (choice == 4)
            {
                SaveGame(board, currentPlayer);
                break;
            }
            if (choice == 5)
            {
                break;
            }
            if (choice != 1)
            {
                continue;
            }
            int sr, sc, dr, dc;
            while (true)
            {
                takeInput(sr, sc);
                if (!isValidInput(sr, sc) || !isValidSource(board, sr, sc, currentPlayer))
                {
                    continue;
                }
                bool hasMove = false;
                for (int r = 0; r < 8; r++)
                {
                    for (int c = 0; c < 8; c++)
                    {
                        if (isValidMove(board, sr, sc, r, c, currentPlayer))
                        {
                            hasMove = true;
                        }
                    }
                }

                if (hasMove) break;
            }
            display_all_moves(board, sr, sc, currentPlayer);
            while (true)
            {
                takeDestination(dr, dc);
                if (isValidInput(dr, dc) && isValidMove(board, sr, sc, dr, dc, currentPlayer))
                {
                    break;
                }
            }
            copyBoard(board, undo_board);
            undoPlayer = currentPlayer;
            undo_whiteKingMoved = whiteKingMoved;
            undo_blackKingMoved = blackKingMoved;
            undo_whiteRookLeftMoved = whiteRookLeftMoved;
            undo_whiteRookRightMoved = whiteRookRightMoved;
            undo_blackRookLeftMoved = blackRookLeftMoved;
            undo_blackRookRightMoved = blackRookRightMoved;
            make_move(board, sr, sc, dr, dc);
            handlePawnPromotion(board, dr, dc, currentPlayer);
            undo_available = true;
            redo_available = false;
        }

        if (isCheckmate(board, opponent))
        {
            cout << "\n Checkmate! Player " << ((opponent == 'W') ? "White" : "Black") << " loses.\n";
            break;
        }
        if (isStalemate(board, opponent))
        {
            cout << "\nStalemate! Game is a draw.\n";
            break;
        }
        currentPlayer = opponent;
        cout << "\nPress Enter for next move";
        cin.ignore();
        cin.get();
    }
    return 0;
}
