#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


/*struct cord - each cord is a node in a linked list*/
typedef struct cord {
    double value;
    struct cord *next;
}cord;

/*functions to create and free cords*/

cord* createCord(double val){
    cord * c = (cord*)malloc(sizeof(cord));
    c->value = val;
    c->next = NULL;
    return c;
}

void freeCords(cord* head) {
    cord* current = head;
    cord* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
}

/*struct vector - each vector is a node in a linked list*/
typedef struct vector{
    struct vector *next;
    struct cord *cords;
}vector;


/* Create and delete vectors */
vector* createVector(cord* head_cord){
    vector * v = (vector*)malloc(sizeof(vector));
    v->cords = head_cord;
    v->next = NULL;
    return v;
}

/*free allocated vectors*/
void freeVectors(vector* head) {
    vector* current = head;
    vector* next;

    while (current != NULL) {
        next = current->next;
        freeCords(current->cords);
        free(current);
        current = next;
    }
}

int not_valid_goal(const char *goal) {
    return !(strcmp(goal, "sym") == 0 || strcmp(goal, "ddg") == 0 || strcmp(goal, "norm") == 0);
}

double squared_euclidean_distance(double* point1, double* point2, int d) {
    double dist = 0;
    int i;
    for (i = 0; i < d; i++) {
        double diff = fabs(point1[i] - point2[i]);
        dist += pow(diff, 2);
    }
    return dist;
}


void matrix_multiplication(double **mat1, double **mat2, int rows1, int cols1, int cols2, double **result)
{
    int i, j, k;
    for (i = 0; i < rows1; i++)
    {
        result[i] = (double*)calloc(cols2, sizeof(double));
    }
    for (i = 0; i < rows1; i++) {
        for (j = 0; j < cols2; j++) {
            for (k = 0; k < cols1; k++) {
                result[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
    }
}

void print_output(double** output, int rows, int cols) {
    int i, j;

    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            if (j > 0) {
                printf(",");
            }
            printf("%.4f", output[i][j]);
        }
        printf("\n");
}
}

double** calculate_similarity_matrix(double** input_data, int rows, int cols) {
    double** similarity_matrix = malloc(rows * sizeof(double *));
    int i, j;
    for (i = 0; i < rows; i++) {
        similarity_matrix[i] = malloc(rows * sizeof(double));
        for (j = 0; j < i+1; j++) {
            if (i == j) {
                similarity_matrix[i][j] = 0;
            } 
            else {
                similarity_matrix[i][j] = exp(-(squared_euclidean_distance(input_data[i], input_data[j], cols))/2);
                similarity_matrix[j][i] = similarity_matrix[i][j];
    
            }
        }
    }
    return similarity_matrix;

}

double** calculate_diagonal_degree_matrix(double** similarity_matrix, int rows) {
    double** degree_matrix = malloc(rows * sizeof(double *));
    int i, j;
    double sum = 0;
    for (i = 0; i < rows; i++) {
        degree_matrix[i] = malloc(rows * sizeof(double));
        for (j = 0; j < rows; j++) {
            if (i!=j){
                degree_matrix[i][j] = 0;
            }
            sum  += similarity_matrix[i][j];
        }
        degree_matrix[i][i] = sum;
        sum = 0;
    }
    return degree_matrix;
}

double** calculate_normalized_similarity_matrix(double** similarity_matrix, double** degree_matrix, int rows) {
    double** normalized_matrix = malloc(rows * sizeof(double *));
    double** process_matrix = malloc(rows * sizeof(double *));
    double** sqrt_degree_matrix = malloc(rows * sizeof(double *));
    int i, j;
    for (i = 0; i < rows; i++) {
        sqrt_degree_matrix[i] = malloc(rows * sizeof(double));
        for (j = 0; j < rows; j++) {
            if (i==j){
                sqrt_degree_matrix[i][j] = 1/sqrt(degree_matrix[i][j]);
            }
            else{
                sqrt_degree_matrix[i][j] = 0;
            }
        }
    }
    matrix_multiplication(sqrt_degree_matrix, similarity_matrix, rows, rows, rows, process_matrix);
    matrix_multiplication(process_matrix, sqrt_degree_matrix, rows, rows, rows, normalized_matrix);
    for (i = 0; i < rows; i++) {
        free(sqrt_degree_matrix[i]);
        free(process_matrix[i]);
    }
    free(sqrt_degree_matrix);
    free(process_matrix);
    return normalized_matrix;
}

void transpose(double **mat, int rows, int cols, double** res) {
    int i, j;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            res[j][i] = mat[i][j];
        }
    }
}

int validate_input(int argc, char *argv[], char **goal) {
    if (argc != 3) {
        printf("An Error Has Occurred\n");
        return 1;
    }

    *goal = argv[1];

    if (not_valid_goal(*goal)) {
        printf("An Error Has Occurred\n");
        return 1;
    }

    return 0;
}

void create_linked_list(double ***input_data, int *rows, int *cols, FILE *file) {
    struct cord *head_cord = createCord(0);
    struct cord *curr_cord = head_cord;
    struct vector *head_vec = createVector(head_cord);
    struct vector *curr_vec = head_vec;
    int i, j;
    double n;
    char c;
    
    while (fscanf(file, "%lf%c", &n, &c) == 2) { /*iterate over file*/
        if (c == '\n') {
            curr_cord->value = n;
            curr_cord->next = NULL;
            curr_vec->cords = head_cord;
            head_cord = createCord(0);
            curr_cord = head_cord;
            curr_vec->next = createVector(head_cord);
            curr_vec = curr_vec->next;
            (*rows)++;
            continue;}
        curr_cord->value = n;
        curr_cord->next = createCord(0);
        curr_cord = curr_cord->next; }
    curr_cord = head_vec->cords;

    while (curr_cord != NULL) { 
        (*cols)++;
        curr_cord = curr_cord->next;}

    *input_data = malloc(*rows * sizeof(double *));
    curr_vec = head_vec;

    for (i = 0; i < *rows; i++) { /*turn the linkedlist to an array*/
        (*input_data)[i] = malloc(*cols * sizeof(double));
        curr_cord = curr_vec->cords;
        for (j = 0; j < *cols; j++) {
            (*input_data)[i][j] = curr_cord->value;
            curr_cord = curr_cord->next;}
        curr_vec = curr_vec->next;}
    freeVectors(head_vec); }

/*function for freeing a 2d array*/
void free_array(double **matrix, int size) {
    int i;
    for (i= 0; i < size; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void print_goal_output(char *goal, double **similarity_matrix, int rows) {
    double **degree_matrix = NULL;
    double **normalized_matrix = NULL;
    if (strcmp(goal, "sym") == 0) {
        print_output(similarity_matrix, rows, rows);
    } else if (strcmp(goal, "ddg") == 0) {
        degree_matrix = calculate_diagonal_degree_matrix(similarity_matrix, rows);
        print_output(degree_matrix, rows, rows);
        free_array(degree_matrix, rows);
    } else if (strcmp(goal, "norm") == 0) {
        degree_matrix = calculate_diagonal_degree_matrix(similarity_matrix, rows);
        normalized_matrix = calculate_normalized_similarity_matrix(similarity_matrix, degree_matrix, rows);
        print_output(normalized_matrix, rows, rows);
        free_array(degree_matrix, rows);
        free_array(normalized_matrix, rows);
    }
}

void copyMat(double **mat, double **copiedMat, int rows, int cols) {
    int i, j;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            copiedMat[i][j] = mat[i][j];
        }
    }
}

void updateH(double **W, double **H, int N, int k){
    int i,j;
    double b = 0.5;
    double **numerator, **HHt, **denominator, **Ht;

    numerator = (double**)calloc(N, sizeof(double*));
    denominator = (double**)calloc(N, sizeof(double*));
    HHt = (double**)calloc(N, sizeof(double*));
    assert(numerator != NULL);
    assert(denominator != NULL);
    assert(HHt != NULL);

    for (i = 0; i < N; i++) {
        numerator[i] = (double*)calloc(k, sizeof(double));
        denominator[i] = (double*)calloc(k, sizeof(double));
        HHt[i] = (double*)calloc(N, sizeof(double));
        assert(numerator[i] != NULL);
        assert(denominator[i] != NULL);
        assert(HHt[i] != NULL); }

    Ht = (double**)calloc(k, sizeof(double*));
    assert(Ht != NULL);
    for (i = 0; i < k; i++) {
        Ht[i] = (double*)calloc(N, sizeof(double));
        assert(Ht[i] != NULL);}
    transpose(H, N, k, Ht);
    matrix_multiplication(W, H, N, N, k, numerator);
    matrix_multiplication(H, Ht, N, k, N, HHt);
    matrix_multiplication(HHt, H, N, N, k, denominator);

    for (i = 0; i < N; i++) {
        for (j = 0; j < k; j++) {
            if (denominator[i][j] == 0) {
                denominator[i][j] = 1e-6; }
            H[i][j] = H[i][j] * (1 - b + (b * numerator[i][j] / denominator[i][j]));}}
    free_array(numerator, N);
    free_array(denominator, N);
    free_array(HHt, N);
    free_array(Ht, k);
}

double frobNorm(double **mat, int rows, int cols)
{
    double result = 0;
    int i, j;
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            result += pow(mat[i][j], 2);
        }
    }
    return result;
}

void convergence(double **H, double **W, int N, int k){
    double **prevH, **Hdiff;
    int maxIter = 300, currentIter = 1, i, j;
    double epsilon = pow(10 , -4);

    prevH = (double**)calloc(N, sizeof(double*));
    assert(prevH != NULL);
    for (i = 0; i < N; i++) {
        prevH[i] = (double*)calloc(k, sizeof(double));
        assert(prevH[i] != NULL);
    }
    copyMat(H, prevH, N, k);
    updateH(W, H, N, k);

    Hdiff = (double**)calloc(N, sizeof(double*));
    assert(Hdiff != NULL);
    for (i = 0; i < N; i++) {
        Hdiff[i] = (double*)calloc(k, sizeof(double));
        assert(Hdiff[i] != NULL);
    }

    for (i = 0; i < N; i++) {
        for (j = 0; j < k; j++) {
            Hdiff[i][j] = H[i][j] - prevH[i][j];
        }
    }
    /*updates H until it has converged*/
    while (currentIter < maxIter && frobNorm(Hdiff, N, k) > epsilon) {
        copyMat(H, prevH, N, k);
        updateH(W, H, N, k);
        for (i = 0; i < N; i++) {
            for (j = 0; j < k; j++) {
                    Hdiff[i][j] = H[i][j] - prevH[i][j]; }}
        currentIter++; }
    free_array(Hdiff, N);

    for (i = 0; i < N; i++) {
        free(prevH[i]); }
    free(prevH);
}

int main(int argc, char *argv[]) {
    double **input_data = NULL;
    double **similarity_matrix = NULL;
    char *goal = NULL;
    int rows = 0, cols = 0;
    FILE *file;

    if (validate_input(argc, argv, &goal) != 0) {
        return 1;
    }
    file = fopen(argv[2], "r");
    if (file == NULL) {
        printf("An Error Has Occurred\n");
        return 1;
    }
    create_linked_list(&input_data, &rows, &cols, file); 
    similarity_matrix = calculate_similarity_matrix(input_data, rows, cols);

    print_goal_output(goal, similarity_matrix, rows);

    free_array(input_data, rows);
    free_array(similarity_matrix, rows);
    
    return 0;
}