#pragma once
#include <stdexcept> 
#include <iostream> 
#include <iomanip>  
#include <string>
#include <vector>
#include <cmath>    
#include "Exceptions.h" 
#include "Complex.h"  

template <typename T>
class Matrix
{
private:
    std::vector<std::vector<T>> data;
    size_t rows;
    size_t cols;

public:
    // Constructor
    Matrix(size_t r=0,size_t c=0) : rows(r),cols(c) { data.resize(rows,std::vector<T>(cols,T{})); } // Initialize with default value of T

    // Resize matrix
    void resize(size_t r,size_t c)
    {
        rows=r;
        cols=c;
        data.assign(rows,std::vector<T>(cols,T{})); // Reinitialize with default value
    }

    // Getters for dimensions
    size_t getRows() const { return rows; }
    size_t getCols() const { return cols; }

    // Access element (read/write)
    T& operator()(size_t r,size_t c)
    {
        if(r>=rows||c>=cols)
            throw std::out_of_range("Matrix index out of bounds.");
        return data[r][c];
    }

    // Access element (read-only)
    const T& operator()(size_t r,size_t c) const
    {
        if(r>=rows||c>=cols)
            throw std::out_of_range("Matrix index out of bounds.");
        return data[r][c];
    }

    // Add a value to an existing element (useful for MNA stamping)
    void add(size_t r,size_t c,const T& value)
    {
        if(r>=rows||c>=cols)
            throw std::out_of_range("Matrix add index out of bounds.");
        data[r][c]+=value;
    }

    // Print matrix (for debugging) - MODIFIED to accept a title
    void print(const std::string& title="") const
    {
        if(!title.empty())
            std::cout<<"\n--- "<<title<<" ---\n";
        std::cout<<"Matrix ("<<rows<<"x"<<cols<<"):\n";
        for(size_t i=0; i<rows; ++i)
        {
            for(size_t j=0; j<cols; ++j)
                std::cout<<std::setw(12)<<std::fixed<<std::setprecision(4)<<data[i][j]<<" ";
            std::cout<<"\n";
        }
    }

    // Gaussian Elimination solver
    std::vector<T> solveGaussianElimination(std::vector<T> b) const
    {
        if(rows==0||cols==0)
            throw MatrixError("Cannot solve empty matrix.");
        if(rows!=cols)
            throw MatrixError("Matrix must be square for Gaussian Elimination.");
        if(b.size()!=rows)
            throw MatrixError("RHS vector size must match matrix dimensions.");
        Matrix<T> A_copy=*this; // Create a mutable copy of the matrix
        std::vector<T> x(rows); // Solution vector
        // Forward elimination
        for(size_t k=0; k<rows; ++k)
        {
            // Find pivot (largest absolute value in current column below or at k)
            size_t pivot_row=k;
            for(size_t i=k+1; i<rows; ++i)
            {
                // Use abs() which will correctly dispatch to std::abs for double
                // and our custom abs for Complex.
                if(abs(A_copy(i,k))>abs(A_copy(pivot_row,k)))
                    pivot_row=i;
            }

            if(pivot_row!=k)
            {
                // Swap rows in A_copy
                swap(A_copy.data[k],A_copy.data[pivot_row]);
                // Swap rows in b
                T temp;
                temp=b[k];
                b[k]=b[pivot_row];
                b[pivot_row]=temp;
            }
            // Check for singular matrix (pivot element is zero or very close to zero)
            // Use abs() for the check
            if(abs(A_copy(k,k))<1e-12)
                throw MatrixError("Matrix is singular or ill-conditioned, cannot solve.");
            // Eliminate column k
            for(size_t i=k+1; i<rows; ++i)
            {
                T factor=A_copy(i,k)/A_copy(k,k);
                for(size_t j=k; j<cols; ++j) // Iterate from k, not k+1 because A_copy(i,k) will become 0
                    A_copy(i,j)-=factor*A_copy(k,j);
                b[i]-=factor*b[k];
            }
        }
        // Back substitution
        for(long i=rows-1; i>=0; --i)
        {
            T sum_terms=T{}; // Initialize with default value (0 for double, Complex(0,0) for Complex)
            for(size_t j=i+1; j<cols; ++j)
                sum_terms+=A_copy(i,j)*x[j];
            x[i]=(b[i]-sum_terms)/A_copy(i,i);
        }
        return x;
    }
};
template<>
inline void Matrix<Complex>::print(const std::string& title) const
{
    if(!title.empty())
        std::cout<<"\n--- "<<title<<" ---\n";
    std::cout<<"Matrix ("<<rows<<"x"<<cols<<"):\n";
    for(size_t i=0; i<rows; ++i)
    {
        for(size_t j=0; j<cols; ++j) // This relies on the Complex::operator<< which handles its own formatting
            std::cout<<std::setw(12)<<data[i][j]<<" ";
        std::cout<<"\n";
    }
}

template class Matrix<double>;
template class Matrix<Complex>;