#include <iostream>
#include <stdio.h>
#include <chrono>
#include <thread>

#include <string>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

#define MAX 100

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37;1m"
#define YELLOW  "\033[33m"
#define BOLD    "\033[1m"
#define BGCYAN  "\033[46m"   // cyan background for highlights

#define BGBLUE   "\033[44m"
#define BGGREEN  "\033[42m"
#define BGYELLOW "\033[43m"
#define BGWHITE  "\033[47m"
#define BLACK    "\033[30m"

#define BOLD    "\033[1m"
#define ITALIC  "\033[3m"
#define UNDERLINE "\033[4m"
#define BLINK   "\033[5m"
#define REVERSE "\033[7m"
#define MAGENTA "\033[35m"

// Cell status codes
#define EMPTY       0   // user-fillable but still empty
#define GIVEN       1   // predefined puzzle number
#define CORRECT     2   // user placed correctly
#define BACKTRACKED 3   // solver failed here, backtracked


class Sudoku
{
    public :
        bool playGame(int arr[][9],int,int,int,int [][9]);
        bool isSafe(const int arr[][9],int,int,int);
        bool isGridFull(const int arr[][9]);
        void printBoard(const int [9][9],const int [][9],int,int);
        void displayWinMessage();
        bool gridChecking(const int arr[][9],int,int,int);
        void startDisplay();
};

class stk
{
    public:
        int i,j,value;
        
        friend void push(stk*,int,int,int);
        //friend void pop();
};

stk undo[MAX];  int u1 = -1;
stk redo[MAX];  int r1 = -1;

void push(stk* s,int i,int j,int value)
{
    s->i = i;
    s->j = j;
    s->value = value;
}

void unDo(int board[9][9],int cellStatus[9][9])
{
    if(u1>=0)
    {
        push(&(redo[++r1]),undo[u1].i,undo[u1].j,undo[u1].value);
        board[undo[u1].i][undo[u1].j] = 0;
        cellStatus[undo[u1].i][undo[u1].j] = EMPTY;
        u1--;
    }
    else
    {
        std::cout<<YELLOW<<"\n\t\t\t\t No more moves to undo!"<<RESET<<std::endl;
    }
}

void reDo(int board[9][9],int cellStatus[9][9])
{
    if(r1>=0)
    {
        push(&(undo[++u1]),redo[r1].i,redo[r1].j,redo[r1].value);
        board[redo[r1].i][redo[r1].j] = redo[r1].value;
        cellStatus[redo[r1].i][redo[r1].j] = CORRECT;
        r1--;
    }
    else
    {
        std::cout<<YELLOW<<"\n\t\t\t\t No moves to redo!"<<RESET<<std::endl;
    }
}

bool Sudoku :: playGame(int board[9][9],int i,int j,int value,int cellStatus[9][9])
{
    int visited[10] = {0,0,0,0,0,0,0,0,0,0};
    visited[value] = 1;

    int newI = 0,newJ = 0,newValue = 0,count = 0;

    if(board[i][j] == 0)
    {
        while(1)
        {
            if(isSafe(board,i,j,value))
            {
                board[i][j] = value;
                cellStatus[i][j] = CORRECT;
                std::cout<<GREEN<<"\n\t\t\t\t The value is placed successfully!"<<RESET<<std::endl;
                visited[value] = 1;

                push(&(undo[++u1]),i,j,value);
                printBoard(board,cellStatus,i,j);
                
                char ch;
                std::cout<<"\n\t\t\t\t Do you want to UNDO your move??"<<std::endl;
                std::cout<<"\t\t\t\t Y:YES        N:NO"<<std::endl;
                std::cin.ignore();
                std::cout<<"\t\t\t\t ";
                std::cin>>ch;

                while(ch=='Y' || ch=='y')
                {
                    int ur;
                    std::cout<<"\n\t\t\t\t "<<YELLOW<<"1.UNDO"<<"\t\t\t\t"<<"2.REDO"<<RESET<<std::endl;
                    std::cout<<RED<<"\t\t\t\t\t\t 3.EXIT"<<RESET<<std::endl;
                    std::cout<<"\n\t\t\t\t Enter your choice : ";
                    std::cin>>ur;

                    switch(ur)
                    {
                        case 1:
                            if (u1 >= MAX-1) 
                            {
                                std::cout << RED << "Undo stack full!" << RESET << std::endl;
                                //return;
                            }
                            else{
                            unDo(board,cellStatus);
                            printBoard(board,cellStatus,i,j);
                            }
                        
                            break;

                        case 2:
                            if (r1 >= MAX-1) 
                            {
                                std::cout << RED << "Redo stack full!" << RESET << std::endl;
                                //return;
                            }
                            else{
                            reDo(board,cellStatus);
                            printBoard(board,cellStatus,i,j);
                            }

                            break;
                        
                        case 3:
                            ch = 'N';
                            break;

                        default:
                            std::cout<<RED<<"\n\t\t\t\t Invalid choice!Please try again"<<RESET<<std::endl;
                    }
                }

                if(isGridFull(board))
                {
                    return true;
                }
    
                std::cout<<"\n\t\t\t\t Enter new position : "<<std::endl;
                std::cout<<"\t\t\t\t Enter row number : ";
                std::cin>>newI;
                std::cout<<"\t\t\t\t Enter column number : ";
                std::cin>>newJ;
                std::cout<<"\t\t\t\t Enter value : ";
                std::cin>>newValue;

                bool a = playGame(board,newI,newJ,newValue,cellStatus);

                if(a == true)
                {
                    return true;
                }
                else
                {
                    board[i][j] = 0;
                    std::cout<<YELLOW<<"\n\t\t\t\t Backtracked to position arr["<<i<<"]["<<j<<"]."<<RESET<<std::endl;
                    cellStatus[i][j] = BACKTRACKED;
                    printBoard(board,cellStatus,i,j); // 🔙 yellow highlight for backtrack
                    
                    //std::this_thread::sleep_for(std::chrono::milliseconds(400));
                    std::cout<<"\n\t\t\t\t Enter new value at arr["<<i<<"]["<<j<<"] : ";
                    std::cin>>value;

                    while(visited[value])
                    {
                        std::cout<<RED<<"\n\t\t\t\t You have already entered this value earlier.Try another input."<<RESET<<std::endl;
                        std::cout<<"\n\t\t\t\t Enter new value : ";
                        std::cin>>value;
                    }
                    visited[value] = 1;
                }
            }
            else
            {
                bool a = true;
                for(int i=1;i<10;i++)
                {
                    if(visited[i] == 0)
                        a = false;
                }

                if(a)  //if(visited={1})
                {
                    std::cout<<YELLOW<<"\n\t\t\t\t You have tried all possibilities from(1-9) here.Backtracking to your previous position."<<RESET<<std::endl;
                    board[i][j] = 0;
                    return false;
                }

                std::cout << RED << "\n\t\t\t\t Invalid move!Try again." << RESET << std::endl;
                std::cout<<"\n\t\t\t\t Enter new value : ";
                std::cin>>value;

                while(visited[value])
                {
                    std::cout<<RED<<"\n\t\t\t\t You have already entered this value earlier.Try another input."<<RESET<<std::endl;
                    std::cout<<"\n\t\t\t\t Enter new value : ";
                    std::cin>>value;
                } 

                visited[value] = 1;
            }
        }
    }
    /*else
    {
        std::cout<<"\n\tThe position is not empty.\n"<<std::endl;
    }*/
}

bool Sudoku::isSafe(const int board[9][9],int i,int j,int value)
{
    //For row checking
    for(int m=0;m<9;m++)        
    {
        if(board[i][m] == value)
        {
            return false;
        }
    }

    //For column checking
    for(int n=0;n<9;n++)    
    {
        if(board[n][j] == value)
        {
            return false;
        }  
    }

    //Grid checking
    return gridChecking(board,i,j,value);
}

bool Sudoku::gridChecking(const int board[9][9],int i,int j,int value)
{
    int endRowIndex,endColIndex;
    
    if(i%3==0)  endRowIndex = i+2;
    else if(i%3==2) endRowIndex = i;
    else    endRowIndex = i%3+i;

    if(j%3==0)  endColIndex = j+2;
    else if(j%3==2) endColIndex = j;
    else endColIndex = j%3+j;

    for(int m = endRowIndex-2;m<=endRowIndex;m++)
    {
        for(int n = endColIndex-2;n<=endColIndex;n++)
        {
            if(!(m==i && n==j))
            {
                if(board[m][n] == value)    return false;
            }
        }
    }

    return true;
}

bool Sudoku::isGridFull(const int board[9][9])
{
    for(int i=0;i<9;i++)
    {
        for(int j=0;j<9;j++)
        {
            if(board[i][j] == 0)
                return false;
        }
    }
    return true;
}

void Sudoku::printBoard(const int board[9][9], const int cellStatus[9][9], int selRow, int selCol)
{
    bool hasSelection = (selRow >= 0 && selCol >= 0);
    int gridRowStart = hasSelection ? (selRow / 3) * 3 : -1;
    int gridColStart = hasSelection ? (selCol / 3) * 3 : -1;

    // Column indices
    printf("\n\t\t\t\t\t\t\t    ");
    for (int j = 0; j < 9; j++) 
    {
        printf("%d ", j);
        if ((j + 1) % 3 == 0 && j != 8) printf("  ");
    }
    printf("\n\t\t\t\t\t\t\t  +-------+-------+-------+\n");

    for (int i = 0; i < 9; i++) 
    {
        printf("\t\t\t\t\t\t\t%d | ", i);
        for (int j = 0; j < 9; j++) 
        {
            bool isInHighlight = hasSelection && (i == selRow || j == selCol || (i >= gridRowStart && i < gridRowStart + 3 && j >= gridColStart && j < gridColStart + 3));
            const char *bg = isInHighlight ? BGCYAN : "";

            if (board[i][j] == 0) 
            {
                if (cellStatus[i][j] == BACKTRACKED)
                    printf("%s" YELLOW ". " RESET, bg);
                else
                    printf("%s" CYAN ". " RESET, bg);
            } 
            else 
            {
                if (cellStatus[i][j] == GIVEN)
                    printf("%s%s%d " RESET, bg, WHITE, board[i][j]);
                else if (cellStatus[i][j] == CORRECT)
                    printf("%s%s%d " RESET, BGGREEN, BLACK, board[i][j]);
                else if (cellStatus[i][j] == BACKTRACKED)
                    printf("%s%s%d " RESET, BGYELLOW, BLACK, board[i][j]);
                else
                    printf("%s%s%d " RESET, bg, BLACK, board[i][j]);
            }

            if ((j + 1) % 3 == 0) printf("| ");
        }

        printf("\n");
        if ((i + 1) % 3 == 0)
            printf("\t\t\t\t\t\t\t  +-------+-------+-------+\n");
    }

    printf("\n\t\t\t\t Legend: %sWhite=Given%s | %sGreen=Correct%s | %sYellow=Backtracked%s | %sCyan=Empty/Highlight%s\n\n",
           WHITE, RESET, GREEN, RESET, YELLOW, RESET, CYAN, RESET);
}


void initCellStatus(const int board[9][9], int cellStatus[9][9]) 
{
    for (int i = 0; i < 9; i++) 
    {
        for (int j = 0; j < 9; j++) 
        {
            if (board[i][j] != 0)
                cellStatus[i][j] = GIVEN;   // Predefined puzzle numbers
            else
                cellStatus[i][j] = EMPTY;   // Empty, player will fill
        }
    }
}


void Sudoku::displayWinMessage()
{
    std::cout<<RED<<"\n\tCongratulation!You have won the game."<<RESET<<std::endl;
}

void sleep_ms(int ms) 
{
    #ifdef _WIN32
        Sleep(ms);
    #else
        usleep(ms * 1000);
    #endif
}

void Sudoku::startDisplay()
{
    string lines[] = 
    {
        CYAN BOLD "\t\t\t\t\t\t******************************************\n\t\t\t\t\t\t\t\t" RESET,
        CYAN BOLD  RESET YELLOW BOLD "\t\t\t\t\t\t           $ SUDOKU MASTER $          " RESET CYAN BOLD  RESET,
        CYAN BOLD "\n\t\t\t\t\t\t******************************************" RESET,
        //WHITE ITALIC "           by Team Logic Ninjas 🧠\n" RESET,
       // GREEN "▶ Objective: " RESET WHITE "Fill the grid so every row, column," RESET,
        //WHITE "  and 3×3 box contains digits 1–9 exactly once!\n" RESET,
        MAGENTA "\n\n        \t\t\t\t\tTips:" RESET " Use logic, not luck! Try to spot patterns.\n" RESET
    };

    for (const string &line : lines) 
    {
        cout << line << endl;
        //std::this_thread::sleep_for(chrono::milliseconds(200)); // delay between lines
        sleep_ms(500);
    }
    std::cout<<endl;
}

int main()
{
    int i,j,value;
    Sudoku s;

    int sudoku[9][9] = {
                        {0,7,0,5,8,3,0,2,0},
                        {0,5,9,2,0,0,3,0,0},
                        {3,4,0,0,0,6,5,0,7},

                        {7,9,5,0,0,0,6,3,2},
                        {0,0,3,6,9,7,1,0,0},
                        {6,8,0,0,0,2,7,0,0},

                        {9,1,4,8,3,5,0,7,6},
                        {0,3,0,7,0,1,4,9,5},
                        {5,6,7,4,2,9,0,1,3},
                    };

    int cellStatus[9][9];

    initCellStatus(sudoku,cellStatus);

    s.startDisplay();
    sleep_ms(500);
    s.printBoard(sudoku,cellStatus,-1,-1);

    std::cout<<"\n\t\t\t\t Enter position : ";
    std::cout<<"\n\t\t\t\t Enter row number : ";
    std::cin>>i;
    std::cout<<"\t\t\t\t Enter column number : ";
    std::cin>>j;
    std::cout<<"\t\t\t\t Enter value : ";
    std::cin>>value;

    bool a = s.playGame(sudoku,i,j,value,cellStatus);  
    
    if(a == true)
    {
        s.displayWinMessage();
    }
    else
    {
        std::cout<<"\n\tNot won,Try again."<<std::endl;
    }
         
    return 0;
}