
int prevmove=-1;

int xNextStep(const Game& G_){
	if(prevmove==-1) return prevmove=1;
	if(prevmove==1) return prevmove=3;
	if((prevmove==3)&&(G_.bPreviousWorked)) return prevmove=1;
	if((prevmove==3)&&(!G_.bPreviousWorked)) return prevmove=0;
	if((prevmove==0)&&(G_.bPreviousWorked)) return prevmove=1;
	if((prevmove==0)&&(!G_.bPreviousWorked)) return prevmove=2;
	if(prevmove==2) return prevmove=1;
	return prevmove=2;
}

int NextStep(const Game& G_){
	if(prevmove==-1) return prevmove=1;
	if(prevmove==1) return prevmove=3;
	if((prevmove==3)&&(G_.bPreviousWorked)) return prevmove=1;
	if((prevmove==3)&&(!G_.bPreviousWorked)) return prevmove=2;
	if((prevmove==0)&&(G_.bPreviousWorked)) return prevmove=2;
	if((prevmove==0)&&(!G_.bPreviousWorked)) return prevmove=1;
	if(prevmove==2) return prevmove=0;
	return prevmove=2;
}
