import numpy as np

def kmeans(k, max_iter, input_data):
    data_points = []
    with open(input_data) as f:
        for line in f:
            data_points.append([float(x) for x in line.split(",")])
    eps = 0.0001
    data_points = np.array(data_points)
    num_points, dimensions = data_points.shape
    centroids = data_points[:k].copy()  # initialize centroids with the first k points
    assignments = np.zeros(num_points, dtype=int)
    
    for iteration in range(max_iter):
        # Step 1: Assign each point to the closest centroid
        for point_index in range(num_points):
            point = data_points[point_index]
            closest_centroid = get_closest_centroid(point, centroids)
            assignments[point_index] = closest_centroid
        
        # Step 2: Update centroids based on current assignments
        updated_centroids = update_centroid_positions(data_points, assignments, k, dimensions)
        
        # Check for convergence by comparing old and new centroids
        has_converged = True
        for current, updated in zip(centroids, updated_centroids):
            if calculate_distance(current, updated) >= eps:
                has_converged = False
                break
        if has_converged:
            break
        
        centroids = updated_centroids.copy()
    
    return assignments

def update_centroid_positions(data_points, assignments, k, dimensions):
    new_centroids = np.zeros((k, dimensions))
    for cluster_index in range(k):
        # Get indices of all points assigned to this cluster
        cluster_points = data_points[assignments == cluster_index]
        # Handle empty cluster case
        if cluster_points.size == 0:
            new_centroids[cluster_index] = np.zeros(dimensions)
        else:
            new_centroids[cluster_index] = np.mean(cluster_points, axis=0)
    return new_centroids

def get_closest_centroid(point, centroids):
    # Calculate the distance between the point and each centroid
    distances = np.linalg.norm(centroids - point, axis=1)
    return np.argmin(distances)

def calculate_distance(vec1, vec2):
    # Calculate Euclidean distance between two points
    return np.linalg.norm(vec1 - vec2)
