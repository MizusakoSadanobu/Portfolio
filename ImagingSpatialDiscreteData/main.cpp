/* 
 * File:   main.cpp
 * Author: mizusako
 *
 * Created on 2014/11/16, 11:26
 */
#include "Basic_include_file_set.h"
#include "basic.hpp"
#include "matrix_algebra.hpp"
#include "fft.hpp"
#include "cluster.hpp"
#include "element.hpp"

/*
 * 
 */
int main(int argc, char** argv) {

    int flag = 0; //choice of machine learning method 0:OLS, 1:LASSO
    int N = 10; //cluster size
    int Deg = 2; //truncation order of Taylor expansion
    int NOP = 220; //number of observation points
    int iter = 10;
    int T = pow(2, iter); //time window
    int W = T/2; //bandpass filter
    double Unit = 1000.;
    string posFile = "./input/position.csv";
    string utDir = "./input/ut/";
    string gridFile = "./input/grid.csv";
    string imageDir = "./output/";
    vector<vector<double> > posData;
    Read(posFile, posData, 220, 3);
    vector<double>* utList[NOP], * uwList[NOP];
    vector<Cluster> ClstArray(NOP);

    /*--------------- Initialization of cluster --------------*/
    for (int i = 0; i < NOP; i++) {
        Cluster& clst = ClstArray[i];
        clst.id = i;
        clst.Deg = Deg;
        clst.N = N;
        clst.W = W;
        clst.T = T;
        clst.iter = iter;
        clst.Unit = Unit;
        clst.Ppos = &posData[i];
        clst.Put = &utList[i];
        clst.Puw = &uwList[i];
        clst.PClstArray = &ClstArray;

        stringstream utFile;
        utFile.str("");
        utFile << utDir << i << ".csv";
        clst.ReadData(utFile.str());
    }
    cout << "finish initialization of cluster" << endl;

    /*------------Make cluster & compute beta---------------*/
    for (int i = 0; i < NOP; i++) {
        ClstArray[i].MakeCluster();
        ClstArray[i].ComputeBeta(flag);
        //cout << "id = " << i << endl;
    }
    cout << "finish making cluster & computing beta" << endl;

    /*----------------- calcuration of ut at the elements --------------------*/
    vector<vector<double> > gridPos;
    int nx = 120;
    int ny = 120;
    vector<vector<double> >* uwGrids = new vector<vector<double> >[nx * ny];
    vector<vector<double> >* utGrids = new vector<vector<double> >[nx * ny];
    Read(gridFile, gridPos, nx * ny, 3);
    Element* ElmList = new Element[nx * ny];
    for (int i = 0; i < nx * ny; i++) {
        Element& elm = ElmList[i];
        elm.id = i;
        elm.Ppos = &gridPos[i];
        elm.Put = &utGrids[i];
        elm.Puw = &uwGrids[i];
        elm.ComputeUw(ClstArray);
        elm.ComputeUt();
    }
    cout << "finish calcuration of ut at all the elements" << endl;

    /*----------------- transformation to image data --------------------*/
    double**** Images;
    NS_MATRIX_ALGEBRA::Initialize4(3, T, nx * ny, 3, Images);
    for (int i = 0; i < nx * ny; i++) {
        for (int d = 0; d < 3; d++) {
            for (int t = 0; t < T; t++) {
                Images[d][t][i][0] = gridPos[i][0];
                Images[d][t][i][1] = gridPos[i][1];
                Images[d][t][i][2] = utGrids[i][2 * d][t];
            }
        }
    }
    cout << "finish transformation to image data" << endl;

    /*----------------- save --------------------*/
    for (int t = 0; t < 400; t++) {
        stringstream imageFile;
        imageFile.str("");
        imageFile << imageDir << t << ".csv";
        ExportData(imageFile.str(), Images[0][t], nx * ny, 3);
    }
    NS_MATRIX_ALGEBRA::Finalize4(3, T, nx * ny, 3, Images);
    cout << "finish saving data" << endl;

    return 0;
}

