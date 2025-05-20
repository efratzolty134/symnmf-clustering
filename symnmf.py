import numpy as np
import pandas as pd
import symnmfModule as nmf
import sys

np.random.seed(1234) 

def printMatrix(mat, rows, cols):
    for i in range(rows):
        for j in range(cols - 1):
            print("{:.4f}".format(mat[i][j]), end=',')
        print("{:.4f}".format(mat[i][cols-1]))

def initH(W, N, k):
    W = np.array(W)
    m = float(np.mean(W))
    if k:
        H = np.random.uniform(0,  2 * ((m/k) ** 0.5), (N, k))
    else: 
        print("An Error Has Occured")
        sys.exit(1)

    return H

def calcGoal(k, goal, file_name):
    """Calculate and return the matrix based on the specified goal and input file."""
    # Validate goal input
    valid_goals = {"ddg", "sym", "norm", "symnmf"}
    if goal not in valid_goals:
        print("An Error Has Occured")
        sys.exit(1)
    
    # Load data from the file
    try:
        X = pd.read_csv(file_name, header=None)
        X = X.to_numpy()
        N, d = np.shape(X)
        X = X.tolist()
    except:
        print("An Error Has Occured")
        sys.exit(1)

    # Ensure k is less than N
    if k >= N:
        print("An Error Has Occured")
        sys.exit(1)

    # Calculate the result based on the goal
    if goal == "sym":
        return nmf.sym(X)
    elif goal == "ddg":
        return nmf.ddg(X)
    elif goal == "norm":
        if N ==1:
            print("An Error Has Occured")
            sys.exit(1)
        return nmf.norm(X)
    elif goal == "symnmf":
        W = nmf.norm(X)
        H0 = initH(W, N, k)
        return nmf.symnmf(H0, W, N, k)

def print_result(matrix, goal, N, k):
    """Print the result based on the goal."""
    if goal in ["sym", "ddg", "norm"]:
        printMatrix(matrix, N, N)
    elif goal == "symnmf":
        printMatrix(matrix, N, k)

def main():
    if len(sys.argv) < 4:
        print("An Error Has Occured")
        sys.exit(1)

    k = int(sys.argv[1])
    goal = sys.argv[2]
    file_name = sys.argv[3]

    # Calculate the result matrix
    result_matrix = calcGoal(k, goal, file_name)
    if result_matrix == None:
        sys.exit(1)

    # Determine the dimensions for printing based on the goal
    N = len(result_matrix)
    k_dim = len(result_matrix[0]) if goal == "symnmf" else N

    # Print the result
    print_result(result_matrix, goal, N, k_dim)

if __name__ == "__main__":
    main()
