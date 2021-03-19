/* 
 * File:   element.hpp
 * Author: mizusako
 *
 * Created on 2014/11/16, 12:51
 */


#ifndef ELEMENT_HPP
#define	ELEMENT_HPP


#include "fft.hpp"
#include "cluster.hpp"
#include "element.h"
#include "Basic_include_file_set.h"


void Element::ComputeUw(vector<Cluster>& ClstArray) {


    /*--------------------- Detection of neighbor -------------------------*/


    int NOP = ClstArray.size();
    NS_CLUSTER::IDr* IDrList = new NS_CLUSTER::IDr[NOP];
    vector<double>* Px0 = this->Ppos;
    vector<double>* Px;


    for (int i = 0; i < NOP; i++) {
        IDrList[i].ID = i;
        Px = ClstArray[i].Ppos;
        double r = 0.;
        for (int d = 0; d < 3; d++) r += ((*Px0)[d]-(*Px)[d])*((*Px0)[d]-(*Px)[d]);
        IDrList[i].r = r;
    }
    std::sort(IDrList, IDrList + NOP);
    this->Pneighbor = &ClstArray[IDrList[0].ID];
    delete IDrList;


    double* Dx = new double[3];
    Px = ClstArray[ this->Pneighbor->id ].Ppos;
    for (int d = 0; d < 3; d++) {
        Dx[d] = ((*Px0)[d] - (*Px)[d]) / this->Pneighbor->Unit;
    }
    //
    /*------------------------------- generation of X ------------------------------*/
    int P = this->Pneighbor->P;
    int Deg = this->Pneighbor->Deg;
    this->X = new double[P + 1];
    int p = 0;
    for (int deg = 0; deg <= Deg; deg++) {
        for (int dx = 0; dx <= deg; dx++) {
            for (int dy = 0; dy <= deg - dx; dy++) {
                int dz = deg - dx - dy;
                this->X[p] = pow(Dx[0], dx) * pow(Dx[1], dy) * pow(Dx[2], dz);
                p++;
            }
        }
    }
    delete Dx;
    //
    int W = this->Pneighbor->W;
    int T = this->Pneighbor->T;
    vector<vector<double> >& uw = *(this->Puw);
    uw.resize(6);
    for (int d = 0; d < 6; d++) {
        uw[d].resize(T);
        for (int t = 0; t < T; t++) uw[d][t] = 0.;
    }
    for (int w = 0; w < W; w++) {
        for (int d = 0; d < 6; d++) {
            double temp = 0;
            for (int p = 0; p < P + 1; p++) {
                temp += X[p] * (this->Pneighbor->Beta)[w][p][d];
            }
            uw[d][w] = temp;
        }
    }
}


void Element::ComputeUt() {
    int T = this->Pneighbor->T;
    int W = this->Pneighbor->W;
    int iter = this->Pneighbor->iter;
    
    vector<vector<double> >& ut = *(this->Put);
    ut.resize(6);
    for (int d = 0; d < 6; d++) {
        ut[d].resize(T);
        for (int t = 0; t < T; t++) ut[d][t] = 0.;
    }
    /*-------------------- BANDPASS ----------------------*/
    for (int d = 0; d < 3; d++) {
        //(*(this->Puw))[2 * d][0] = 0.;
        //(*(this->Puw))[2 * d + 1][0] = 0.;
        for (int w = 1; w < W; w++) {
            ut[2 * d][T - w] = (*(this->Puw))[2 * d][w]; //Re
            ut[2 * d + 1][T - w] = -(*(this->Puw))[2 * d + 1][w]; //Im
        }
    }
    //
    /*----------------------- ifft -------------------------*/
    for (int d = 0; d < 6; d++) {
        copy((*(this->Puw))[d].begin(), (*(this->Puw))[d].end(), (*(this->Put))[d].begin());
    }
    for (int d = 0; d < 3; d++) {
        fft1((*(this->Put))[2 * d], (*(this->Put))[2 * d + 1], T, iter, 1);
    }
    //
}


#endif	/* ELEMENT_HPP */

