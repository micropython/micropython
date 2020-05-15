"""Linear algebra functions"""


def cholesky(A):
    """
    :param ~ulab.array A: a positive definite, symmetric square matrix
    :return ~ulab.array L: a square root matrix in the lower triangular form
    :raises ValueError: If the input does not fulfill the necessary conditions

    The returned matrix satisfies the equation m=LL*"""
    ...

def det():
    """
    :param: m, a square matrix
    :return float: The determinant of the matrix

    Computes the eigenvalues and eigenvectors of a square matrix"""
    ...

def dot(m1, m2):
    """
    :param ~ulab.array m1: a matrix
    :param ~ulab.array m2: a matrix

    Computes the matrix product of two matrices

    **WARNING:** Unlike ``numpy``, this function cannot be used to compute the dot product of two vectors"""
    ...

def eig(m):
    """
    :param m: a square matrix
    :return tuple (eigenvectors, eigenvalues):

    Computes the eigenvalues and eigenvectors of a square matrix"""
    ...

def inv(m):
    """
    :param ~ulab.array m: a square matrix
    :return: The inverse of the matrix, if it exists
    :raises ValueError: if the matrix is not invertible

    Computes the inverse of a square matrix"""
    ...

def size(array):
    """Return the total number of elements in the array, as an integer."""
    ...

def trace(m):
    """
    :param m: a square matrix

    Compute the trace of the matrix, the sum of its diagonal elements."""
    ...
