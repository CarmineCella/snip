import numpy as np
from sklearn.cluster import KMeans

# -------- LINEAR REGRESSION --------

print("---- Linear Regression ----")
X = np.array([1, 2, 3])
y = np.array([2, 4, 6])

# Make X a 2D array for linear regression (column vector)
X_design = np.vstack([X, np.ones(len(X))]).T
# Solve least squares
w, residuals, rank, s = np.linalg.lstsq(X_design, y, rcond=None)

print("Linear Regression Coefficients (slope, intercept):", w)

# Predict at x=4
x_new = np.array([4])
x_new_design = np.vstack([x_new, np.ones(len(x_new))]).T
y_pred = x_new_design @ w

print("Predicted value for x=4:", y_pred[0])  # Should be close to 8

# -------- KMEANS CLUSTERING --------

print("\n---- KMeans ----")
X_kmeans = np.array([
    [1, 1],
    [2, 2],
    [9, 9]
])

kmeans = KMeans(n_clusters=2, random_state=0, n_init="auto").fit(X_kmeans)
centroids = kmeans.cluster_centers_

print("KMeans centroids:", centroids)
# Should find centers near (1.5, 1.5) and (9,9)

# -------- KNN CLASSIFICATION --------

print("\n---- KNN ----")
dataset = [
    (np.array([1, 1]), 0),  # point (1,1) labeled 0
    (np.array([5, 5]), 1)   # point (5,5) labeled 1
]

query = np.array([2, 2])
k = 1

# Compute distances
distances = []
for point, label in dataset:
    dist = np.linalg.norm(point - query)
    distances.append((dist, label))

# Sort by distance
distances.sort()

# Pick k nearest labels
nearest_labels = [label for (dist, label) in distances[:k]]

# Majority vote (simple since k=1)
prediction = max(set(nearest_labels), key=nearest_labels.count)

print("Predicted label for (2,2):", prediction)  # Should be 0
