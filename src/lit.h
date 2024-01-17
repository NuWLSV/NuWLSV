#ifndef LIT_H 
#define LIT_H

class lit
{
    public:
    int clause_num; //clause num, begin with 0
	int var_num;	//variable num, begin with 1
	bool sense;		//is 1 for true literals, 0 for false literals.
};

#endif