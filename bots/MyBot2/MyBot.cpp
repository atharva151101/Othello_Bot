/*
* @file botTemplate.cpp
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-02-04
* Template for users to create their own bots
*/

#include "Othello.h"
#include "OthelloBoard.h"
#include "OthelloPlayer.h"
#include <cstdlib>
#include <unordered_map>
#include <chrono>

using namespace std;
using namespace Desdemona;
const int MAX = 1000000; 
const int MIN = -1000000;
const int panda = -10000000;
auto start = chrono::steady_clock::now();

class MyBot: public OthelloPlayer
{
    public:
        
        /**
         * Initialisation routines here
         * This could do anything from open up a cache of "best moves" to
         * spawning a background processing thread. 
         */
        MyBot( Turn turn );

        /**
         * Play something 
         */
        virtual Move play(const OthelloBoard& board );
        virtual int Heur(const OthelloBoard& board, list<Move>::iterator ptr, Turn root, Turn current);
        // virtual void dummy(OthelloBoard& board, list<Move>::iterator ptr);
        virtual int Positions(const OthelloBoard& board, Turn current, list<Move>::iterator ptr);
        virtual int Parity(OthelloBoard& board);
        virtual int Corners(const OthelloBoard& board, Turn current);
        virtual int CoinsD(OthelloBoard& board, Turn root);
        virtual int MiniMax(Turn root, Turn current, int depth, int maxDepth, OthelloBoard& board, list<Move>::iterator ptr, int alpha, int beta);
        virtual int mahaStability(const OthelloBoard& board, Turn current);
        virtual int Stability(const OthelloBoard& board, Turn current);
        virtual int Mobility(const OthelloBoard& board, Turn current);
    private:
        
};

MyBot::MyBot( Turn turn )
    : OthelloPlayer( turn )
{
}

Move MyBot::play(const OthelloBoard& board)
{
    start = chrono::steady_clock::now();
    list<Move> moves = board.getValidMoves( turn );
    list<Move>::iterator ptr; 
    list<Move>::iterator maxit = moves.begin();
    int maxi = -10000000; 
    for (int j = 3; j < 64; j+=1){
        // cout << j << "\n";
        for (ptr = moves.begin(); ptr != moves.end(); ptr++) 
        {
            // if((ptr->x == 0 && ptr->y == 0) || (ptr->x == 7 && ptr->y == 0) || (ptr->x == 0 && ptr->y == 7) || (ptr->x == 7 && ptr->y == 7)){
            //     return *ptr;
            // }
            OthelloBoard alt = OthelloBoard(board);
            int evaluated = MiniMax(this->turn,this->turn,0,j,alt,ptr,MIN,MAX);
            if(evaluated == panda){
                // board.print();
                return *maxit;
            }
            if(evaluated > maxi){
                maxi = evaluated;
                maxit = ptr; 
            }
        } 
    }
    return *maxit;
}



int MyBot::MiniMax(Turn root, Turn current, int depth, int maxDepth, OthelloBoard& board, list<Move>::iterator ptr, int alpha, int beta){
    if(chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start).count() > 1997){
        return panda;
    }
    if(depth == maxDepth){
        // cout << Heur(board,ptr,current)<<"\n";
        return Heur(board,ptr,root,current);
    }
    if(root == current){
        int best = MAX;
        OthelloBoard alt = OthelloBoard(board);
        alt.makeMove(current,*ptr);
        list<Move> validMoves = alt.getValidMoves(other(current));
        list<Move>::iterator newptr;
        for (newptr = validMoves.begin(); newptr != validMoves.end(); newptr++) 
        {
            int val = MiniMax(root,other(current),depth+1,maxDepth,alt,newptr,alpha,beta);
            best = min(best, val); 
            beta = min(beta, best); 
            if (beta <= alpha) 
                break;
        }  
        return best;
    }
    else{
        int best = MIN;
        OthelloBoard alt = OthelloBoard(board);
        alt.makeMove(current,*ptr);
        // cout << "Hi";        
        list<Move> validMoves = alt.getValidMoves(other(current));
        list<Move>::iterator newptr;
        for (newptr = validMoves.begin(); newptr != validMoves.end(); newptr++) 
        {
            int val = MiniMax(root,other(current),depth+1,maxDepth,alt,newptr,alpha,beta);
            if(val == panda){
                return panda;
            }
            best = max(best, val); 
            alpha = max(alpha, best);  
            if (beta <= alpha) 
                break; 
        }  
        return best;
    }
}

int MyBot::Parity(OthelloBoard& board){
    int total = 64 - (board.getBlackCount() + board.getRedCount());
    if(total%2 == 0){
        return -1;
    }
    else{
        return 1;
    }
}

int MyBot::Corners(const OthelloBoard& board, Turn current){
    int fr = 0;
    int bk = 0;
    if(board.get(0,0) == current){
        fr+=1;
    }
    else if(board.get(0,0) == other(current)){
        bk+=1;
    }
    if(board.get(7,0) == current){
        fr+=1;
    }
    else if(board.get(7,0) == other(current)){
        bk+=1;
    }
    if(board.get(7,7) == current){
        fr+=1;
    }
    else if(board.get(7,7) == other(current)){
        bk+=1;
    }
    if(board.get(0,7) == current){
        fr+=1;
    }
    else if(board.get(0,7) == other(current)){
        bk+=1;
    }
    return 100*(fr-bk)/(fr+bk+1);
}

int MyBot::Mobility(const OthelloBoard& board, Turn current){
    int fr = (board.getValidMoves(current)).size();
    int bk = (board.getValidMoves(other(current))).size();
    // cout << 100*(fr-bk)/(fr+bk+1) << "\n";
    return 100*(fr-bk)/(fr+bk+1);
}


int MyBot::Heur(const OthelloBoard& board, list<Move>::iterator ptr, Turn root, Turn current){
    int finalValue = 0;
    OthelloBoard alt = OthelloBoard(board);
    alt.makeMove(current,*ptr);
    int totalCoins = alt.getBlackCount() + alt.getRedCount();
    finalValue += 10000*Corners(alt,root);
    finalValue += 10000*mahaStability(alt,root);
    if(totalCoins<=20){
        finalValue += 20*Positions(alt,root,ptr);
        // finalValue += 5*Mobility(alt,root);
    }
    else if(totalCoins<=56){
        finalValue += 3*CoinsD(alt,root);
        finalValue += 100*Parity(alt);
        // finalValue += 2*Mobility(board,current);
        finalValue += 10*Positions(alt,root,ptr);
    }
    else{
        finalValue += 500*CoinsD(alt,root);
        finalValue += 500*Parity(alt);   
    }
    // cout<<finalValue<<\n";
    return finalValue;
}

int MyBot::mahaStability(const OthelloBoard& board, Turn current){
    return (Stability(board,current)-Stability(board,other(current)));
}

int MyBot::Stability(const OthelloBoard& board, Turn current){
    unordered_map<int, int> umap;
    for (int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(board.get(i,j) == current){
                umap[10*i+j] = 1;
            }
            else{
                break;
            }
        }
    } 
    for (int i = 7; i > -1; i--){
        for(int j = 0; j < 8; j++){
            if(board.get(i,j) == current){
                umap[10*i+j] = 1;
            }
            else{
                break;
            }
        }
    }
    for (int i = 7; i > -1; i--){
        for(int j = 7; j > -1; j--){
            if(board.get(i,j) == current){
                umap[10*i+j] = 1;
            }
            else{
                break;
            }
        }
    }
    for (int i = 0; i < 8; i++){
        for(int j = 7; j > -1; j--){
            if(board.get(i,j) == current){
                umap[10*i+j] = 1;
            }
            else{
                break;
            }
        }
    }
    ////////
    for (int j = 0; j < 8; j++){
        for(int i = 0; i < 8; i++){
            if(board.get(i,j) == current){
                umap[10*i+j] = 1;
            }
            else{
                break;
            }
        }
    } 
    for (int j = 7; j > -1; j--){
        for(int i = 0; i < 8; i++){
            if(board.get(i,j) == current){
                umap[10*i+j] = 1;
            }
            else{
                break;
            }
        }
    }
    for (int j = 7; j > -1; j--){
        for(int i = 7; i > -1; i--){
            if(board.get(i,j) == current){
                umap[10*i+j] = 1;
            }
            else{
                break;
            }
        }
    }
    for (int j = 0; j < 8; j++){
        for(int i = 7; i > -1; i--){
            if(board.get(i,j) == current){
                umap[10*i+j] = 1;
            }
            else{
                break;
            }
        }
    }
    return umap.size();
}



int MyBot::CoinsD(OthelloBoard& board, Turn root){
    int rC,bC;
    if(root == 1){
        bC = board.getBlackCount();
        rC = board.getRedCount();
        return 100*((bC-rC)/(rC+bC)); 
    }
    else{
        bC = board.getBlackCount();
        rC = board.getRedCount();
        return 100*((rC-bC)/(rC+bC));
    }
}

//get strategic value of positions
int MyBot::Positions(const OthelloBoard& board, Turn current, list<Move>::iterator ptr){
    int fame = 0;
    int grid[8][8] = { 
        {20, -100, 11, 8, 8, 11, -100, 20},
        {-100, -200, -4, 1, 1, -4, -200, -100},
        {11, -4, 2, 2, 2, 2, -4, 11},
        {8, 1, 2, -3, -3, 2, 1, 8},
        {8, 1, 2, -3, -3, 2, 1, 8},
        {11, -4, 2, 2, 2, 2, -4, 11},
        {-100, -200, -4, 1, 1, -4, -200, -100},
        {20, -100, 11, 8, 8, 11, -100, 20}
    };

    // if(board.get(0,0) != 0){
    //     grid[0][1] = 0;
    //     grid[0][2] = 0;
    //     grid[0][3] = 0;
    //     grid[1][0] = 0;
    //     grid[1][1] = 0;
    //     grid[1][2] = 0;
    //     grid[2][0] = 0;
    //     grid[2][1] = 0;
    //     grid[3][0] = 0;
    // }

    // if(board.get(7,0) != 0){
    //     grid[7][1] = 0;
    //     grid[7][2] = 0;
    //     grid[7][3] = 0;
    //     grid[6][0] = 0;
    //     grid[6][1] = 0;
    //     grid[6][2] = 0;
    //     grid[5][0] = 0;
    //     grid[5][1] = 0;
    //     grid[4][0] = 0;
    // }

    // if(board.get(7,7) != 0){
    //     grid[7][6] = 0;
    //     grid[7][5] = 0;
    //     grid[7][4] = 0;
    //     grid[6][7] = 0;
    //     grid[6][6] = 0;
    //     grid[6][5] = 0;
    //     grid[5][7] = 0;
    //     grid[5][6] = 0;
    //     grid[4][7] = 0;
    // }

    // if(board.get(0,7) != 0){
    //     grid[0][6] = 0;
    //     grid[0][5] = 0;
    //     grid[0][4] = 0;
    //     grid[1][7] = 0;
    //     grid[1][6] = 0;
    //     grid[1][5] = 0;
    //     grid[2][7] = 0;
    //     grid[2][6] = 0;
    //     grid[3][7] = 0;
    // }


    for (int i=0;i<8;i++){
        for (int j=0;j<8;j++){
            if(board.get(i,j) == current){
                fame += grid[i][j];
            }
            else if(board.get(i,j) == other(current)){
                fame -= grid[i][j];
            }
        }
    }   
    return fame;
}

// The following lines are _very_ important to create a bot module for Desdemona

extern "C" {
    OthelloPlayer* createBot( Turn turn )
    {
        return new MyBot( turn );
    }

    void destroyBot( OthelloPlayer* bot )
    {
        delete bot;
    }
}



