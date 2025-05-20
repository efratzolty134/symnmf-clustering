#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "symnmf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Helper function to allocate and populate a 2D array from a Python object */
static double** allocate_and_populate(PyObject *py_matrix, int rows, int cols) {
    double **matrix = (double**)calloc(rows, sizeof(double*));
    if (matrix == NULL) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for matrix");
        return NULL;
    }
    
    for (int i = 0; i < rows; i++) {
        PyObject *row_obj = PyObject_GetItem(py_matrix, PyLong_FromSsize_t(i));
        matrix[i] = (double*)malloc(cols * sizeof(double));
        if (matrix[i] == NULL) {
            PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for matrix row");
            for (int j = 0; j < i; j++) free(matrix[j]);
            free(matrix);
            return NULL;
        }
        for (int j = 0; j < cols; j++) {
            PyObject *coor = PyObject_GetItem(row_obj, PyLong_FromSsize_t(j));
            matrix[i][j] = PyFloat_AsDouble(coor);
            Py_DECREF(coor);
        }
        Py_DECREF(row_obj);
    }
    return matrix;
}

/*helper function to free a 2d array*/
void freeArray(double **array, int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        free(array[i]);
    }
    free(array);
}

static PyObject* py_sym(PyObject *self, PyObject *args) {
    int rows, cols, i, j;
    double **inputCdata, **similarity_matrix;
    PyObject *inputPyData, *pyVector, *pyCord, *result, *first_row;

    if (!PyArg_ParseTuple(args, "O", &inputPyData)) {
        printf("An Error Has Occurred\n");
        return NULL;
    }
    if (!PyList_Check(inputPyData)) {
        printf("An Error Has Occurred\n");
        return NULL; }
    
    rows = PyList_Size(inputPyData);
    first_row = PyList_GetItem(inputPyData, 0);
    cols = PyList_Size(first_row);

    inputCdata = allocate_and_populate(inputPyData, rows, cols);
    similarity_matrix = (double **)calculate_similarity_matrix(inputCdata, rows, cols);

    result = PyList_New(rows);
    for (i = 0; i < rows; i++)
    {
        pyVector = PyList_New(rows);
        for (j = 0; j < rows; j++)
        {
            pyCord = Py_BuildValue("f", similarity_matrix[i][j]);
            PyList_SetItem(pyVector, j, pyCord);
        }
        PyList_SetItem(result, i, pyVector);
    }

    freeArray(similarity_matrix, rows);

    return result;
}

static PyObject* py_diag(PyObject *self, PyObject *args)
{
    int rows, cols, i, j;
    double **inputCdata, **similarity_matrix, **diagonal_degree_matrix;
    PyObject *inputPyData, *pyVector, *pyCord, *result, *first_row;

    if (!PyArg_ParseTuple(args, "O", &inputPyData)) {
        printf("An Error Has Occurred\n");
        return NULL;
    }
    if (!PyList_Check(inputPyData)) {
        printf("An Error Has Occurred\n");
        return NULL;
    }
    
    rows = PyList_Size(inputPyData);
    first_row = PyList_GetItem(inputPyData, 0);
    cols = PyList_Size(first_row);

    inputCdata = allocate_and_populate(inputPyData, rows, cols);
    similarity_matrix = calculate_similarity_matrix(inputCdata, rows, cols);
    diagonal_degree_matrix = calculate_diagonal_degree_matrix(similarity_matrix, rows);

    result = PyList_New(rows);
    for (i = 0; i < rows; i++)
    {
        pyVector = PyList_New(rows);
        for (j = 0; j < rows; j++)
        {
            pyCord = Py_BuildValue("f", diagonal_degree_matrix[i][j]);
            PyList_SetItem(pyVector, j, pyCord);
        }
        PyList_SetItem(result, i, pyVector);
    }

    freeArray(diagonal_degree_matrix, rows);
    freeArray(similarity_matrix, rows);

    return result;
}

static PyObject* py_norm(PyObject *self, PyObject *args){
    int rows, cols, i, j;
    double **inputCdata, **similarity_matrix, **diagonal_degree_matrix, **normalized_similarity_matrix;
    PyObject *inputPyData, *pyVector, *pyCord, *result, *first_row;

    if (!PyArg_ParseTuple(args, "O", &inputPyData)) {
        printf("An Error Has Occurred\n");
        return NULL;
    }
    if (!PyList_Check(inputPyData)) {
        printf("An Error Has Occurred\n");
        return NULL;
    }
    
    rows = PyList_Size(inputPyData);
    first_row = PyList_GetItem(inputPyData, 0);
    cols = PyList_Size(first_row);

    inputCdata = allocate_and_populate(inputPyData, rows, cols);
    similarity_matrix = calculate_similarity_matrix(inputCdata, rows, cols);
    diagonal_degree_matrix = calculate_diagonal_degree_matrix(similarity_matrix, rows);
    normalized_similarity_matrix = calculate_normalized_similarity_matrix(similarity_matrix, diagonal_degree_matrix, rows);

    result = PyList_New(rows);
    for (i = 0; i < rows; i++) {
        pyVector = PyList_New(rows);
        for (j = 0; j < rows; j++) {
            pyCord = Py_BuildValue("f", normalized_similarity_matrix[i][j]);
            PyList_SetItem(pyVector, j, pyCord);
        }
        PyList_SetItem(result, i, pyVector);
    }

    freeArray(diagonal_degree_matrix, rows);
    freeArray(similarity_matrix, rows);
    freeArray(normalized_similarity_matrix, rows);
    return result;
}


static PyObject* py_h(PyObject *self, PyObject *args) {
    int N, k;
    PyObject *py_H0, *py_W, *py_H;
    double **H0, **W;
    if (!PyArg_ParseTuple(args, "OOii", &py_H0, &py_W, &N, &k)) return NULL;

    W = allocate_and_populate(py_W, N, N);
    if (W == NULL) return NULL;

    H0 = allocate_and_populate(py_H0, N, k);
    if (H0 == NULL) {
        freeArray(W, N);
        return NULL;
    }
    convergence(H0, W, N, k);

    py_H = PyList_New(N);
    if (py_H == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create list for result");
        freeArray(H0, N);
        freeArray(W, N);
        return NULL;
    }
    for (int i = 0; i < N; i++) {
        PyObject *row_tuple = PyTuple_New(k);
        if (row_tuple == NULL) {
            PyErr_SetString(PyExc_RuntimeError, "Failed to create tuple for result");
            freeArray(H0, N);
            freeArray(W, N);
            Py_DECREF(py_H);
            return NULL; }
        for (int j = 0; j < k; j++) {
            PyTuple_SetItem(row_tuple, j, PyFloat_FromDouble(H0[i][j]));
        }
        PyList_SetItem(py_H, i, row_tuple); 
    }
    freeArray(H0, N);
    freeArray(W, N);
    return py_H;
}

/* Define the methods of the module */
static PyMethodDef methods[] = {
    {"sym", (PyCFunction)py_sym, METH_VARARGS, PyDoc_STR("get Similarity matrix")},
    {"ddg", (PyCFunction)py_diag, METH_VARARGS, PyDoc_STR("get Diagonal matrix")},
    {"norm", (PyCFunction)py_norm, METH_VARARGS, PyDoc_STR("get Norm matrix")},
    {"symnmf", (PyCFunction)py_h, METH_VARARGS, PyDoc_STR("get H matrix")},
    {NULL, NULL, 0, NULL}
};

/* Define the module */
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "symnmfModule", /* Module name */
    NULL,
    -1,
    methods
};

/* Module initialization function */
PyMODINIT_FUNC PyInit_symnmfModule(void) {
    PyObject *m;
    m = PyModule_Create(&moduledef);
    if (!m) {
        return NULL;
    }
    return m;
}