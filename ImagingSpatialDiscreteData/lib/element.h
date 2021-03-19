/* 
 * File:   element.h
 * Author: mizusako
 *
 * Created on 2014/11/16, 11:54
 */

#ifndef ELEMENT_H
#define	ELEMENT_H

class Element {
public:
    int id;
    std::vector<double>* Ppos; //position of element
    vector<vector<double> >* Put;
    vector<vector<double> >* Puw;
    Cluster* Pneighbor;
    double* X;
    //

    Element() {
    }

    ~Element() {
    }
    //
    void ComputeUw(std::vector<Cluster>& ClstArray);
    void ComputeUt();
};

#endif	/* ELEMENT_H */