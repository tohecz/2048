
// Declare the structures
struct Game;
int NextStep(const Game& G_);

// Common includes
#include <iostream>     /* cin, cout, cerr */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <ctime>        /* clock */

// Do not modify these
#define TIME_LIMIT_CLOCK (TIME_LIMIT*CLOCKS_PER_SEC) // Time limit in clocks
#define NN (N*N) // Number of tiles

// The main object class
struct Game {
	int Board[NN]; // The board
	int nScore; // The current score
	int nOccupiedTiles; // Number of occupied tiles
	bool bPreviousWorked; // `true` if the last performed move actually did something
	
	// Generate a new 2/4 in the board
	bool NewTile(){
		int nEmptyTiles = 0;
		int i;
		// Count empty tiles
		for(i=0; i<NN; i++) if(!Board[i]) nEmptyTiles++;
		// Choose one of them, `k` is in {1..`nEmptytiles`}
		int k = rand()%nEmptyTiles+1;
		// `i` will store the index of `k`th empty tile
		for(i=0; i<NN; i++){
			if(Board[i]==0) k--;
			if(k==0) break;
		}
		// The new tile is 2 with probablity 90%, and 4 otherwise
		Board[i] = (rand()%10 ? 2 : 4);
		// We've just occupied another tile
		nOccupiedTiles++;
		return true;
	}
	
	// Returns true if there's a possible move
	bool CanPlay(){
		// An empty tile always guarantees a possible move
		if(nOccupiedTiles!=NN) return true;
		int i,j;
		// Two adjacent tiles with the same value always guarantee a possible move
		for(i=0; i<N; i++) for(j=0; j<N-1; j++){
			if(Board[i*N+j]==Board[i*N+(j+1)]) return true;
			if(Board[i+j*N]==Board[i+(j+1)*N]) return true;
		}
		// If both previous conditions fail, there's no possible move
		return false;
	}
	
	// Constructor
	Game(){
		// Initialization
		nOccupiedTiles=0;
		nScore=0;
		bPreviousWorked=true;
		for(int i=0; i<NN; i++) Board[i]=0;

		// There are two occupied tiles at the very beginning
		NewTile();
		NewTile();
	}
	
	// Method that lets the tiles fall down in the correct direction
	bool Gravity(int nDirection_){
		// The "bottom" w.r.t. the used gravity will be tiles `s1`, `s1+s2`, `s1+2*s2`, ...
		// Each "column" w.r.t. the used graity will be `s1+j*s2`, `s1+j*s2+s3`, `s1+j*s2+2*s3`, ...
		int s1,s2,s3;
		switch(nDirection_){
			case 0: s1=0; s2=1; s3=N; break; // UP
			case 1: s1=N-1; s2=N; s3=-1; break; // RIGHT
			case 2: s1=N*(N-1); s2=1; s3=-N; break; // DOWN
			case 3: s1=0; s2=N; s3=1; break; // LEFT
			default: return false;
		}
		int i,j,k;
		// We return whether anything moved
		bool ret=false;
		for(j=0; j<N; j++){
			for(i=1; i<N; i++){
				// Condition: the current tile is non-empty and the "one down" is empty
				if(( Board[s1+j*s2+i*s3]!=0 )&&( Board[s1+j*s2+(i-1)*s3]==0 )){
					k=i;
					// See how far we can "fall"
					while(( k>0 )&&( Board[s1+j*s2+(k-1)*s3]==0 )) k--;
					// Move the tile
					Board[s1+j*s2+k*s3]=Board[s1+j*s2+i*s3];
					Board[s1+j*s2+i*s3]=0;
					// We did some move
					ret=true;
				}
			}
		}
		return ret;
	}
	
	// Method that merges adjacent tiles in the correct direction
	bool Merge(int nDirection_){
		// Same as in method `Gravity`
		int s1,s2,s3;
		switch(nDirection_){
			case 0: s1=0; s2=1; s3=N; break;
			case 1: s1=N-1; s2=N; s3=-1; break;
			case 2: s1=N*(N-1); s2=1; s3=-N; break;
			case 3: s1=0; s2=N; s3=1; break;
			default: return false;
		}
		int i,j,k;
		// We return whether anything moved
		bool ret=false;
		for(j=0; j<N; j++){
			for(i=1; i<N; i++){
				// Condition: the two tiles have the same non-zero value
				if(( Board[s1+j*s2+i*s3]!=0 )&&( Board[s1+j*s2+i*s3]==Board[s1+j*s2+(i-1)*s3] )){
					// Double the value of the "bottom" tile
					Board[s1+j*s2+(i-1)*s3]*=2;
					// Add the new value to the score
					nScore+=Board[s1+j*s2+(i-1)*s3];
					// Empty the "top" tile
					Board[s1+j*s2+i*s3]=0;
					// We removed an occupied tile
					nOccupiedTiles--;
					// We did some merge
					ret=true;
				}
			}
		}
		return ret;
	}

	// Perform a step
	void Step(int nDirection_){
		// We check whether we did anything, better separately to avoid compiler optimizations
		bool b1, b2, b3;
		// The step of the game can be performed as Gravity+Merge+Gravity
		b1=Gravity(nDirection_);
		b2=Merge(nDirection_);
		b3=Gravity(nDirection_);
		// Store the information whether something changed
		bPreviousWorked = b1||b2||b3;
		// If the step did something, add another new tile
		if(bPreviousWorked) NewTile();
	}

	// The main "infinite loop"
	void Play(){
		// Store the limit clock value
		std::clock_t EndClock=std::clock()+TIME_LIMIT_CLOCK;
		// As long as a move is possible
		while(CanPlay()){
			// If requested, print the board
			#if(PRINT>1)
			Print(std::cerr);
			#endif
			// Do the step; the direction is determined by an external
			//  function NextStep, which takes the board as an argument
			Step(NextStep(*this));
			// If we are over the score or time limit, set the maximal score and quit
			if(( nScore>SCORE_LIMIT )||( std::clock()>EndClock )){
				nScore=SCORE_LIMIT;
				break;
			}
		}
		// If requested, print the board
		#if(PRINT>0)
		Print(std::cerr);
		#endif
	}
	
	// Printing function, it's all a mess
	void Print(std::ostream& os_){
		int i,j;
		for(j=0; j<N; j++) os_ << "+--------";
		os_ << "+" << std::endl;;
		for(i=0; i<N; i++){
			for(j=0; j<N; j++) os_ << "|        ";
			os_ << "|" << std::endl;;
			for(j=0; j<N; j++) if(Board[i*N+j]!=0){
				os_ << "| ";
				os_.width(6);
				os_ << Board[i*N+j] << " ";
			} else os_ << "|        ";
			os_ << "|" << std::endl;;
			for(j=0; j<N; j++) os_ << "|        ";
			os_ << "|" << std::endl;;
			for(j=0; j<N; j++) os_ << "+--------";
			os_ << "+" << std::endl;;
		}
		os_ << " Last step worked: " << (bPreviousWorked?"Y":"n") << std::endl;
		os_ << " Occupied tiles: " << nOccupiedTiles << std::endl;
		os_ << " Score: " << nScore << std::endl;
	}

};

int main(){
	int i;
	long int nScore=0;
	// Initialized the random number generator
	srand(time(NULL));
	// We store the clock value
	std::clock_t StartClock=std::clock();
	for(i=0; i<NUM_GAMES; i++){
		// Initialize the game
		Game G;
		// Let the game run
		G.Play();
		// Add the score
		nScore+=G.nScore;
	}
	// Print the resulting values
	std::cerr << "Number of games: " << NUM_GAMES << std::endl;
	std::cerr << "Total score: " << nScore << std::endl;
	std::cerr << "Average score: " << float(nScore)/NUM_GAMES << std::endl;
	std::cerr << "Execution time: " << float(clock()-StartClock)/CLOCKS_PER_SEC << std::endl;
	std::cerr << "Average time: " << float(clock()-StartClock)/CLOCKS_PER_SEC/NUM_GAMES << std::endl;
	return 0;
}
