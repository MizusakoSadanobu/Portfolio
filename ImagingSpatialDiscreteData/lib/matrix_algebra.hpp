/* 
 * File:   matrix_algebra.hpp
 * Author: mizusako
 *
 * Created on 2014/11/16, 11:37
 */


#ifndef MATRIX_ALGEBRA_HPP
#define	MATRIX_ALGEBRA_HPP


#include "Basic_include_file_set.h"
#include "matrix_algebra.h"


//////////////////////////////////////////////////////////////////////
// namespace NS_MATRIX_ALGEBRA                                      //
//////////////////////////////////////////////////////////////////////


double NS_MATRIX_ALGEBRA::Product(int size0, double* v0, double* v1) {
    double ret = 0.0;
    for (int i = 0; i < size0; i++) ret += v0[i] * v1[i];
    return (ret);
}


void NS_MATRIX_ALGEBRA::Product(int size0, int size1, double** m, double* v, double* vn) {
    for (int i0 = 0; i0 < size0; ++i0) {
        vn[i0] = 0.;
        for (int i1 = 0; i1 < size1; ++i1) vn[i0] += m[i0][i1] * v[i1];
    }
}


void NS_MATRIX_ALGEBRA::Product(int size0, int size1, int size2, double ** m0, double** m1, double** m2) {
    for (int i0 = 0; i0 < size0; i0++) {
        for (int i2 = 0; i2 < size2; i2++) {
            m2[i0][i2] = 0.0;
            for (int i1 = 0; i1 < size1; i1++) m2[i0][i2] += m0[i0][i1] * m1[i1][i2];
        }
    }
}


void NS_MATRIX_ALGEBRA::Plus(int size0, double* v0, double* v1, double* v2) {
    for (int i = 0; i < size0; i++) v2[i] = v0[i] + v1[i];
}


void NS_MATRIX_ALGEBRA::Plus(int size0, int size1, double** m0, double** m1, double** m2) {
    for (int i0 = 0; i0 < size0; i0++) {
        for (int i1 = 0; i1 < size1; i1++) m2[i0][i1] = m0[i0][i1] + m1[i0][i1];
    }
}


void NS_MATRIX_ALGEBRA::Minus(int size0, double* v0, double* v1, double* v2) {
    for (int i = 0; i < size0; i++) v2[i] = v0[i] - v1[i];
}


void NS_MATRIX_ALGEBRA::Minus(int size0, int size1, double** m0, double** m1, double** m2) {
    for (int i0 = 0; i0 < size0; i0++) {
        for (int i1 = 0; i1 < size1; i1++) m2[i0][i1] = m0[i0][i1] - m1[i0][i1];
    }
}


void NS_MATRIX_ALGEBRA::Initialize2(int size0, int size1, double** &m) {
    m = new double* [size0];
    for (int i0 = 0; i0 < size0; ++i0) m[i0] = new double [size1];
}


void NS_MATRIX_ALGEBRA::Initialize3(int size0, int size1, int size2, double*** &m) {
    m = new double** [size0];
    for (int i0 = 0; i0 < size0; ++i0) NS_MATRIX_ALGEBRA::Initialize2(size1, size2, m[i0]);
}


void NS_MATRIX_ALGEBRA::Initialize4(int size0, int size1, int size2, int size3, double**** &m) {
    m = new double*** [size0];
    for (int i0 = 0; i0 < size0; ++i0) NS_MATRIX_ALGEBRA::Initialize3(size1, size2, size3, m[i0]);
}


void NS_MATRIX_ALGEBRA::Finalize2(int size0, int size1, double** m) {
    for (int i0 = 0; i0 < size0; ++i0) delete [] m[i0];
    delete [] m;
}


void NS_MATRIX_ALGEBRA::Finalize3(int size0, int size1, int size2, double*** m) {
    for (int i0 = 0; i0 < size0; ++i0) NS_MATRIX_ALGEBRA::Finalize2(size1, size2, m[i0]);
    delete [] m;
}


void NS_MATRIX_ALGEBRA::Finalize4(int size0, int size1, int size2, int size3, double**** m) {
    for (int i0 = 0; i0 < size0; ++i0) NS_MATRIX_ALGEBRA::Finalize3(size1, size2, size3, m[i0]);
    delete [] m;
}


void NS_MATRIX_ALGEBRA::Assignment(int size0, double* v0, double* v1) {
    for (int i = 0; i < size0; i++) v1[i] = v0[i];
}


void NS_MATRIX_ALGEBRA::Assignment(int size0, int size1, double** m0, double** m1) {
    for (int i0 = 0; i0 < size0; i0++) {
        for (int i1 = 0; i1 < size1; i1++) m1[i0][i1] = m0[i0][i1];
    }
}


void NS_MATRIX_ALGEBRA::Times(int size0, double* v0, double t, double* v1) {
    for (int i = 0; i < size0; i++) v1[i] = v0[i] * t;
}


void NS_MATRIX_ALGEBRA::Times(int size0, int size1, double** m0, double t, double** m1) {
    for (int i0 = 0; i0 < size0; i0++) {
        for (int i1 = 0; i1 < size1; i1++) m1[i0][i1] = t * m0[i0][i1];
    }
}


double NS_MATRIX_ALGEBRA::PowM(double **A, int N) {
    //if(N==1) return A[0][0];
    double *x = new double [N];
    double *x_new = new double [N];
    int t;
    int i, k;
    double r, r1, r2;
    double r_old;


    /* ステップ0 */
    t = 0;
    for (k = 0; k < N; k++)
        x[k] = 1.0;


    /* norlization of x */
    r1 = 0.0;
    for (int i = 0; i < N; i++) r1 += x[i] * x[i];
    r1 = 1.0 / sqrt(r1);
    for (int i = 0; i < N; i++) x[i] = x[i] * r1;


    /* ステップ1 */
step1:
    t = t + 1;
    /* new_x = A * x */
    for (i = 0; i < N; i++) {
        x_new[i] = 0;
        for (k = 0; k < N; k++)
            x_new[i] += A[i][k] * x[k];
    }
    /* ステップ2 */
    r1 = r2 = 0.0;
    for (k = 0; k < N; k++) r1 += x_new[k] * x[k];
    for (k = 0; k < N; k++) r2 += x[k] * x[k];
    r = r1 / r2;
    r2 = 0.0;
    for (k = 0; k < N; k++) r2 += x_new[k] * x_new[k];
    r2 = 1 / sqrt(r2);
    for (k = 0; k < N; k++) x[k] = x_new[k] * r2;
    if (t == 1) goto step1;
    if (fabs(r - r_old) < 10e-6) goto finish;
    if ( (r > pow(10., 8.)) || (t > 10000) ) {
        std::cout << "EXIT_FAILURE in PowM" << endl;
        std::cout << "r=" << r << endl;
        //exit(1);
        return r;
    }
    r_old = r;
    goto step1;
finish:
    delete x;
    delete x_new;
    return r;
}


void NS_MATRIX_ALGEBRA::Inverse(int size0, double** m_, double** &mi) {


    if (size0 == 1) {
        mi[0][0] = 1 / m_[0][0];
        return;
    }


    double a;
    double** m;
    NS_MATRIX_ALGEBRA::Initialize2(size0, size0, m);
    for (int i = 0; i < size0; i++) {
        for (int j = 0; j < size0; j++) {
            mi[i][j] = 0;
            m[i][j] = m_[i][j];
        }
    }
    for (int i = 0; i < size0; i++) {
        mi[i][i] = 1;
    }


    //gauss
    for (int i = 0; i < size0; i++) {
        if (m[i][i] == 0.0) {
            std::cout << "pivot=0!" << endl;
            //exit(1);
            return;
        }
        a = m[i][i];
        for (int k = 0; k < size0; k++) {
            mi[i][k] /= a;
            m[i][k] /= a;
        }
        for (int j = i + 1; j < size0; j++) {
            a = m[j][i];
            for (int k = 0; k < size0; k++) {
                mi[j][k] -= a * mi[i][k];
                m[j][k] -= a * m[i][k];
            }
        }
    }
}


double NS_MATRIX_ALGEBRA::Norm(int size0, int size1, double** m) {
    double n = 0.0;
    for (int i0 = 0; i0 < size0; i0++) {
        for (int i1 = 0; i1 < size1; i1++) {
            n += m[i0][i1] * m[i0][i1];
        }
    }
    return (sqrt(n));
}


double NS_MATRIX_ALGEBRA::Trace(int size0, double** m) {
    double t = 0.0;
    for (int i0 = 0; i0 < size0; i0++) t += m[i0][i0];
    return t;
}


void NS_MATRIX_ALGEBRA::PseudoInverse(int size0, int size1, double** A, double** &Ai) {
    double** B, ** newC, ** tempC, ** newCB, ** tempCB, ** I, ** temp;
    NS_MATRIX_ALGEBRA::Initialize2(size1, size1, B);
    NS_MATRIX_ALGEBRA::Initialize2(size1, size1, I);
    NS_MATRIX_ALGEBRA::Initialize2(size1, size1, newC);
    NS_MATRIX_ALGEBRA::Initialize2(size1, size1, tempC);
    NS_MATRIX_ALGEBRA::Initialize2(size1, size1, newCB);
    NS_MATRIX_ALGEBRA::Initialize2(size1, size1, tempCB);
    NS_MATRIX_ALGEBRA::Initialize2(size1, size1, temp);
    for (int i0 = 0; i0 < size1; i0++) {
        for (int i1 = 0; i1 < size1; i1++) {
            B[i0][i1] = 0.0;
            I[i0][i1] = 0.0;
            for (int i2 = 0; i2 < size0; i2++) {
                B[i0][i1] += A[i2][i0] * A[i2][i1];
            }
        }
        I[i0][i0] = 1.0;
    }
    NS_MATRIX_ALGEBRA::Assignment(size1, size1, I, newC);
    NS_MATRIX_ALGEBRA::Product(size1, size1, size1, newC, B, newCB);


    double NormCB = NS_MATRIX_ALGEBRA::Norm(size1, size1, newCB);


    int j = 1;
    while (NS_MATRIX_ALGEBRA::Norm(size1, size1, newCB) / NormCB > pow(10., -2.)) {
        NS_MATRIX_ALGEBRA::Assignment(size1, size1, newCB, tempCB);
        NS_MATRIX_ALGEBRA::Assignment(size1, size1, newC, tempC);


        NS_MATRIX_ALGEBRA::Times(size1, size1, I, NS_MATRIX_ALGEBRA::Trace(size1, newCB) / (double) j, temp);
        NS_MATRIX_ALGEBRA::Minus(size1, size1, temp, newCB, newC);
        NS_MATRIX_ALGEBRA::Product(size1, size1, size1, newC, B, newCB);
        j++;
        if ((j - size0 - 1)*(j - size1 - 1) == 0) break;
    }


    double r = (double) j - 1.0;
    double t = NS_MATRIX_ALGEBRA::Trace(size1, tempCB);
    t = r / t;
    for (int i0 = 0; i0 < size1; i0++) {
        for (int i1 = 0; i1 < size0; i1++) {
            Ai[i0][i1] = 0.0;
            for (int i2 = 0; i2 < size1; i2++) Ai[i0][i1] += tempC[i0][i2] * A[i1][i2];
            Ai[i0][i1] *= t;
        }
    }
    NS_MATRIX_ALGEBRA::Finalize2(size1, size1, B);
    NS_MATRIX_ALGEBRA::Finalize2(size1, size1, I);
    NS_MATRIX_ALGEBRA::Finalize2(size1, size1, newC);
    NS_MATRIX_ALGEBRA::Finalize2(size1, size1, tempC);
    NS_MATRIX_ALGEBRA::Finalize2(size1, size1, newCB);
    NS_MATRIX_ALGEBRA::Finalize2(size1, size1, tempCB);
    NS_MATRIX_ALGEBRA::Finalize2(size1, size1, temp);
}


 void NS_MATRIX_ALGEBRA::T(int size0, int size1, double** m, double** &Tm){
     for(int i0=0; i0<size0; i0++){
         for(int i1=0; i1<size1; i1++) Tm[i1][i0]=m[i0][i1];
     }
 }




#endif	/* MATRIX_ALGEBRA_HPP */
