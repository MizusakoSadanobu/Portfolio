/* 
 * File:   fft.hpp
 * Author: mizusako
 *
 * Created on 2014/11/16, 11:42
 */




#ifndef FFT_HPP
#define	FFT_HPP




/*		fft.c		*/
#include <stdio.h>
#include <stdlib.h>
#include "Basic_include_file_set.h"
#include "fft.h"
/*-------fft flag=0, inverse_fft flag=1--------*/
void fft1(double ar[], double ai[], int n, int iter, int flag) {
    int i, it, j, j1, j2, k, xp, xp2;
    double arg, dr1, dr2, di1, di2, tr, ti, w, wr, wi;




    if (n < 2) {
        fprintf(stderr, "Error : n < 2  in fft1()\n");
        return;
    }
    if (iter <= 0) {
        iter = 0;
        i = n;
        while ((i /= 2) != 0) iter++;
    }
    j = 1;
    for (i = 0; i < iter; i++) j *= 2;
    if (n != j) {
        fprintf(stderr, "Error : n != 2 ^ k  in fft1()\n");
        return;
    }
    w = (flag ? M_PI : -M_PI) / (double) n;
    xp2 = n;
    for (it = 0; it < iter; it++) {
        xp = xp2;
        xp2 /= 2;
        w *= 2;
        for (k = 0, i = -xp; k < xp2; i++) {
            wr = cos(arg = w * k++);
            wi = sin(arg);
            for (j = xp; j <= n; j += xp) {
                j2 = (j1 = j + i) + xp2;
                tr = (dr1 = ar[j1]) - (dr2 = ar[j2]);
                ti = (di1 = ai[j1]) - (di2 = ai[j2]);
                ar[j1] = dr1 + dr2;
                ai[j1] = di1 + di2;
                ar[j2] = tr * wr - ti * wi;
                ai[j2] = ti * wr + tr * wi;
            }
        }
    }
    j = j1 = n / 2;
    j2 = n - 1;
    for (i = 1; i < j2; i++) {
        if (i < j) {
            w = ar[i];
            ar[i] = ar[j];
            ar[j] = w;
            w = ai[i];
            ai[i] = ai[j];
            ai[j] = w;
        }
        k = j1;
        while (k <= j) {
            j -= k;
            k /= 2;
        }
        j += k;
    }
    if (flag == 0) return;
    w = 1. / (double) n;
    for (i = 0; i < n; i++) {
        ar[i] *= w;
        ai[i] *= w;
    }
    return;
}




void fft1(vector<double> &ar, vector<double> &ai, int n, int iter, int flag) {
    int i, it, j, j1, j2, k, xp, xp2;
    double arg, dr1, dr2, di1, di2, tr, ti, w, wr, wi;




    if (n < 2) {
        fprintf(stderr, "Error : n < 2  in fft1()\n");
        return;
    }
    if (iter <= 0) {
        iter = 0;
        i = n;
        while ((i /= 2) != 0) iter++;
    }
    j = 1;
    for (i = 0; i < iter; i++) j *= 2;
    if (n != j) {
        fprintf(stderr, "Error : n != 2 ^ k  in fft1()\n");
        return;
    }
    w = (flag ? M_PI : -M_PI) / (double) n;
    xp2 = n;
    for (it = 0; it < iter; it++) {
        xp = xp2;
        xp2 /= 2;
        w *= 2;
        for (k = 0, i = -xp; k < xp2; i++) {
            wr = cos(arg = w * k++);
            wi = sin(arg);
            for (j = xp; j <= n; j += xp) {
                j2 = (j1 = j + i) + xp2;
                tr = (dr1 = ar[j1]) - (dr2 = ar[j2]);
                ti = (di1 = ai[j1]) - (di2 = ai[j2]);
                ar[j1] = dr1 + dr2;
                ai[j1] = di1 + di2;
                ar[j2] = tr * wr - ti * wi;
                ai[j2] = ti * wr + tr * wi;
            }
        }
    }
    j = j1 = n / 2;
    j2 = n - 1;
    for (i = 1; i < j2; i++) {
        if (i < j) {
            w = ar[i];
            ar[i] = ar[j];
            ar[j] = w;
            w = ai[i];
            ai[i] = ai[j];
            ai[j] = w;
        }
        k = j1;
        while (k <= j) {
            j -= k;
            k /= 2;
        }
        j += k;
    }
    if (flag == 0) return;
    w = 1. / (double) n;
    for (i = 0; i < n; i++) {
        ar[i] *= w;
        ai[i] *= w;
    }
    return;
}




#endif	/* FFT_HPP */
