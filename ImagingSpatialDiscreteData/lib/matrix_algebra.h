/* 
 * File:   matrix_algebra.h
 * Author: mizusako
 *
 * Created on 2014/11/16, 11:36
 */

#ifndef MATRIX_ALGEBRA_H
#define	MATRIX_ALGEBRA_H

//////////////////////////////////////////////////////////////////////
// namespace NS_MATRIX_ALGEBRA                                      //
//////////////////////////////////////////////////////////////////////
namespace NS_MATRIX_ALGEBRA {
    double Product(int size0, double* v0, double* v1);
    void Product(int size0, int size1, double** m, double* v, double* vn);
    void Product(int size0, int size1, int size2, double ** m0, double** m1, double** m2);
    void Plus(int size0, double* v0, double* v1, double* v2);
    void Plus(int size0, int size1, double** m0, double** m1, double** m2);
    void Minus(int size0, double* v0, double* v1, double* v2);
    void Minus(int size0, int size1, double** m0, double** m1, double** m2);
    void Initialize2(int size0, int size1, double** &m);
    void Initialize3(int size0, int size1, int size2, double*** &m);
    void Initialize4(int size0, int size1, int size2, int size3, double**** &m);
    void Finalize2(int size0, int size1, double** m);
    void Finalize3(int size0, int size1, int size2, double*** m);
    void Finalize4(int size0, int size1, int size2, int size3, double**** m);
    void Assignment(int size0, double* v0, double*v1);
    void Assignment(int size0, int size1, double** m0, double** m1);
    void Times(int size0, double* v0, double t, double* v1);
    void Times(int size0, int size1, double** m0, double t, double** m1);
    double PowM(double **A, int N);
    void Inverse(int size0, double** m_, double** &mi);
    void PseudoInverse(int size0, int size1, double** A, double** &Ai);
    double Trace(int size0, double** m);
    double Norm(int size0, int size1, double** m);
    void T(int size0, int size1, double** m, double** &Tm);
}
#endif	/* MATRIX_ALGEBRA_H */

