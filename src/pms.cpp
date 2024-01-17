#include "basis_pms.h"
#include "build.h"
#include "pms.h"
#include "heuristic.h"
#include <signal.h>

ISDist s;
int seed = 1;
int sample_num = 20;
int threshold = 10;
int max_num_global_opt_solns = 0;
long long best_known;
long long total_step = 0;
long long consecutive_better_soft = 0;
char * file_name = NULL;
void interrupt(int sig)
{
	// if (s.verify_sol() == 1)
	// 	cout << "c verified" << endl;
	// s.print_best_solution();
	// s.simple_print();
	if(s.global_sol_feasible)
		cout << s.opt_unsat_weight << '\t' << s.opt_time << endl;
	s.free_memory();
	exit(10);
}

int main(int argc, char *argv[])
{
	start_timing();

	signal(SIGTERM, interrupt);

	sscanf(argv[2], "%d", &seed);
	srand(seed);
	s.build_instance(argv[1]);

	s.settings();
	s.parse_parameters2(argc, argv);
	
	
	if(argc > 3){
		sscanf(argv[3], "%d", &sample_num);
		sscanf(argv[4], "%d", &threshold);
		sscanf(argv[5], "%d", &max_num_global_opt_solns);
		s.local_search_with_decimation(argv[1], sample_num, threshold, max_num_global_opt_solns);
	}
	else{
		s.local_search_with_decimation(argv[1]);
	}
	

	//s.simple_print();

	// s.free_memory();

	return 0;
}
