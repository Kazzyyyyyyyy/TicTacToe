#include <iostream>
#include <vector>
#include <string> 
#include <utility>
#include <stdexcept>
#include <limits>

using namespace std;

char board[3][3]; 
const char freeCellChar = '+', playerChar = 'X', cpuChar = 'O';
int roundsPlayed = 0;

enum GameStates { PLAYER_TURN, CPU_TURN };
GameStates currGameState = PLAYER_TURN;

const vector<vector<pair<int, int>>> winConditions {
    //horizontal
    {{{0,0}, {0,1}, {0,2}}},
    {{{1,0}, {1,1}, {1,2}}},
    {{{2,0}, {2,1}, {2,2}}},

    //vertical
    {{{0,0}, {1,0}, {2,0}}},
    {{{0,1}, {1,1}, {2,1}}},
    {{{0,2}, {1,2}, {2,2}}},

    //diagonal
    {{{0,0}, {1,1}, {2,2}}},
    {{{0,2}, {1,1}, {2,0}}}
}; 


//logic
pair<int, int> cpuLogic() {

    pair<int, int> blockPlayer = {-1, -1}, possibleWinLine = { -1, -1 }; 
    for(vector<pair<int, int>> vec : winConditions) {
        pair<int, int> freeCell;
        int sameInLine = 0; 
        char searchChar = ' '; 

        for(int i = 0; i < vec.size(); i++) { //goes through the 'sets' of win combinations 
            int y = vec[i].first, x = vec[i].second;
            char cell = board[y][x];

            if(cell != freeCellChar && searchChar == ' ') { //defines 'searchChar' and so the searched char for this win combination
                searchChar = cell; 
                sameInLine++; 
            }
            else if(cell == searchChar) { 
                sameInLine++; 
            }
            else if(cell == freeCellChar) { //freeCell is the position, that later gets filled to win 
                freeCell = {y, x}; 
            }
            else        //nothing of the upper ifs are true, this means cell is not searchChar and or space => nobody can win on this combination, 
                break;  //so theres nothing to do here and we can go to the next combination
             
            
            if(sameInLine == 2 && i == vec.size() - 1) { //sameInLine == 2 and every cell of the combination is searched, means the combination can be 
                                                         //completet through placing in freeCell => win
                if(searchChar == cpuChar) //the cpu can win on this turn 
                    return freeCell; //cpu won 
                
                else
                    blockPlayer = freeCell; //cpu can block the player from winning, this doesnt get returned instantly 
                                            //so every combination will be scanned for possible cpu win 
                
            }
            else if(sameInLine == 1 && i == vec.size() - 1 && searchChar == cpuChar) //sameInLine == 1 and every cell of the combination is searched means
                possibleWinLine = freeCell;                                          //that this combination can possibly be won in 2 rounds  
            
        }
    }

    if(blockPlayer.first != -1) //blocking player from winning if possible after the full loop is finished and the cpu couldnt win
        return blockPlayer; 
    
    else if(possibleWinLine.first != -1) //fill one cell in combination that can be now possibly be won next round
        return possibleWinLine; 
    

    const vector<pair<int, int>> preferedCells { {1, 1}, {0, 0}, {0, 2}, {2, 0}, {2, 2} }; 
    for(pair<int, int> p : preferedCells) { //checks powerful cells to have and takes if possible
        if(board[p.first][p.second] == freeCellChar) 
            return p; 
    }

    for(int y = 0; y < 3; y++) { //last option if the actual logic didnt succeed: takes the first free cell found
        for(int x = 0; x < 3; x++) {
            if(board[y][x] == freeCellChar) 
                return {y, x};
        }
    }
    
    throw logic_error("should not reach here");
}

pair<bool, char> checkWin() {
    
    for(vector<pair<int, int>> vec : winConditions) {
        char winChar = ' '; 

        winChar = board[vec[0].first][vec[0].second];

        if(winChar == freeCellChar) 
            continue;
     
        if(winChar == board[vec[1].first][vec[1].second] && 
            winChar == board[vec[2].first][vec[2].second]) 
             return { true, winChar };
    }

    return { false, ' '};
}


//utils
void initializeGame() {
    for(int y = 0; y < 3; y++) {
        for(int x = 0; x < 3; x++) {
            board[y][x] = freeCellChar; 
        }
    }
} 

void printBoard() {

    cout << string(100, '\n');

    for(int y = 0; y < 3; y++) {
        for(int x = 0; x < 3; x++) {
            cout << board[y][x] << ' '; 
        }
        cout << endl;
    }
}


int main() {

    while(true) { //main loop
    
        initializeGame();
        printBoard(); 

        while(true) { //game loop

            switch(currGameState) {
                case GameStates::PLAYER_TURN: 
                    //player input
                    int playerY, playerX;
                    
                    cout << "y: "; cin >> playerY;       
                    cout << "x: "; cin >> playerX; 
                    
                    //index 1-3 => 0-2
                    playerY--; 
                    playerX--; 
                    
                    //if player input isnt usable
                    if (cin.fail()) {
                        cin.clear(); 
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        continue; //new input
                    }
                    
                    //out of bounds and that the cell is free check
                    if(!(0 <= playerY <= 2 && 0 <= playerX <= 2) || board[playerY][playerX] != freeCellChar) 
                        continue; //new input
                    
                    board[playerY][playerX] = playerChar;   
                break;

                case GameStates::CPU_TURN:
                    pair<int, int> cpuTurn = cpuLogic(); 
                    board[cpuTurn.first][cpuTurn.second] = cpuChar; 
                break;
            }
            
            printBoard(); 
            
            roundsPlayed++;
            
            //checks for win and tie
            pair<bool, char> win = checkWin();
            if(win.first) {
                cout << endl << (win.second == playerChar ? "YOU WON!!!" : "CPU WON...") << endl;
                break; 
            } 
            else if(roundsPlayed == 9) { //when roundsPlayed == 9 every cell has to be filled => tie
                cout << endl << "TIE" << endl;
                break; 
            } 
            
            //switch gameState
            currGameState = (currGameState == GameStates::PLAYER_TURN ? GameStates::CPU_TURN : GameStates::PLAYER_TURN); 
        }
        
        //reset or end
        cout << "again? (y / n)\n" << endl;
        char inp; 
        cin >> inp;

        if(inp != 'y') 
            break; 
    }

    return 0; 
}
