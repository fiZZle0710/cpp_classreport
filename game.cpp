#include <bits/stdc++.h>
#include <conio.h>
#include <windows.h>
using namespace std;

const int BOARD_SIZE = 15;

enum class PieceColor
{
    Empty,
    Black, // X
    White  // O
};

struct Position
{
    int x;
    int y;
    Position(int x = 0, int y = 0) : x(x), y(y) {}
};

class GameObject
{
protected:
    Position pos;
    bool active;
    string name;

public:
    GameObject(int x, int y, const string &objName)
        : pos(x, y), active(true), name(objName) {}
    virtual ~GameObject() = default;
    virtual void display() const = 0;
    bool isActive() const { return active; }
    void setActive(bool a) { active = a; }
    Position getPos() const { return pos; }
    void setPos(int x, int y)
    {
        pos.x = x;
        pos.y = y;
    }
    string getName() const { return name; }
};

// ============================================================
// 派生类: ChessBoard - 管理15x15棋盘
// 重写 display() 显示棋盘和棋子
// ============================================================
class ChessBoard : public GameObject
{
private:
    vector<vector<PieceColor>> board; // 使用vector管理二维棋盘
    int size;

public:
    ChessBoard(int s = BOARD_SIZE)
        : GameObject(0, 0, "ChessBoard"), size(s)
    {
        board.resize(size, vector<PieceColor>(size, PieceColor::Empty));
    }

    // 重写: 显示带坐标的棋盘
    void display() const override
    {
        cout << "   ";
        for (int i = 0; i < size; i++)
        {
            if (i < 10)
                cout << " " << i << "  ";
            else
                cout << i << "  ";
        }
        cout << endl;

        for (int i = 0; i < size; i++)
        {
            if (i < 10)
                cout << " " << i << " ";
            else
                cout << i << " ";

            for (int j = 0; j < size; j++)
            {
                if (board[i][j] == PieceColor::Empty)
                {
                    cout << " .  ";
                }
                else if (board[i][j] == PieceColor::Black)
                {
                    cout << " X  ";
                }
                else
                {
                    cout << " O  ";
                }
            }
            cout << endl
                 << endl;
        }
    }

    // 落子
    bool placePiece(int x, int y, PieceColor color)
    {
        if (x < 0 || x >= size || y < 0 || y >= size)
            return false;
        if (board[y][x] != PieceColor::Empty)
            return false;
        board[y][x] = color;
        return true;
    }

    // 检查五子连珠（胜利条件）
    bool checkWin(int x, int y, PieceColor color) const
    {
        int dx[] = {1, 0, 1, 1};
        int dy[] = {0, 1, 1, -1};

        for (int dir = 0; dir < 4; dir++)
        {
            int count = 1;

            for (int step = 1; step < 5; step++)
            {
                int nx = x + dx[dir] * step;
                int ny = y + dy[dir] * step;
                if (nx < 0 || nx >= size || ny < 0 || ny >= size)
                    break;
                if (board[ny][nx] != color)
                    break;
                count++;
            }

            for (int step = 1; step < 5; step++)
            {
                int nx = x - dx[dir] * step;
                int ny = y - dy[dir] * step;
                if (nx < 0 || nx >= size || ny < 0 || ny >= size)
                    break;
                if (board[ny][nx] != color)
                    break;
                count++;
            }

            if (count >= 5)
                return true;
        }
        return false;
    }

    // 检查棋盘是否已满（平局）
    bool isFull() const
    {
        for (const auto &row : board)
            for (auto cell : row)
                if (cell == PieceColor::Empty)
                    return false;
        return true;
    }

    int getSize() const { return size; }
};

// ============================================================
// 派生类: Player - 管理玩家信息和输入
// 重写 display() 显示玩家的棋子
// ============================================================
class Player : public GameObject
{
private:
    PieceColor color;
    string playerName;

public:
    Player(const string &name, PieceColor c)
        : GameObject(0, 0, "Player"), color(c), playerName(name) {}

    void display() const override
    {
        cout << (color == PieceColor::Black ? "X" : "O");
    }

    PieceColor getColor() const { return color; }
    string getPlayerName() const { return playerName; }

    // 获取玩家输入的坐标
    Position getInput() const
    {
        int x, y;
        while (true)
        {
            cout << playerName << " (" << (color == PieceColor::Black ? "X" : "O")
                 << ") 请输入坐标 (x y): ";
            cin >> x >> y;
            if (cin.fail())
            {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "输入无效！请输入数字。" << endl;
                continue;
            }
            return Position(x, y);
        }
    }
};

// ============================================================
// 类: Game - 游戏主控制器
// 管理游戏循环、玩家和游戏状态
// ============================================================
class Game
{
private:
    ChessBoard *board;
    vector<Player *> players;
    int currentPlayerIndex;
    bool gameOver;
    bool gameWin;
    int moveCount;

public:
    Game() : board(nullptr), currentPlayerIndex(0),
             gameOver(false), gameWin(false), moveCount(0)
    {
        board = new ChessBoard();
        // 默认黑棋(X)先手
        players.push_back(new Player("玩家1", PieceColor::Black));
        players.push_back(new Player("玩家2", PieceColor::White));
    }

    ~Game()
    {
        delete board;
        for (auto &p : players)
            delete p;
        players.clear();
    }

    // 主游戏循环
    void run()
    {
        while (!gameOver && !gameWin)
        {
            system("cls");

            cout << "=== 五子棋 ===" << endl;
            cout << "回合: " << moveCount + 1 << endl;
            cout << "当前: ";
            players[currentPlayerIndex]->display();
            cout << "  (黑棋=X先手, 白棋=O后手)" << endl
                 << endl;

            board->display();
            cout << endl;

            Player *currentPlayer = players[currentPlayerIndex];
            Position pos = currentPlayer->getInput();

            if (board->placePiece(pos.x, pos.y, currentPlayer->getColor()))
            {
                moveCount++;

                if (board->checkWin(pos.x, pos.y, currentPlayer->getColor()))
                {
                    system("cls");
                    board->display();
                    cout << endl;
                    cout << "=== " << currentPlayer->getPlayerName()
                         << " (" << (currentPlayer->getColor() == PieceColor::Black ? "X" : "O")
                         << ") 获胜！ ===" << endl;
                    gameWin = true;
                }
                else if (board->isFull())
                {
                    system("cls");
                    board->display();
                    cout << endl;
                    cout << "=== 平局！棋盘已满 ===" << endl;
                    gameOver = true;
                }
                else
                {
                    currentPlayerIndex = (currentPlayerIndex + 1) % 2;
                }
            }
            else
            {
                cout << "该位置已有棋子或坐标无效！请重新输入。" << endl;
                system("pause");
            }
        }

        cout << endl
             << "游戏结束！按任意键退出...";
        _getch();
    }
};

int main()
{
    // 设置控制台编码为UTF-8，确保中文正常显示
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    system("mode con cols=70 lines=40");

    try
    {
        Game game;
        game.run();
    }
    catch (const exception &e)
    {
        cerr << "错误: " << e.what() << endl;
        system("pause");
        return 1;
    }

    return 0;
}
