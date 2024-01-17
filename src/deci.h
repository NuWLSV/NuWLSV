#ifndef _DECI_H_
#define _DECI_H_

#include "lit.h"
#define USING_VOTING_IN_UNIT_PROCESS true
using namespace std;

class Decimation
{
  public:
    Decimation(lit **ls_var_lit, int *ls_var_lit_count, lit **ls_clause_lit, long long *ls_org_clause_weight, long long ls_top_clause_weight);
    Decimation(lit **ls_var_lit, int *ls_var_lit_count, lit **ls_clause_lit, long long *ls_org_clause_weight, long long ls_top_clause_weight, int sample_num, int threshold, int size);

    void make_space(int max_c, int max_v);
    void free_memory();
    void init(int *ls_local_opt, int *ls_global_opt, lit *ls_unit_clause, int ls_unit_clause_count, int *ls_clause_lit_count);
    void push_unit_clause_to_queue(lit tem_l);
    void assign(int v, int sense);
    void remove_unassigned_var(int v);
    void hunit_propagation();
    void sunit_propagation();
    void random_propagation();
    void unit_prosess();
    bool choose_sense(int v);
    void push_global_opt_soln(int* cur_vars, int unsat_weight);
    void replace_global_opt(int* soln, int unsat_weight);
    int  voting_by_local_opt(int v, int weight = 1);
    int  index_similar(int* cur_vars); // return the index of solution that is similarest and beyond the threshold at the same time

    // helper functions
    void array_copy(int* arr1, int* arr2, int len);


    vector<int> fix;

    int num_vars;
    int num_clauses;

    long long *h_true_score;
    long long *h_false_score;
    long long *hscore;
    long long *s_true_score;
    long long *s_false_score;
    long long *sscore;

    lit **clause_lit;
    lit **var_lit;
    int *var_lit_count;

    int *local_opt;
    int *global_opt;
    long long *org_clause_weight;
    long long top_clause_weight;

    lit *hunit_clause_queue;
    int *sense_in_hunit_clause_queue;
    int hunit_beg_pointer;
    int hunit_end_pointer;

    lit *sunit_clause_queue;
    int *sense_in_sunit_clause_queue;
    int sunit_beg_pointer;
    int sunit_end_pointer;

    int *unassigned_var;
    int *index_in_unassigned_var;
    int unassigned_var_count;

    int *clause_delete;
    int *clause_lit_count;

    int ** global_var_val_cnt;
    int ** global_opt_solns;
    long long *  global_opt_unsat_weight;
    int *  local_opt_time_stamps; // useless here
    int local_opt_last_time_stamp; // useless here
    int num_global_opt_solns;
    int max_num_global_opt_solns;
    int worst_global_soln_index;
    int replace_possibility;
    int mode_possibility;

    int  sample_num;
    int  push_threshold;
    int* similar_cnt;
};

Decimation::Decimation(lit **ls_var_lit, int *ls_var_lit_count, lit **ls_clause_lit, long long *ls_org_clause_weight, long long ls_top_clause_weight)
{
    var_lit = ls_var_lit;
    var_lit_count = ls_var_lit_count;
    clause_lit = ls_clause_lit;
    org_clause_weight = ls_org_clause_weight;
    top_clause_weight = ls_top_clause_weight;
    
    mode_possibility = 30;
    max_num_global_opt_solns = 2;
    
    num_global_opt_solns = 0;
    worst_global_soln_index = 0;
}

Decimation::Decimation(lit **ls_var_lit, int *ls_var_lit_count, lit **ls_clause_lit, long long *ls_org_clause_weight, long long ls_top_clause_weight, int sample_num, int threshold, int size)
{
    var_lit = ls_var_lit;
    var_lit_count = ls_var_lit_count;
    clause_lit = ls_clause_lit;
    org_clause_weight = ls_org_clause_weight;
    top_clause_weight = ls_top_clause_weight;

    // mode_possibility = mode_p;
    sample_num = sample_num;
    push_threshold = threshold;
    max_num_global_opt_solns = size;
    
    num_global_opt_solns = 0;
    // worst_global_soln_index = 0;

}

void Decimation::make_space(int max_c, int max_v)
{
    int i;
    num_vars = max_v;
    num_clauses = max_c;

    max_c += 10;
    max_v += 10;

    h_true_score = new long long[max_v];
    h_false_score = new long long[max_v];
    hscore = new long long[max_v];
    s_true_score = new long long[max_v];
    s_false_score = new long long[max_v];
    sscore = new long long[max_v];

    fix.resize(max_v);
    fix.reserve(max_v);

    hunit_clause_queue = new lit[max_v];
    sense_in_hunit_clause_queue = new int[max_v];

    sunit_clause_queue = new lit[max_v];
    sense_in_sunit_clause_queue = new int[max_v];

    unassigned_var = new int[max_v];
    index_in_unassigned_var = new int[max_v];

    clause_delete = new int[max_c];
    clause_lit_count = new int[max_c];

    global_var_val_cnt = new int* [max_v];
    for(i = 0;i < max_v;i++)
    {
        global_var_val_cnt[i] = new int[2];
    }
    global_opt_solns = new int* [max_num_global_opt_solns];
    for(i = 0;i < max_num_global_opt_solns;i++)
    {
        global_opt_solns[i] = new int[max_v];
    }
    global_opt_unsat_weight = new long long[max_num_global_opt_solns];
    similar_cnt = new int[max_num_global_opt_solns];
    // local_opt_time_stamps  = new int[max_num_global_opt_solns];
}

void Decimation::free_memory()
{
    int i;
    fix.clear();
    delete[] h_true_score;
    delete[] h_false_score;
    delete[] hscore;
    delete[] s_true_score;
    delete[] s_false_score;
    delete[] sscore;

    delete[] hunit_clause_queue;
    delete[] sense_in_hunit_clause_queue;

    delete[] sunit_clause_queue;
    delete[] sense_in_sunit_clause_queue;

    delete[] unassigned_var;
    delete[] index_in_unassigned_var;

    delete[] clause_delete;
    delete[] clause_lit_count;

    for(i = 0;i < max_num_global_opt_solns;i++)
    {
        delete global_opt_solns[i];
    }
    delete[] global_opt_solns;
    // delete[] local_opt_time_stamps;
    delete[] global_opt_unsat_weight;
    delete[] similar_cnt;
}

void Decimation::init(int *ls_local_opt, int *ls_global_opt, lit *ls_unit_clause, int ls_unit_clause_count, int *ls_clause_lit_count)
{
    int v;
    int c;
    //hyperparameters
    
    replace_possibility = 1;
    

    //parameters used in decimation
    

    hunit_beg_pointer = 0;
    hunit_end_pointer = 0;

    sunit_beg_pointer = 0;
    sunit_end_pointer = 0;

    unassigned_var_count = num_vars;

    local_opt_last_time_stamp = 0;

    //data structure of the instance
    local_opt = ls_local_opt;
    global_opt = ls_global_opt;

    for (int i = 0; i < num_vars; ++i)
    {
        v = i + 1;
        unassigned_var[i] = v;
        index_in_unassigned_var[v] = i;

        fix[v] = -1;
        sense_in_hunit_clause_queue[v] = -1; // not in hunit queue
        sense_in_sunit_clause_queue[v] = -1; // not in sunit queue

        global_var_val_cnt[i][0] = 0;
        global_var_val_cnt[i][1] = 0;
    }

    for (int i = 0; i < num_clauses; ++i)
    {
        clause_lit_count[i] = ls_clause_lit_count[i];
        clause_delete[i] = 0;
    }

    for (int i = 0; i < ls_unit_clause_count; ++i)
    {
        push_unit_clause_to_queue(ls_unit_clause[i]);
    }

    //figout score
    for (v = 1; v <= num_vars; ++v)
    {
        h_false_score[v] = 0;
        h_true_score[v] = 0;
        s_false_score[v] = 0;
        s_true_score[v] = 0;
        for (int i = 0; i < var_lit_count[v]; ++i)
        {
            c = var_lit[v][i].clause_num;
            if (org_clause_weight[c] == top_clause_weight)
            {
                if (var_lit[v][i].sense == 1)
                    ++h_true_score[v];
                else
                    ++h_false_score[v];
            }
            else
            {
                if (var_lit[v][i].sense == 1)
                    s_true_score[v] += org_clause_weight[c];
                else
                    s_false_score[v] += org_clause_weight[c];
            }
        }
        hscore[v] = max(h_false_score[v], h_true_score[v]);
        sscore[v] = max(s_false_score[v], s_true_score[v]);
    }
}

void Decimation::push_global_opt_soln(int* cur_vars, int unsat_weight){
    if(num_global_opt_solns < max_num_global_opt_solns){ // the population not reaches its limit
        array_copy(global_opt_solns[num_global_opt_solns], cur_vars, num_vars);
        global_opt_unsat_weight[num_global_opt_solns] = unsat_weight;
        // local_opt_time_stamps[num_global_opt_solns] = local_opt_last_time_stamp;
        // local_opt_last_time_stamp ++;
        num_global_opt_solns += 1;
                    
    }
    else{ // global_opt_solns is full
        // int index_for_simliar = index_similar(cur_vars);
        // if(index_for_simliar != -1){
        //     array_copy(global_opt_solns[index_for_simliar], cur_vars, num_vars);
        //     global_opt_unsat_weight[index_for_simliar] = unsat_weight;
        // }
        // else{
            replace_global_opt(cur_vars, unsat_weight);
        // }
    }
}

void Decimation::replace_global_opt(int* cur_vars, int unsat_weight){
    int i, v;
    int worst_index = 0, worst_unsat_weight = global_opt_unsat_weight[0];
    for(i = 1;i < num_global_opt_solns;i++){
        if(global_opt_unsat_weight[i] > worst_unsat_weight){
            worst_unsat_weight = global_opt_unsat_weight[i];
            worst_index = i;
        }
    }
    for(i = 1;i <= num_vars;i++){
        global_opt_solns[worst_index][i] = cur_vars[i];
    }
    global_opt_unsat_weight[worst_index] = unsat_weight;
}

int  Decimation::index_similar(int* cur_vars){
    int i, j, var, index = -1;
    if(num_global_opt_solns > 0){    
        for(j = 0;j < num_global_opt_solns;j++){
            similar_cnt[j] = 0;
            for(i = 0;i < sample_num;i++){
                var = rand() % num_vars + 1;
                if(cur_vars[var] == global_opt_solns[j][var]){
                    similar_cnt[j] += 1;
                }
            }
        }
        int max_similar = -1;
        for(j = 0;j < num_global_opt_solns;j++){
            if(similar_cnt[j] > push_threshold && similar_cnt[j] > max_similar){
                max_similar = similar_cnt[j];
                index = j;
            }
        }
    }
    return index;
}

int  Decimation::voting_by_local_opt(int v, int voting_weight)
{
    int cnt_0 = 0;
    int cnt_1 = 0;
    
    int weight = voting_weight;
    for(int i = 0;i < num_global_opt_solns;i++)
    {
        if(global_opt_solns[(i + worst_global_soln_index) % max_num_global_opt_solns][v] == 0)
        {
            cnt_0 += weight;
            // cnt_0 ++;
        }
        else{
            cnt_1 += weight;
            // cnt_1 ++;
        }       
        weight++;
    }

    if(cnt_0 == 0 && cnt_1 == 0){
        return rand() % 2;
    }
    else if(cnt_0 == 0)
    {
        return 1;
    }
    else if(cnt_1 == 0)
    {
        return 0;
    }
    else{
        int r_num = rand() % (cnt_0 + cnt_1);
        if(r_num < cnt_0){
            return 0;
        }
        else{
            return 1;
        }
    }
}

void Decimation::push_unit_clause_to_queue(lit tem_l)
{
    int v = tem_l.var_num;
    int c = tem_l.clause_num;
    if (org_clause_weight[c] == top_clause_weight)
    {
        if (sense_in_hunit_clause_queue[v] == -1)
        {
            sense_in_hunit_clause_queue[v] = tem_l.sense;
            hunit_clause_queue[hunit_end_pointer++] = tem_l;
        }
        else
        {
            if (sense_in_hunit_clause_queue[v] != tem_l.sense) //conflict var in hard unit queue
            {
                sense_in_hunit_clause_queue[v] = -2; //means this variable is conflict in hard unit queue
            }
        }
    }
    else
    {
        if (sense_in_hunit_clause_queue[v] != -1)
            return; //be defined by hard unit queue

        if (sense_in_sunit_clause_queue[v] == -1)
        {
            sense_in_sunit_clause_queue[v] = tem_l.sense;
            sunit_clause_queue[sunit_end_pointer++] = tem_l;
        }
        else
        {
            if (sense_in_sunit_clause_queue[v] != tem_l.sense) //conflict var in soft unit queue
            {
                sense_in_sunit_clause_queue[v] = -3; //means this variable is conflict in soft unit queue
            }
        }
    }
}

void Decimation::remove_unassigned_var(int v)
{
    int index = index_in_unassigned_var[v];
    int last_var = unassigned_var[--unassigned_var_count];
    unassigned_var[index] = last_var;
    index_in_unassigned_var[last_var] = index;
}

void Decimation::assign(int v, int sense)
{
    int c, l;
    lit tem_lit;
    fix[v] = sense;
    remove_unassigned_var(v);

    for (int i = 0; i < var_lit_count[v]; ++i)
    {
        c = var_lit[v][i].clause_num;
        if (clause_delete[c] == 1)
            continue;

        if (sense == var_lit[v][i].sense)
        {
            clause_delete[c] = 1;
            if (org_clause_weight[c] == top_clause_weight)
            {
                for (int j = 0; j < clause_lit_count[c]; j++)
                {
                    tem_lit = clause_lit[c][j];
                    if (tem_lit.sense == 1)
                    {
                        h_true_score[tem_lit.var_num]--;
                    }
                    else
                        h_false_score[tem_lit.var_num]--;
                    hscore[tem_lit.var_num] = max(h_true_score[tem_lit.var_num], h_false_score[tem_lit.var_num]);
                }
            }
            else
            {
                for (int j = 0; j < clause_lit_count[c]; j++)
                {
                    tem_lit = clause_lit[c][j];
                    if (tem_lit.sense == 1)
                    {
                        s_true_score[tem_lit.var_num] -= org_clause_weight[c];
                    }
                    else
                        s_false_score[tem_lit.var_num] -= org_clause_weight[c];
                    sscore[tem_lit.var_num] = max(s_true_score[tem_lit.var_num], s_false_score[tem_lit.var_num]);
                }
            }
            continue;
        }

        for (int j = 0; j < clause_lit_count[c]; j++)
        {
            if (clause_lit[c][j].var_num == v)
            {
                swap(clause_lit[c][j], clause_lit[c][--clause_lit_count[c]]);
                break;
            }
        }
        if (clause_lit_count[c] == 1)
        {
            push_unit_clause_to_queue(clause_lit[c][0]);
        }
    }
}

bool Decimation::choose_sense(int v)
{
    return rand() % 2;
}

void Decimation::hunit_propagation()
{
    int v, c, sense, rd;

    v = hunit_clause_queue[hunit_beg_pointer].var_num;
    sense = hunit_clause_queue[hunit_beg_pointer].sense; // literal
    hunit_beg_pointer++;

    if (sense_in_hunit_clause_queue[v] == -2) // conflict in hard clause
    {
        if (sscore[v] > 0)
        {
            if (sscore[v] == s_true_score[v]) 
                sense = 1;
            else
                sense = 0;
        }
        else
        {
            sense = choose_sense(v);
        }
    }
    assign(v, sense);
}

void Decimation::sunit_propagation()
{
    int v, c, sense, rd;

    int ht;
    ht = 15;

    int best_v = sunit_clause_queue[sunit_beg_pointer].var_num;
    int best_score = sscore[best_v];
    int index = sunit_beg_pointer;
    int count = sunit_end_pointer - sunit_beg_pointer;
    if (count > 15)
    {
        for (int i = 0; i < 15; ++i)
        {
            rd = rand() % count;

            v = sunit_clause_queue[sunit_beg_pointer + rd].var_num;
            if (sscore[v] > best_score)
            {
                best_v = v;
                index = sunit_beg_pointer + rd;
            }
        }
    }
    else
    {
        for (int i = sunit_beg_pointer; i < sunit_end_pointer; ++i)
        {
            v = sunit_clause_queue[i].var_num;
            if (sscore[v] > best_score)
            {
                best_v = v;
                index = i;
            }
        }
    }
    swap(sunit_clause_queue[sunit_beg_pointer], sunit_clause_queue[index]);
    v = sunit_clause_queue[sunit_beg_pointer].var_num;
    sense = sunit_clause_queue[sunit_beg_pointer].sense;
    sunit_beg_pointer++;

    if (fix[v] != -1)
        return;

    if (sense_in_sunit_clause_queue[v] == -3)
    {
        sense = choose_sense(v);
    }
    assign(v, sense);
}

void Decimation::random_propagation()
{
    int v, sense;
    v = unassigned_var[rand() % unassigned_var_count];
    sense = rand() % 2;
    assign(v, sense);
}

void Decimation::unit_prosess()
{
    while (unassigned_var_count > 0)
    {
        if (hunit_beg_pointer != hunit_end_pointer)
        {
            hunit_propagation();
            continue;
        }
        // #if USING_VOTING_IN_UNIT_PROCESS
        // int mode_random_num = rand() % 100;
        // // if(mode_random_num < mode_possibility || num_global_opt_solns == 0){
        // #endif
            if (sunit_beg_pointer != sunit_end_pointer)
            {
                sunit_propagation();
            }
            else
            {     
                // random_propagation(); 
                #if USING_VOTING_IN_UNIT_PROCESS
                int v = unassigned_var[rand() % unassigned_var_count];
                int sense = voting_by_local_opt(v, 1);
                assign(v, sense); 
                #else
                random_propagation();
                #endif
            }
        // #if USING_VOTING_IN_UNIT_PROCESS
        // }
        // else{    
            // int v = unassigned_var[rand() % unassigned_var_count];
            // int sense = voting_by_local_opt(v, 2);
            // assign(v, sense);    
        // }
        // #endif
    }
}

void Decimation::array_copy(int* arr1, int* arr2, int len)
{
    for(int i = 0;i <= len;i++)
    {
        arr1[i] = arr2[i];
    }
}

#endif
