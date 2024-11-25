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
#include <chrono>
#include <functional>
#include <vector>
#include <algorithm>
using namespace std;
using namespace Desdemona;

class MyBot: public OthelloPlayer
{
    public:
        /**
         * Initialisation routines here
         * This could do anything from open up a cache of "best moves" to
         * spawning a background processing thread. 
         */
        int weights[8][8]= {{4,-3,2,2,2,2,-3,4},
                            {-3,-4,-1,-1,-1,-1,-4,-3},
                            {2,-1,1,0,0,1,-1,2},
                            {2,-1,0,1,1,0,-1,2},
                            {2,-1,0,1,1,0,-1,2},
                            {2,-1,1,0,0,1,-1,2},
                            {-3,-4,-1,-1,-1,-1,-4,-3},
                            {4,-3,2,2,2,2,-3,4}
                           };
        MyBot( Turn turn );
        int cornerHeuristic(const OthelloBoard& board);
        pair<int,bool> mobilityHeuristic(const OthelloBoard& board);
        int potentialMobilityHeuristic(const OthelloBoard& board);
        int stabilityHeuristic(const OthelloBoard& board);
        int parityHeuristic(const OthelloBoard& board);
        int evaluateBoard(const OthelloBoard&);
        int endgameEvaluateBoard(const OthelloBoard& board);
        bool compare2(const pair<OthelloBoard,Move>& board1 , const pair<OthelloBoard,Move>& board2);
        bool compare(const OthelloBoard& board1 , const OthelloBoard& board2);
        bool compareRev(const OthelloBoard& board1 , const OthelloBoard& board2);
        Move game(const OthelloBoard& board, chrono::high_resolution_clock::time_point start,int depth, bool endgame);
        int strong_alpha_beta(const OthelloBoard& board,int depth,int alpha, int beta, int min_value,int max_value, function<int(const OthelloBoard&)> herustic, bool MaxPlayer,bool passed,chrono::high_resolution_clock::time_point start);

        /**
         * Play something 
         */
        virtual Move play( const OthelloBoard& board );
    private:
};

MyBot::MyBot( Turn turn )
    : OthelloPlayer( turn )
{
}

int MyBot::stabilityHeuristic(const OthelloBoard& board)
{
    int myscore=0;
    int oppscore=0;

    for(int i=0;i<8;i++)
    {
        for(int j=0;j<8;j++)
        {
            if(board.get(i,j)!=EMPTY)
            {
                board.get(i,j)==turn ? myscore+=weights[i][j]: oppscore+=weights[i][j]; 
            }
        }
    }

    if(myscore+oppscore!=0)
    {
        return myscore-oppscore;
    }

    return 0;

}
int MyBot::parityHeuristic(const OthelloBoard& board)
{
    int rcount=board.getRedCount();
    int bcount=board.getBlackCount();

    return turn==RED? (100*(rcount-bcount))/(rcount+bcount) : (100*(bcount-rcount))/(rcount+bcount);
}

int MyBot::potentialMobilityHeuristic(const OthelloBoard& board)
{
    int mycount=0;
    int oppcount=0;
    for(int i=0;i<8;i++)
    {
        for(int j=0;j<8;j++)
        {   
            bool my=false;
            bool opp=false;
            if(board.get(i,j)==EMPTY)
            {

                if(i-1>=0)
                {
                    if(j-1>=0)
                        if(board.get(i-1,j-1)!=EMPTY)
                            board.get(i-1,j-1)==turn? opp=true: my=true;
                    
                    if(j+1<=7)
                        if(board.get(i-1,j+1)!=EMPTY)
                            board.get(i-1,j+1)==turn? opp=true: my=true;

                    if(board.get(i-1,j)!=EMPTY)
                        board.get(i-1,j)==turn? opp=true: my=true;        
                }
                if(i+1<=7)
                {
                    if(j-1>=0)
                        if(board.get(i+1,j-1)!=EMPTY)
                            board.get(i+1,j-1)==turn? opp=true: my=true;
                    
                    if(j+1<=7)
                        if(board.get(i+1,j+1)!=EMPTY)
                            board.get(i+1,j+1)==turn? opp=true: my=true;

                    if(board.get(i+1,j)!=EMPTY)
                        board.get(i+1,j)==turn? opp=true: my=true;
                }
                if(j-1>=0)
                    if(board.get(i,j-1)!=EMPTY)
                        board.get(i,j-1)==turn? opp=true: my=true;
                
                if(j+1<=7)
                    if(board.get(i,j+1)!=EMPTY)
                        board.get(i,j+1)==turn? opp=true: my=true;
            }    
            if(my)
                mycount++;
            if(opp)
                oppcount++;    
        }
    }
    if(mycount+oppcount!=0)
    {
        return (100*(mycount-oppcount))/(mycount+oppcount);
    }

    return 0;
}


pair<int,bool> MyBot::mobilityHeuristic(const OthelloBoard& board)
{
    int mycount=0;
    int oppcount=0;
    for(int i=0;i<8;i++)
    {
        for(int j=0;j<8;j++)
        {
            if(board.validateMove(turn,i,j))
                mycount++;

            if(board.validateMove(other(turn),i,j))
                oppcount++;    
        }
    }
    if(mycount+oppcount!=0)
    {
        return make_pair((100*(mycount-oppcount))/(mycount+oppcount),false);
    }

    return make_pair(0,mycount==0);
}

int MyBot::cornerHeuristic(const OthelloBoard& board)
{
    int mycount=0;
    int oppcount=0;

    Coin c=board.get(0,0);
    if(c!=EMPTY) 
        c==turn ? mycount++ : oppcount++;

    c=board.get(0,7);
    if(c!=EMPTY) 
        c==turn ? mycount++ : oppcount++;

    c=board.get(7,0);
    if(c!=EMPTY) 
        c==turn ? mycount++ : oppcount++;

    c=board.get(7,7);
    if(c!=EMPTY) 
        c==turn ? mycount++ : oppcount++;

    if(mycount+oppcount!=0)
    {
        return (100*(mycount-oppcount))/(mycount+oppcount);
    }

    return 0;            
}

int MyBot::endgameEvaluateBoard(const OthelloBoard& board)
{
    int rcount=board.getRedCount();
    int bcount=board.getBlackCount();

    if(rcount>bcount)
        return turn==RED ? 64+rcount : -64+bcount;
    else if(rcount<bcount)    
        return turn==RED ? -64+rcount : 64+bcount;
    else 
        return turn==RED ? rcount : bcount;    
}

bool MyBot::compare2(const pair<OthelloBoard,Move>& board1 , const pair<OthelloBoard,Move>& board2)
{
    return evaluateBoard(board1.first) > evaluateBoard(board2.first);
}
bool MyBot::compare(const OthelloBoard& board1 , const OthelloBoard& board2)
{
    return evaluateBoard(board1) > evaluateBoard(board2);
}
bool MyBot::compareRev(const OthelloBoard& board1 , const OthelloBoard& board2)
{
    return evaluateBoard(board1) < evaluateBoard(board2);
}

int MyBot::evaluateBoard(const OthelloBoard& board)
{
    int rcount=board.getRedCount();
    int bcount=board.getBlackCount();

    pair<int,bool> mobility=mobilityHeuristic(board);
    if(mobility.second==true)   //Game is over
    {
        if(rcount > bcount)
            return turn==RED? 10000: -10000 ;
        else if(rcount < bcount)
            return turn==RED? -10000: 10000 ;
        else 
            return 0;        
    }
    else
    {
        if(rcount+bcount<=20)
        {
            int corners = cornerHeuristic(board);
            int potentialMobility = potentialMobilityHeuristic(board);
            int stability = stabilityHeuristic(board);
            
            return 50*corners + 10 * mobility.first + 10*potentialMobility + 30 * stability;  
        }
        else if(rcount+bcount<=56)
        {
            int corners = cornerHeuristic(board);
            int potentialMobility = potentialMobilityHeuristic(board);
            int stability = stabilityHeuristic(board);
            
            return 60*corners + 30 * stability + 5 * mobility.first + 5 *potentialMobility;
             
        }
        else
        {
            int corners = cornerHeuristic(board);
            int parity = parityHeuristic(board);
            return 50*corners + 50 * parity; 
        }
    }    
}

Move MyBot::play( const OthelloBoard& board )
{
    auto start=chrono::high_resolution_clock::now();
    list<Move> moves = board.getValidMoves( turn );
    int rcount = board.getRedCount();
    int bcount = board.getBlackCount();
    //cout<<"move no "<<rcount+bcount<<endl;
    if(moves.size()==0)
    {
        return Move::pass();
    }
    else
    {   
        Move best_move=*moves.begin();
        int depth =1;
        for(depth=1;depth<=64 - (rcount + bcount);depth++)
        {	
            //cout<<" "<<depth<<endl;
            Move move = game(board,start,depth,false);
            auto stop=chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
            if(duration.count()>1950)
            {
            	//cout<<" before "<<depth<<endl;
                return best_move;
            }
            else 
                best_move = move;
        }
        
        Move move=game(board,start,64 - (rcount + bcount),true);
        auto stop=chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
        if(duration.count()>1950)
            {//cout<<" between "<<depth<<endl;
            return best_move;}
	//cout<<" after "<<depth<<endl;
        return move;    
    }   
}

Move MyBot::game(const OthelloBoard& board,chrono::high_resolution_clock::time_point start,int depth, bool endgame)
{
    //cout<<endl<<" game start "<<depth<<endl;
    list<Move> moves = board.getValidMoves( turn );
    
    list<Move>::iterator it=moves.begin();
    vector<pair<OthelloBoard,Move>> boards;
    for(int i=0;i<(int)moves.size(); it++, i++)
    {
    	OthelloBoard board_copy=board;
        board_copy.makeMove(turn,(*it));
        boards.push_back(make_pair(board_copy,(*it)));
    }
    
    
    sort(boards.begin(),boards.end(),bind(&MyBot::compare2,this,placeholders::_1,placeholders::_2));

    int value= endgame ? -64 : -10000;
    
    
    int min_value = endgame ? -64 : -10000;
    int max_value = endgame ? +128 : +10000;
    int alpha = min_value;
    int beta = max_value;
    function<int(const OthelloBoard&)> evaluator = endgame ? bind(&MyBot::endgameEvaluateBoard,this,placeholders::_1):bind(&MyBot::evaluateBoard,this,placeholders::_1);

    Move best_move=boards[0].second;
    for(int i=0;i<(int)boards.size();i++)
    {
        int new_value=strong_alpha_beta(boards[i].first,depth-1,alpha,beta,min_value,max_value,evaluator, false,false,start);
        if(new_value>value)
        {
            value = new_value;
            best_move=boards[i].second;
        }    
        if(value >=beta)
            break;
        alpha=max(alpha,value);
        auto stop=chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
        if(duration.count()>1950)
        {
    	   return best_move;
        }    
    }
    //cout<<value<<endl;
    return best_move;
}
    


int MyBot::strong_alpha_beta(const OthelloBoard& board,int depth,int alpha, int beta, int min_value,int max_value, function<int(const OthelloBoard&)> heuristic, bool MaxPlayer,bool passed,chrono::high_resolution_clock::time_point start)
{
    //cout<<" alpha-beta start "<<depth<<endl;
    if(depth == 0)
    {
        return heuristic(board);
    }

    if(MaxPlayer)
    {
        list<Move> moves = board.getValidMoves( turn );
        
        if(moves.size()==0)
        {   
            if(passed)
            {
                return heuristic(board);
            }

            return strong_alpha_beta(board,depth-1,alpha,beta,min_value,max_value,heuristic,!MaxPlayer,true,start);
        }
        else 
        {
            int value=min_value;
            list<Move>::iterator it=moves.begin();
    	    vector<OthelloBoard> boards;
    	    for(int i=0;i<(int)moves.size();it++, i++)
    	    {
    		OthelloBoard board_copy=board;
        	board_copy.makeMove(turn,(*it));
        	boards.push_back(board_copy);
    	    }
    	    sort(boards.begin(),boards.end(),bind(&MyBot::compare,this,placeholders::_1,placeholders::_2));
            for(int i=0;i<(int)boards.size(); i++)
            {
                value = max(value, strong_alpha_beta(boards[i],depth-1,alpha,beta,min_value,max_value,heuristic,!MaxPlayer,false,start));
                if(value >=beta)
                    break;
                alpha=max(alpha,value);
                auto stop=chrono::high_resolution_clock::now();
        	    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
        	    if(duration.count()>1950)
        	    {
    	   	        return max_value;        
        	    }        
            }
            return value;
        }
    }
    else {
        list<Move> moves = board.getValidMoves( other(turn) );
        
        if(moves.size()==0)
        {   
            if(passed)
            {
                return heuristic(board);
            }
               
            return strong_alpha_beta(board,depth-1,alpha,beta,min_value,max_value,heuristic,!MaxPlayer,true,start);
        }
        else
        {
            int value=max_value;
            list<Move>::iterator it=moves.begin();
    	    vector<OthelloBoard> boards;
    	    for(int i=0;i<(int)moves.size(); it++, i++)
    	    {
    		OthelloBoard board_copy=board;
        	board_copy.makeMove(other(turn),(*it));
        	boards.push_back(board_copy);
    	    }
    	    sort(boards.begin(),boards.end(),bind(&MyBot::compareRev,this,placeholders::_1,placeholders::_2));
            for(int i=0;i<(int)boards.size(); i++)
            {
                value = min(value, strong_alpha_beta(boards[i],depth-1,alpha,beta,min_value,max_value,heuristic,!MaxPlayer,false,start));
                if(value <=alpha)
                    break;
                beta=min(beta,value);
                auto stop=chrono::high_resolution_clock::now();
        	    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
        	    if(duration.count()>1950)
        	    {
    	   	        return min_value;        
        	    }        
            }
            return value;
        }
    }
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


