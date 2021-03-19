/* 
 * File:   cluster.h
 * Author: mizusako
 *
 * Created on 2014/11/16, 11:45
 */

#ifndef CLUSTER_H
#define	CLUSTER_H

class Cluster {
    void ReadAnalyticalSolution(std::string name, std::vector<double>*& ut);
    void ReadMeSOnet(std::string name, std::vector<double>*& ut);
    //...
public:
    int id;
    int P; //number of unknown parameters
    int N; //number of points in cluster
    int Deg; //truncation order of Taylor expansion
    int W; //bandpass
    int T;
    int iter;
    double Unit;
    std::vector<double>* Ppos; //position of center of Taylor expansion
    std::vector<double>** Put;
    std::vector<double>** Puw;
    std::vector<Cluster>* PClstArray;
    /*----- Y=X.Beta -------*/
    double** X;
    double*** Y;
    double*** Beta;
    //...

    Cluster() {
    }

    ~Cluster() {
    }
    //...
    void ReadData(std::string name);
    void MakeCluster();
    void ComputeBeta(int flag);
};


namespace NS_CLUSTER {
    struct IDr;
};


#endif	/* CLUSTER_H */