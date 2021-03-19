/* 
 * File:   basic.hpp
 * Author: mizusako
 *
 * Created on 2014/11/16, 11:34
 */


#ifndef BASIC_HPP
#define	BASIC_HPP

#include "Basic_include_file_set.h"
#include "basic.h"


template <class T> void Read(string name, vector<T>& Tset) {
    ifstream in(name.c_str());
    if (!in) {
        cout << "error in reading" << endl;
        //exit(1);
    }
    int n;
    in >> n;
    Tset.resize(n);
    for (typename vector<T>::iterator i = Tset.begin(); i != Tset.end(); ++i) {
        in >> *i;
    }
    in.close();
}


template <class T> void Read(string name, vector<T>& Tset, int n) {
    ifstream in(name.c_str());
    if (!in) {
        cout << "error in reading" << endl;
        //exit(1);
    }
    Tset.resize(n);
    for (typename vector<T>::iterator i = Tset.begin(); i != Tset.end(); ++i) {
        in >> *i;
    }
    in.close();
}


void Read(string name, vector<vector<double> >& data, int n0, int n1) {
    ifstream in(name.c_str());
    if (!in) {
        cout << "error in reading" << endl;
        exit(1);
    }
    data.resize(n0);
    string temp;
    string strval;
    int ichar;
    for (int i0 = 0; i0 < n0; i0++) {
        data[i0].resize(n1);
        temp = "";
        in >> temp;
        int i = 0;
        for (int i1 = 0; i1 < n1; i1++) {
            strval = "";
            ichar = temp.at(i);
            while ((ichar != ',')&&(ichar != EOF)&&(ichar != '\n')) {
                strval += ichar;
                i++;
                if (i < temp.size()) {
                    ichar = temp.at(i);
                } else {
                    break;
                }
            }
            data[i0][i1] = atof(strval.c_str());
            //cout << data[i0][i1] << endl;
            i++;
        }
    }
    in.close();
}


void ExportData(string filename, double** data, int size0, int size1) {
    ofstream ofs(filename.c_str(), std::ios::out | std::ios::trunc);
    int i1, i0;
    for (i0 = 0; i0 < size0; i0++) {
        for (i1 = 0; i1 < size1 - 1; i1++) {
            ofs << data[i0][i1] << ",";
        }
        ofs << data[i0][i1] << endl;
    }
    ofs.close();
}


void ExportData(string filename, double* data, int size0) {
    ofstream ofs(filename.c_str(), std::ios::out | std::ios::trunc);
    for (int i0 = 0; i0 < size0; i0++) {
        ofs << data[i0] << endl;
    }
    ofs.close();
}


template <class T> void ExportData(string filename, vector<vector<T> > data, int size0, int size1) {
    ofstream ofs(filename.c_str(), std::ios::out | std::ios::trunc);
    int i1, i0;
    for (i0 = 0; i0 < size0; i0++) {
        for (i1 = 0; i1 < size1 - 1; i1++) {
            ofs << data[i0][i1] << ",";
        }
        ofs << data[i0][i1] << endl;
    }
    ofs.close();
}


template <class T> void ExportData(string filename, vector<T> data, int size0) {
    ofstream ofs(filename.c_str(), std::ios::out | std::ios::trunc);
    for (int i0 = 0; i0 < size0; i0++) {
        ofs << data[i0] << endl;
    }
    ofs.close();
}




#endif	/* BASIC_HPP */
