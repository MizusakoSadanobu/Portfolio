/* 
 * File:   cluster.hpp
 * Author: mizusako
 *
 * Created on 2014/11/16, 11:45
 */


#ifndef CLUSTER_HPP
#define	CLUSTER_HPP


#include "Basic_include_file_set.h"
#include "matrix_algebra.hpp"
#include "cluster.h"
#include "method.hpp"


struct NS_CLUSTER::IDr {
    int ID;
    double r;


    bool operator<(const IDr& right) const {
        return r < right.r;
    }


    bool operator>(const IDr& right) const {
        return r > right.r;
    }
};

void Cluster::ReadAnalyticalSolution(string name, vector<double>*& ut) {
    std::ifstream ifs(name.c_str());
    std::string str;
    double temp;
    getline(ifs, str);
    for (int t = 0; t<this->T; t++) {
        getline(ifs, str);
        std::replace(str.begin(), str.end(), ',', ' ');
        istringstream is(str);
        is >> temp;
        for (int d = 0; d < 3; d++) {
            is >> ut[2 * d][t];
        }
    }
}


void Cluster::ReadData(std::string name) {
    vector<double>*& ut = *(this->Put);
    ut = new vector<double>[6];
    for (int d = 0; d < 3; d++) {
        ut[2 * d].resize(this->T);
        ut[2 * d + 1].resize(this->T);
        for (int t = 0; t < this->T; t++) {
            ut[2 * d + 1][t] = 0.;
        }
    }

    this->ReadAnalyticalSolution(name, ut);
    
    vector<double>*& uw = *(this->Puw);
    uw = new vector<double>[6];
    for (int d = 0; d < 6; d++) {
        uw[d].resize(T);
        copy(ut[d].begin(), ut[d].end(), uw[d].begin());
    }
    for (int d = 0; d < 3; d++) {
        fft1(uw[2 * d], uw[2 * d + 1], T, iter, 0);
    }
}


void Cluster::MakeCluster() {
    int N = this->N;
    int W = this->W;
    int Deg = this->Deg;
    this->P = Deg * (2 * Deg * Deg + 12 * Deg + 22) / 12;
    int P = this->P;
    NS_MATRIX_ALGEBRA::Initialize2(N, P, this->X);
    NS_MATRIX_ALGEBRA::Initialize3(W, N, 6, this->Y);
    int NOP = this->PClstArray->size();


    NS_CLUSTER::IDr* IDrList = new NS_CLUSTER::IDr[NOP];
    vector<double>* Px0 = this->Ppos;
    vector<double>* Px;


    for (int i = 0; i < NOP; i++) {
        IDrList[i].ID = i;
        Px = (*this->PClstArray)[i].Ppos;
        double r = 0.;
        for (int d = 0; d < 3; d++) r += ((*Px0)[d]-(*Px)[d])*((*Px0)[d]-(*Px)[d]);
        IDrList[i].r = r;
    }
    std::sort(IDrList, IDrList + NOP);


    double** Dx;
    NS_MATRIX_ALGEBRA::Initialize2(N, 3, Dx);
    for (int n = 0; n < N; n++) {
        Px = (*this->PClstArray)[ IDrList[n].ID ].Ppos;
        for (int d = 0; d < 3; d++) {
            Dx[n][d] = ((*Px)[d] - (*Px0)[d]) / this->Unit;
        }
    }


    /*------------------------------- generation of X ------------------------------*/
    for (int n = 0; n < N; n++) {
        int p = 0;
        for (int deg = 1; deg <= Deg; deg++) {
            for (int dx = 0; dx <= deg; dx++) {
                for (int dy = 0; dy <= deg - dx; dy++) {
                    int dz = deg - dx - dy;
                    this->X[n][p] = pow(Dx[n][0], dx) * pow(Dx[n][1], dy) * pow(Dx[n][2], dz);
                    p++;
                }
            }
        }
    }


    NS_MATRIX_ALGEBRA::Finalize2(N, 3, Dx);


    /*----------------------------- generation of Y ---------------------------------*/
    vector<double>** Puw0;
    for (int n = 0; n < N; n++) {
        Puw0 = (*this->PClstArray)[ IDrList[n].ID ].Puw;
        for (int d = 0; d < 6; d++) {
            for (int w = 0; w < W; w++) {
                this->Y[w][n][d] = (*Puw0)[d][w];
            }
        }
    }
    delete IDrList;
}


void Cluster::ComputeBeta(int flag) {
    int W = this->W;
    Method method;
    this->Beta = new double** [W];
    method.PX = &(this->X);
    method.D = 6;
    method.N = this->N;
    method.P = this->P;
    method.W = W;


    for (int w = 0; w < W; w++) {
        method.PY = &(this->Y[w]);
        method.PBeta = &(this->Beta[w]);
        switch (flag) {
            case 0:
                method.OLS();
                break;
            case 1:
                method.LASSO();
                break;
        }
    }
    //NS_MATRIX_ALGEBRA::Finalize2(N, P, this->X);
    //NS_MATRIX_ALGEBRA::Finalize3(W, N, 6, this->Y);
}




#endif	/* CLUSTER_HPP */
