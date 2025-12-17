#include <stdio.h>

#define MAX_STATES 32
#define MAX_SYMBOLS 10
#define EPSILON    -1

int num_states = 3;
int num_symbols = 3;


char symbols[MAX_SYMBOLS] = {'a', 'b', 'c'};

unsigned int transitions[MAX_STATES][MAX_SYMBOLS + 1] = {0};



void add_transition(int from, int symbol, int to) {
    if (symbol == EPSILON)
        transitions[from][num_symbols] |= (1 << to);
    else
        transitions[from][symbol] |= (1 << to);
}

void print_set(unsigned int set) {
    printf("{ ");
    for (int i = 0; i < num_states; i++)
        if (set & (1 << i))
            printf("%d ", i);
    printf("}");
}


unsigned int epsilon_closure(int state) {
    unsigned int closure = (1 << state);
    int changed;

    do {
        changed = 0;
        for (int s = 0; s < num_states; s++) {
            if (closure & (1 << s)) {
                unsigned int eps = transitions[s][num_symbols];
                if ((closure | eps) != closure) {
                    closure |= eps;
                    changed = 1;
                }
            }
        }
    } while (changed);

    return closure;
}



void remove_epsilon(unsigned int new_transitions[MAX_STATES][MAX_SYMBOLS],
                    unsigned int *new_finals,
                    unsigned int old_finals) {

    unsigned int eclosure[MAX_STATES];

    for (int i = 0; i < num_states; i++)
        eclosure[i] = epsilon_closure(i);

    for (int s = 0; s < num_states; s++) {
        for (int a = 0; a < num_symbols; a++) {
            unsigned int reachable = 0;

            for (int p = 0; p < num_states; p++)
                if (eclosure[s] & (1 << p))
                    reachable |= transitions[p][a];

            unsigned int result = 0;
            for (int r = 0; r < num_states; r++)
                if (reachable & (1 << r))
                    result |= eclosure[r];

            new_transitions[s][a] = result;
        }
    }

    *new_finals = 0;
    for (int s = 0; s < num_states; s++)
        if (eclosure[s] & old_finals)
            *new_finals |= (1 << s);
}


int main() {
  
    add_transition(0, 0, 0);      // 0 means a, 1 means b ,2 means c
    add_transition(0, EPSILON, 1); 
    add_transition(1, 1, 1);       
    add_transition(1, EPSILON, 2); 
    add_transition(2, 2, 2);       

    unsigned int final_states = (1 << 2);

    unsigned int new_transitions[MAX_STATES][MAX_SYMBOLS] = {0};
    unsigned int new_finals;

    remove_epsilon(new_transitions, &new_finals, final_states);

    
    printf("NFA without epsilon transitions:\n\n");

    for (int s = 0; s < num_states; s++) {
        for (int a = 0; a < num_symbols; a++) {
            printf("δ(%d, %c) = ", s, symbols[a]);
            print_set(new_transitions[s][a]);
            printf("\n");
        }
        printf("\n");
    }

    printf("Final states: ");
    print_set(new_finals);
    printf("\n");

    return 0;
}
