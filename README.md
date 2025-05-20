# symnmf-clustering
Academic project implementing Symmetric Non-negative Matrix Factorization (SymNMF) in C with Python preprocessing and evaluation. The algorithm creates a symmetric similarity matrix and factorizes it into a lower-rank matrix, from which cluster assignments can be derived.

| File               | Description                                                                 |
|--------------------|-----------------------------------------------------------------------------|
| `symnmf.py`        | Handles I/O, CLI arguments, and calls the compiled C module                 |
| `symnmf.c`         | Core implementation of the SymNMF algorithm in C                            |
| `symnmf.h`         | Header file for the SymNMF function declarations                            |
| `symnmfmodule.c`   | C extension module that exposes the SymNMF logic to Python                  |
| `setup.py`         | Build script for compiling the C extension                                  |
| `analysis.py`      | Compares SymNMF clustering results to K-means++ using the Silhouette Score  |
