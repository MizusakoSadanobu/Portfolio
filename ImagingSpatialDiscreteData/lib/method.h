/* 
 * File:   method.h
 * Author: mizusako
 *
 * Created on 2014/11/16, 16:43
 */

#ifndef METHOD_H
#define	METHOD_H

class Method {
private:
    /*----------------- Lasso -----------------*/
    double MyNorm(int size0, int size1, double** m);
    double MyMax(int size0, int size1, double** x);
    void SetBeta();
    int Num;
    double*** PbetaOld;
    double*** Pbeta;
    double*** PError;
    double*** Pz;
    double*** Ptemp1;
    double*** Ptemp2;
    double*** Ptemp3;
    double*** PRi;
    double*** PTxx;
    double*** PTxy;
    double lambda;
    double eta;
    double df;
    double s;
    double rate;
    double ConvRate;
    /*-----------------------------------------*/
public:
    double*** PX;
    double*** PY;
    double*** PBeta;
    int W;
    int N;
    int P;
    int D;
    //

    Method() {
    }

    ~Method() {
    }
    //
    void OLS();
    void LASSO();
};

#endif	/* METHOD_H */

