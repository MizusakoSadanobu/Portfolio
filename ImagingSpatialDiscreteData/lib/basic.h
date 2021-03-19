/* 
 * File:   basic.h
 * Author: mizusako
 *
 * Created on 2014/11/16, 11:33
 */

#ifndef BASIC_H
#define	BASIC_H

template <class T> void Read(std::string name, std::vector<T>& Tset);
template <class T> void Read(std::string name, std::vector<T>& Tset, int n);
void Read(std::string name, std::vector<std::vector<double> >& data, int n0, int n1);

void ExportData(std::string filename, double** data, int size0, int size1);
void ExportData(std::string filename, double* data, int size0);
template <class T> void ExportData(std::string filename, std::vector<std::vector<T> > data, int size0, int size1);
template <class T> void ExportData(std::string filename, std::vector<T> data, int size0);

#endif	/* BASIC_H */

