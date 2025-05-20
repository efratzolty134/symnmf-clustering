#ifndef SYMNMF_H_
#define SYMNMF_H_

double** calculate_similarity_matrix(double** input_data, int rows, int cols);
double** calculate_diagonal_degree_matrix(double** similarity_matrix, int rows);
double** calculate_normalized_similarity_matrix(double** similarity_matrix, double** degree_matrix, int rows);
void updateH(double **W, double **H, int N, int k);
void convergence(double **H, double **W, int N, int k);

#endif