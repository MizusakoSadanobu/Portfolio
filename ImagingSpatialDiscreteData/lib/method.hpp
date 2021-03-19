/* 
 * File:   method.hpp
 * Author: mizusako
 *
 * Created on 2014/11/16, 16:43
 */


#ifndef METHOD_HPP
#define	METHOD_HPP


#include "Basic_include_file_set.h"
#include "matrix_algebra.hpp"
#include "method.h"
#include "cluster.h"


double Method::MyNorm(int size0, int size1, double** m) {
    double s = 0.0;
    for (int i0 = 0; i0 < size0; i0++) {
        for (int i1 = 0; i1 < size1; i1++) s += m[i0][i1] * m[i0][i1];
    }
    return sqrt(s);
}


double Method::MyMax(int size0, int size1, double** x) {
    double Max = fabs(x[0][0]);
    double Abs;
    for (int i0 = 0; i0 < size0; i0++) {
        for (int i1 = 0; i1 < size1; i1++) {
            Abs = fabs(x[i0][i1]);
            if (Max < Abs) {
                Max = Abs;
            }
        }
    }
    return (Max);
}


void Method::SetBeta() {


    double**& beta = *(this->Pbeta);
    double**& betaOld = *(this->PbetaOld);
    //double**& Error = *(this->PError);
    double**& z = *(this->Pz);
    double**& temp1 = *(this->Ptemp1);
    double**& temp2 = *(this->Ptemp2);
    double**& temp3 = *(this->Ptemp3);
    double**& Ri = *(this->PRi);
    double**& Txx = *(this->PTxx);
    double**& Txy = *(this->PTxy);
    double& eta = this->eta;
    double& lambda = this->lambda;
    double& df = this->df;
    double& s = this->s;
    double& rate = this->rate;
    int P = this->P;
    int D = this->D;
    NS_MATRIX_ALGEBRA::Assignment(P, D, beta, betaOld);
    NS_MATRIX_ALGEBRA::Product(P, P, D, Txx, z, temp1); //temp1=Tx.x.z
    NS_MATRIX_ALGEBRA::Product(P, D, D, temp1, Ri, temp2); //temp2=Tx.x.z.Ri
    NS_MATRIX_ALGEBRA::Times(P, D, temp2, eta, temp1); //temp1=eta*Tx.x.z.Ri
    NS_MATRIX_ALGEBRA::Minus(P, D, z, temp1, temp2); //temp2=z-eta*Tx.x.z.Ri
    NS_MATRIX_ALGEBRA::Product(P, D, D, Txy, Ri, temp1); //temp1=Tx.y.Ri
    NS_MATRIX_ALGEBRA::Times(P, D, temp1, eta, temp1); //temp1=eta*Tx.y.Ri
    NS_MATRIX_ALGEBRA::Plus(P, D, temp1, temp2, temp3); //temp3=z + eta * Tx.(y-x.z).Ri
    df = 0.;
    for (int p = 0; p < P; p++) {
        for (int d = 0; d < D; d++) {
            if (temp3[p][d] < -lambda * eta) {
                df = df + 1.;
                beta[p][d] = (temp3[p][d] + lambda * eta);
            } else if (temp3[p][d] > lambda * eta) {
                df = df + 1.;
                beta[p][d] = (temp3[p][d] - lambda * eta);
            } else {
                beta[p][d] = 0.;
            }
        }
    }


    double snew = (1.0 + sqrt(1.0 + 4.0 * s * s)) * 0.5;
    NS_MATRIX_ALGEBRA::Minus(P, D, beta, betaOld, temp1);
    NS_MATRIX_ALGEBRA::Times(P, D, temp1, (s - 1.0) / snew, temp2);
    NS_MATRIX_ALGEBRA::Plus(P, D, beta, temp2, z);
    s = snew;


    double norm = this->MyNorm(P, D, beta);
    if (norm > pow(10., -10)) {
        rate = this->MyNorm(P, D, temp1) / norm;
    } else {
        rate = 0.;
    }
}


void Method::LASSO() {
    int& Num = this->Num;
    Num = 50;
    double& eta = this->eta;
    double& lambda = this->lambda;
    double& df = this->df;
    double& s = this->s;
    double& rate = this->rate;
    double& ConvRate = this->ConvRate;
    ConvRate = pow(10., -4);
    //
    int D = this->D;
    int N = this->N;
    int P = this->P;
    double**& Beta = *(this->PBeta);
    double**& X = *(this->PX);
    double**& Y = *(this->PY);
    double** x;
    double** y;
    double** beta;
    this->Pbeta = &beta;


    double* ax = new double[P];
    double* sx = new double[P];
    double* ay = new double[D];
    NS_MATRIX_ALGEBRA::Initialize2(P + 1, D, Beta);
    NS_MATRIX_ALGEBRA::Initialize2(P, D, beta);
    NS_MATRIX_ALGEBRA::Initialize2(N, P, x);
    NS_MATRIX_ALGEBRA::Initialize2(N, D, y);
    for (int p = 0; p < P; p++) {
        for (int d = 0; d < D; d++) beta[p][d] = 0.;
    }




    double a;
    for (int p = 0; p < P; p++) {
        a = 0;
        for (int n = 0; n < N; n++) a += X[n][p];
        a /= (double) N;
        ax[p] = a;
    }
    double dev;
    for (int p = 0; p < P; p++) {
        dev = 0;
        for (int n = 0; n < N; n++) {
            x[n][p] = X[n][p] - ax[p];
            dev += x[n][p] * x[n][p];
        }
        dev /= (double) N;
        dev = sqrt(dev);
        for (int n = 0; n < N; n++) x[n][p] /= dev;
        sx[p] = dev;
    }


    for (int d = 0; d < D; d++) {
        a = 0;
        for (int n = 0; n < N; n++) a += Y[n][d];
        a /= (double) N;
        ay[d] = a;
    }
    for (int d = 0; d < D; d++) {
        for (int n = 0; n < N; n++) y[n][d] = Y[n][d] - ay[d];
    }




    double** temp1;
    double** temp2;
    double** temp3;
    this->Ptemp1 = &temp1;
    this->Ptemp2 = &temp2;
    this->Ptemp3 = &temp3;
    NS_MATRIX_ALGEBRA::Initialize2(P, D, temp1);
    NS_MATRIX_ALGEBRA::Initialize2(P, D, temp2);
    NS_MATRIX_ALGEBRA::Initialize2(P, D, temp3);


    double** R;
    double** Ri;
    double** Tx;
    double** Txx;
    double** Txy;
    double** betaOld;
    double** z;
    this->PRi = &Ri;
    this->PTxx = &Txx;
    this->PTxy = &Txy;
    this->PbetaOld = &betaOld;
    this->Pz = &z;
    NS_MATRIX_ALGEBRA::Initialize2(D, D, R);
    NS_MATRIX_ALGEBRA::Initialize2(D, D, Ri);
    NS_MATRIX_ALGEBRA::Initialize2(P, N, Tx);
    NS_MATRIX_ALGEBRA::Initialize2(P, P, Txx);
    NS_MATRIX_ALGEBRA::Initialize2(P, D, Txy);
    NS_MATRIX_ALGEBRA::Initialize2(P, D, betaOld);
    NS_MATRIX_ALGEBRA::Initialize2(P, D, z);
    for (int d0 = 0; d0 < D; d0++) {
        for (int d1 = 0; d1 < D; d1++) {
            R[d0][d1] = 0.;
        }
    }
    for (int d = 0; d < D; d++) R[d][d] = 1.;
    NS_MATRIX_ALGEBRA::Inverse(D, R, Ri);
    NS_MATRIX_ALGEBRA::T(N, P, x, Tx);
    NS_MATRIX_ALGEBRA::Product(P, N, P, Tx, x, Txx);
    NS_MATRIX_ALGEBRA::Product(P, N, D, Tx, y, Txy);


    NS_MATRIX_ALGEBRA::PowM(Txx, P);
    eta = 1. / NS_MATRIX_ALGEBRA::PowM(Ri, D) / NS_MATRIX_ALGEBRA::PowM(Txx, P);
    double lambda0 = this->MyMax(P, D, Txy);


    for (int num = 0; num < Num; num++) {
        rate = 1.;
        lambda = lambda0 * pow(0.001, num / 200.);
        NS_MATRIX_ALGEBRA::Assignment(P, D, beta, z);
        while (rate > ConvRate) this->SetBeta();
    }






    NS_MATRIX_ALGEBRA::Finalize2(P, D, temp1);
    NS_MATRIX_ALGEBRA::Finalize2(P, D, temp2);
    NS_MATRIX_ALGEBRA::Finalize2(P, D, temp3);


    NS_MATRIX_ALGEBRA::Finalize2(D, D, R);
    NS_MATRIX_ALGEBRA::Finalize2(D, D, Ri);
    NS_MATRIX_ALGEBRA::Finalize2(P, D, Tx);
    NS_MATRIX_ALGEBRA::Finalize2(P, P, Txx);
    NS_MATRIX_ALGEBRA::Finalize2(P, D, Txy);
    NS_MATRIX_ALGEBRA::Finalize2(P, D, betaOld);
    NS_MATRIX_ALGEBRA::Finalize2(P, D, z);


    for (int p = 0; p < P; p++) {
        for (int d = 0; d < D; d++) Beta[p + 1][d] = beta[p][d] / sx[p];
    }
    for (int d = 0; d < D; d++) {
        Beta[0][d] = ay[d];
        for (int p = 0; p < P; p++) Beta[0][d] -= ax[p] * Beta[p + 1][d];
    }


    NS_MATRIX_ALGEBRA::Finalize2(P, D, beta);
    NS_MATRIX_ALGEBRA::Finalize2(N, P, x);
    NS_MATRIX_ALGEBRA::Finalize2(N, D, y);
    delete ax;
    delete sx;
    delete ay;
}


void Method::OLS() {
    int D = this->D;
    int N = this->N;
    int P = this->P;
    double**& Beta = *(this->PBeta);
    double**& X = *(this->PX);
    double**& Y = *(this->PY);
    double** x;
    double** y;
    double** beta;


    double* ax = new double[P];
    double* sx = new double[P];
    double* ay = new double[D];
    NS_MATRIX_ALGEBRA::Initialize2(P + 1, D, Beta);
    NS_MATRIX_ALGEBRA::Initialize2(P, D, beta);
    NS_MATRIX_ALGEBRA::Initialize2(N, P, x);
    NS_MATRIX_ALGEBRA::Initialize2(N, D, y);


    double a;
    for (int p = 0; p < P; p++) {
        a = 0;
        for (int n = 0; n < N; n++) a += X[n][p];
        a /= (double) N;
        ax[p] = a;
    }
    double s;
    for (int p = 0; p < P; p++) {
        s = 0;
        for (int n = 0; n < N; n++) {
            x[n][p] = X[n][p] - ax[p];
            s += x[n][p] * x[n][p];
        }
        s /= (double) N;
        s = sqrt(s);
        for (int n = 0; n < N; n++) x[n][p] /= s;
        sx[p] = s;
    }


    for (int d = 0; d < D; d++) {
        a = 0;
        for (int n = 0; n < N; n++) a += Y[n][d];
        a /= (double) N;
        ay[d] = a;
    }
    for (int d = 0; d < D; d++) {
        for (int n = 0; n < N; n++) y[n][d] = Y[n][d] - ay[d];
    }


    double** xi;
    NS_MATRIX_ALGEBRA::Initialize2(P, N, xi);
    NS_MATRIX_ALGEBRA::PseudoInverse(N, P, x, xi);
    NS_MATRIX_ALGEBRA::Product(P, N, D, xi, y, beta);


    for (int p = 0; p < P; p++) {
        for (int d = 0; d < D; d++) Beta[p + 1][d] = beta[p][d] / sx[p];
    }
    for (int d = 0; d < D; d++) {
        Beta[0][d] = ay[d];
        for (int p = 0; p < P; p++) Beta[0][d] -= ax[p] * Beta[p + 1][d];
    }


    NS_MATRIX_ALGEBRA::Finalize2(P, D, beta);
    NS_MATRIX_ALGEBRA::Finalize2(N, P, x);
    NS_MATRIX_ALGEBRA::Finalize2(N, D, y);
    delete ax;
    delete sx;
    delete ay;
}


#endif	/* METHOD_HPP */
