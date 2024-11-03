#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int POPULATION_SIZE = 60;
int MAX_GENERATIONS = 100;
int SLOT = 4;
int COLORS = 6;
float CROSSOVER_RATE = 0.7;
float MUTATION_RATE = 0.03;
float PERMUTATION_RATE = 0.03;
float INVERSION_RATE = 0.03;
int WHITE_WEIGHT = 1;
int BLACK_WEIGHT = 2;

typedef struct
{
    int white;
    int black;
} Score;

typedef struct
{
    int *code;
    Score score;
} Guess;

typedef struct
{
    Guess *guess;
    int turn;
} Guesses;

typedef struct
{
    int *code;
    int fitness;
} Code_Fitness;

typedef struct
{
    int *code;
} Code;

typedef struct
{
    Code *code;
    int size;
} Chosen_One;

// Function declaration

float random_float()
{
    return (float)rand() / (float)(RAND_MAX);
}

// play game
Score play_game(int *guess, int *answer)
{
    Score score;
    score.white = 0;
    score.black = 0;
    int *answer_copy = (int *)malloc(sizeof(int) * SLOT);
    for (int i = 0; i < SLOT; i++)
    {
        answer_copy[i] = answer[i];
    }
    // Check black
    for (int i = 0; i < SLOT; i++)
    {
        if (guess[i] == answer_copy[i])
        {
            score.black++;
            answer_copy[i] = -1;
        }
    }
    // Check white
    for (int i = 0; i < SLOT; i++)
    {
        for (int j = 0; j < SLOT; j++)
        {
            if (guess[i] == answer_copy[j])
            {
                score.white++;
                answer_copy[j] = -1;
                break;
            }
        }
    }
    // Free memory
    free(answer_copy);
    return score;
}

// Generate random answer
int *generate_answer()
{
    int *answer = (int *)malloc(sizeof(int) * SLOT);
    for (int i = 0; i < SLOT; i++)
    {
        answer[i] = rand() % COLORS;
    }
    return answer;
}

int *SinglePointCrossover(int *code1, int *code2)
{
    int *newcode = malloc(sizeof(int) * SLOT);
    if ((rand() % 100) >= 50)
    {
        for (int i = 0; i < SLOT; i++)
        {
            if (i < 2)
            {
                newcode[i] = code1[i];
            }
            else
            {
                newcode[i] = code2[i];
            }
        }
    }
    else
    {
        for (int i = 0; i < SLOT; i++)
        {
            if (i < 2)
            {
                newcode[i] = code2[i];
            }
            else
            {
                newcode[i] = code1[i];
            }
        }
    }
    return newcode;
}

int *TwoPointCrossover(int *code1, int *code2)
{
    // using CROSSOVER_PROBABILITY to decide whether to crossover or not
    int *newcode = malloc(sizeof(int) * SLOT);
    // make a , b for the index of the crossover
    int a = rand() % SLOT;
    int b = rand() % SLOT;
    // make sure that a < b
    if (a > b)
    {
        int temp = a;
        a = b;
        b = temp;
    }
    if ((rand() % 100) >= 50)
    {
        for (int i = 0; i < SLOT; i++)
        {
            if (i >= a && i < b)
            {
                newcode[i] = code1[i];
            }
            else
            {
                newcode[i] = code2[i];
            }
        }
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            if (i >= a && i < b)
            {
                newcode[i] = code2[i];
            }
            else
            {
                newcode[i] = code1[i];
            }
        }
    }
    return newcode;
}

int *UniformCrossove(int *code1, int *code2)
{
    int *newcode = malloc(sizeof(int) * SLOT);
    for (int i = 0; i < SLOT; i++)
    {
        if (random_float() < CROSSOVER_RATE)
        {
            newcode[i] = code1[i];
        }
        else
        {
            newcode[i] = code2[i];
        }
    }
    return newcode;
}

int *mutate(int *code)
{
    // rand the index to mutate
    int index = rand() % SLOT;
    code[index] = rand() % COLORS + 1;
    return code;
}

int *permute(int *code)
{
    // loop all the code and swap the code by using rand
    for (int i = 0; i < SLOT; i++)
    {
        int index = rand() % SLOT;
        int temp = code[i];
        code[i] = code[index];
        code[index] = temp;
    }
    return code;
}

int *inversion(int *code)
{
    // rand the index to invert
    int index = rand() % SLOT;
    int index2 = rand() % SLOT;
    // make sure that index < index2
    if (index > index2)
    {
        int temp = index;
        index = index2;
        index2 = temp;
    }
    // loop from index to index2 and swap the code
    for (int i = index; i < index2; i++)
    {
        int temp = code[i];
        code[i] = code[i + 1];
        code[i + 1] = temp;
    }
    return code;
}

// Fitness function
int fitness(Guesses guesses, int *answer)
{
    int sum_black = 0;
    int sum_white = 0;
    for (int i = 0; i < guesses.turn; i++)
    {
        Score guess_score = play_game(guesses.guess[i].code, answer);
        sum_black += fabs(guess_score.black - guesses.guess[i].score.black);
        sum_white += fabs(guess_score.white - guesses.guess[i].score.white);
    }
    return sum_black * BLACK_WEIGHT + sum_white * WHITE_WEIGHT;
}

int check_same_code(int *code1, int *code2)
{
    for (int i = 0; i < SLOT; i++)
    {
        if (code1[i] != code2[i])
        {
            return 0;
        }
    }
    return 1;
}

// Genetic Evolution
Chosen_One Genetic_Evolution(Guesses guesses, int population_size, int max_generation)
{
    Code_Fitness *population = (Code_Fitness *)malloc(sizeof(Code_Fitness) * population_size);
    for (int i = 0; i < population_size; i++)
    {
        population[i].code = generate_answer();
        population[i].fitness = fitness(guesses, population[i].code);
    }
    for (int i = 1; i < population_size; i++)
    {
        Code_Fitness temp = population[i];
        int j = i - 1;
        while (j >= 0 && population[j].fitness > temp.fitness)
        {
            population[j + 1] = population[j];
            j--;
        }
        population[j + 1] = temp;
    }

    int h = 0;
    Chosen_One chosen_one;
    chosen_one.size = 0;
    chosen_one.code = (Code *)malloc(sizeof(Code) * population_size);
    while (h < max_generation && chosen_one.size < population_size)
    {
        int new_population_size = 0;
        Code_Fitness *new_population = (Code_Fitness *)malloc(sizeof(Code_Fitness) * population_size);
        for (int i = 0; i < population_size; i++)
        {
            if (i == population_size - 1)
            {
                new_population[new_population_size].code = population[i].code;
                break;
            }
            int *newcode = UniformCrossove(population[i].code, population[i + 1].code);
            if (random_float() < MUTATION_RATE)
            {
                newcode = mutate(newcode);
            }
            if (random_float() < PERMUTATION_RATE)
            {
                newcode = permute(newcode);
            }
            if (random_float() < INVERSION_RATE)
            {
                newcode = inversion(newcode);
            }
            new_population[new_population_size].code = newcode;
            new_population_size++;
        }
        for (int i = 0; i < new_population_size; i++)
        {
            new_population[i].fitness = fitness(guesses, new_population[i].code);
        }
        for (int i = 1; i < new_population_size; i++)
        {
            Code_Fitness temp = new_population[i];
            int j = i - 1;
            while (j >= 0 && new_population[j].fitness > temp.fitness)
            {
                new_population[j + 1] = new_population[j];
                j--;
            }
            new_population[j + 1] = temp;
        }

        // int found = 0;
        // for (int i = 0; i < chosen_one.size; i++)
        // {
        //     if (check_same_code(chosen_one.code[i].code, new_population[0].code))
        //     {
        //         for (int j = i; j < chosen_one.size - 1; j++)
        //         {
        //             chosen_one.code[j] = chosen_one.code[j + 1];
        //         }
        //         chosen_one.size--;
        //     }
        // }
        for (int i = 0; i < new_population_size; i++)
        {
            if (new_population[i].fitness != 0)
            {
                continue;
            }
            int found = 0;
            // if found delete from chosen_one
            for (int j = 0; j < chosen_one.size; j++)
            {
                if (check_same_code(chosen_one.code[j].code, new_population[i].code))
                {
                    for (int k = j; k < chosen_one.size - 1; k++)
                    {
                        chosen_one.code[k] = chosen_one.code[k + 1];
                    }
                    chosen_one.size--;
                    found = 1;
                    break;
                }
            }
        }

        int eligible_size = 0;
        Code *eligible = (Code *)malloc(sizeof(Code) * population_size);
        for (int i = 0; i < new_population_size; i++)
        {
            if (new_population[i].fitness == 0)
            {
                eligible[eligible_size].code = new_population[i].code;
                eligible_size++;
            }
        }
        if (eligible_size == 0)
        {
            h++;
            continue;
        }
        for (int i = 0; i < eligible_size; i++)
        {
            if (chosen_one.size == population_size)
            {
                break;
            }
            // Check if the code is in chosen_one
            int found = 0;
            for (int j = 0; j < chosen_one.size; j++)
            {
                if (check_same_code(chosen_one.code[j].code, eligible[i].code))
                {
                    found = 1;
                    break;
                }
            }
            if (found == 1)
            {
                continue;
            }
            chosen_one.code[chosen_one.size].code = eligible[i].code;
            chosen_one.size++;
        }
        new_population_size = 0;
        for (int i = 0; i < eligible_size; i++)
        {
            new_population[new_population_size].code = eligible[i].code;
            new_population_size++;
        }
        while (new_population_size < population_size)
        {
            int *random_code = generate_answer();
            int found = 0;
            for (int i = 0; i < new_population_size; i++)
            {
                if (check_same_code(new_population[i].code, random_code))
                {
                    found = 1;
                    break;
                }
            }
            if (found == 0)
            {
                new_population[new_population_size].code = random_code;
                new_population_size++;
            }
        }
        for (int i = 0; i < population_size; i++)
        {
            population[i].code = new_population[i].code;
            population[i].fitness = new_population[i].fitness;
        }
        h++;
    }
    return chosen_one;
}

int fitness_score_next_guess(Chosen_One chosen_one, int *answer)
{
    int sum_black = 0;
    int sum_white = 0;
    for (int i = 0; i < chosen_one.size; i++)
    {
        Score guess_score = play_game(answer, chosen_one.code[i].code);
        sum_black += fabs(guess_score.black - SLOT);
        sum_white += fabs(guess_score.white - SLOT);
    }
    return sum_black * BLACK_WEIGHT + sum_white * WHITE_WEIGHT;
}

int main()
{
    srand(time(NULL));
    Code answer;
    answer.code = generate_answer();

    printf("Answer: ");
    for (int i = 0; i < SLOT; i++)
    {
        printf("%d ", answer.code[i]);
    }
    printf("\n");

    Guesses guesses;
    guesses.turn = 0;
    guesses.guess = (Guess *)malloc(sizeof(Guess) * 100);
    // Using genetic algorithm to guess the answer
    // Test first guess
    // using code 1122
    int *first_guess = (int *)malloc(sizeof(int) * SLOT);
    first_guess[0] = 1;
    first_guess[1] = 1;
    first_guess[2] = 2;
    first_guess[3] = 3;
    if (SLOT == 6)
    {
        first_guess[4] = 3;
        first_guess[5] = 4;
    }
    Score first_score = play_game(first_guess, answer.code);

    printf("Turn %d\n", guesses.turn + 1);
    printf("Guess:");
    for (int i = 0; i < SLOT; i++)
    {
        printf(" %d", first_guess[i]);
    }
    printf("\n");
    printf("Score: %d %d\n", first_score.black, first_score.white);

    guesses.guess[guesses.turn].code = first_guess;
    guesses.guess[guesses.turn].score = first_score;
    guesses.turn++;
    // Genetic evolution
    while (guesses.guess[guesses.turn - 1].score.black != SLOT)
    {
        Chosen_One chosen_one = Genetic_Evolution(guesses, POPULATION_SIZE, MAX_GENERATIONS);
        while (chosen_one.size == 0)
        {
            // Genetic Again
            chosen_one = Genetic_Evolution(guesses, POPULATION_SIZE * 2, MAX_GENERATIONS / 2);
        }
        int chosen_one_size = chosen_one.size;
        printf("After Genetic Chosen_One Size: %d\n", chosen_one.size);
        // Clear Duplicate Guess In Chosen_One and in Guesses
        // Print All Chosen
        for (int i = 0; i < chosen_one.size; i++)
        {
            printf("Chosen_One %d: ", i);
            for (int j = 0; j < SLOT; j++)
            {
                printf("%d ", chosen_one.code[i].code[j]);
            }
            printf("\n");
        }

        for (int i = 0; i < chosen_one.size; i++)
        {
            // Check if the code is in guesses
            for (int j = 0; j < guesses.turn; j++)
            {
                if (check_same_code(chosen_one.code[i].code, guesses.guess[j].code))
                {
                    printf("Found Duplicate Guess %d %d\n", i, j);
                    for (int k = i; k < chosen_one.size - 1; k++)
                    {
                        chosen_one.code[k] = chosen_one.code[k + 1];
                    }
                    chosen_one.size--;
                }
            }
        }
        // printf

        Chosen_One chosen_one_next_guess;
        chosen_one_next_guess.size = 0;
        chosen_one_next_guess.code = (Code *)malloc(sizeof(Code) * chosen_one.size);

        while (chosen_one.size > 1)
        {
            printf("Chosen_One Size: %d\n", chosen_one.size);
            int fitness_min = 0;
            for (int i = 0; i < chosen_one.size; i++)
            {
                int fitness_score = fitness_score_next_guess(chosen_one, chosen_one.code[i].code);
                printf("Fitness Score: %d\n", fitness_score);
                if (fitness_score > fitness_min)
                {
                    // Clear the chosen_one_next_guess
                    chosen_one_next_guess.size = 0;
                    chosen_one_next_guess.code = (Code *)malloc(sizeof(Code) * chosen_one.size);
                    fitness_min = fitness_score;
                    chosen_one_next_guess.code[chosen_one_next_guess.size].code = chosen_one.code[i].code;
                    chosen_one_next_guess.size++;
                }
                else if (fitness_score == fitness_min)
                {
                    chosen_one_next_guess.code[chosen_one_next_guess.size].code = chosen_one.code[i].code;
                    chosen_one_next_guess.size++;
                }
            }
            // Change the chosen_one to chosen_one_next_guess and clear the chosen_one_next_guess
            if (chosen_one_next_guess.size == chosen_one.size)
            {
                printf("Break By Chosen_One Size: %d\n", chosen_one.size);
                break;
            }

            for (int i = 0; i < chosen_one_next_guess.size; i++)
            {
                chosen_one.code[i].code = chosen_one_next_guess.code[i].code;
            }
            chosen_one.size = chosen_one_next_guess.size;
            // Free chosen_one_next_guess
            // for (int i = 0; i < chosen_one_next_guess.size; i++)
            // {
            //     free(chosen_one_next_guess.code[i].code);
            // }
            chosen_one_next_guess.size = 0;
            chosen_one_next_guess.code = (Code *)malloc(sizeof(Code) * chosen_one.size);
            printf("Chosen_One Size: %d\n", chosen_one.size);
        }

        int *guess = chosen_one.code[0].code;
        Score score = play_game(guess, answer.code);
        printf("Turn %d\n", guesses.turn + 1);
        printf("Guess:");
        for (int i = 0; i < SLOT; i++)
        {
            printf(" %d", guess[i]);
        }
        printf("\n");
        printf("Score: %d %d\n", score.black, score.white);

        // // Free Chosen_One Chosen_One_Next_Guess
        // for (int i = 0; i < chosen_one_size; i++)
        // {
        //     free(chosen_one.code[i].code);
        // }
        // for (int i = 0; i < chosen_one_next_guess.size; i++)
        // {
        //     free(chosen_one_next_guess.code[i].code);
        // }
        // free(chosen_one.code);
        // free(chosen_one_next_guess.code);

        guesses.guess[guesses.turn].code = guess;
        guesses.guess[guesses.turn].score = score;
        guesses.turn++;
    }
    if (guesses.guess[guesses.turn - 1].score.black == SLOT)
    {
        printf("You win\n");
    }
    else
    {
        printf("You lose\n");
        // print the answer the last guess and the score
        printf("Answer: ");
        for (int i = 0; i < SLOT; i++)
        {
            printf("%d ", answer.code[i]);
        }
        printf("\n");
        printf("Last Guess:");
        for (int i = 0; i < SLOT; i++)
        {
            printf(" %d", guesses.guess[guesses.turn - 1].code[i]);
        }
        printf("\n");
        printf("Score: %d %d\n", guesses.guess[guesses.turn - 1].score.black, guesses.guess[guesses.turn - 1].score.white);
    }
}
