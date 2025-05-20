import sys
import numpy as np
import symnmf as nmf
import kmeans as km
import pandas as pd
from sklearn.metrics import silhouette_score

max_iter = 300

def main():
    k = int(sys.argv[1])  # Convert to integer immediately after reading from input
    input_data_path = sys.argv[2]
    data_points = pd.read_csv(input_data_path, header=None)
    data_points = data_points.to_numpy()
    kmeans_res = np.array(km.kmeans(k, max_iter, input_data_path))
    kmeans_score = silhouette_score(data_points, kmeans_res)
    
    symnmf_H = np.array(nmf.calcGoal(k, "symnmf", input_data_path))
    symnmf_res = np.argmax(symnmf_H, axis=1)
    symnmf_score = silhouette_score(data_points, symnmf_res)
    
    print("nmf: %.4f" % symnmf_score)
    print("kmeans: %.4f" % kmeans_score)
if __name__ == "__main__":
    main()
