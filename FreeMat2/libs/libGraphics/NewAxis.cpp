// Copyright (c) 2002-2005 Samit Basu
// 
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.

#include "NewAxis.hpp"
#include <math.h>
#include <iostream>
#include <string>

namespace FreeMat {
  static std::string TrimPrint(double val, bool scientificNotation) {
    char buffer[1000];
    char *p;
    if (!scientificNotation) {
      sprintf(buffer,"%f",val);
      p = buffer + strlen(buffer) - 1;
      while (*p == '0') {
	*p = 0;
	p--;
      }
      if ((*p == '.') || (*p == ',')) {
	*(p+1) = '0';
	*(p+2) = 0;
      }
      return std::string(buffer);
    } else {
      sprintf(buffer,"%e",val);
      std::string label(buffer);
      unsigned int ePtr;
      ePtr = label.size() - 1;
      while ((label[ePtr] != 'e') && (label[ePtr] != 'E'))
	ePtr--;
      ePtr--;
      while (label[ePtr] == '0') {
	label.erase(ePtr,1);
	ePtr--;
      }
      if ((label[ePtr] == '.') || (label[ePtr] == ','))
	label.insert(ePtr+1, 1,'0');
      ePtr = label.size() - 1;
      while ((label[ePtr] != 'e') && (label[ePtr] != 'E'))
	ePtr--;
      ePtr+=2;
      while ((label[ePtr] == '0') && ePtr < label.size()) {
	label.erase(ePtr,1);
      }
      if (ePtr == label.size())
	label.append("0");
      return label;
    }
  }

  NewAxis::NewAxis() {
    tMin = 0;
    tMax = 1;
    tBegin = 0;
    tEnd = 1;
    isLogarithmic = false;
    space = 10;
    tickLength = 5;
    axisLength = 200;
    manualmode = false;
  }

  int NewAxis::GetNominalTickCount() {
    return std::max(2,(int)(axisLength/50));
  }

  void NewAxis::SetAxisLength(int npix) {
    axisLength = npix;
    if (manualmode)
      ManualSetAxis(tStart,tStop);
    else
      AutoSetAxis();
  }

  void NewAxis::SetDataRange(double t1, double t2) {
    tMin = t1;
    tMax = t2;
    AutoSetAxis();
  }

  void NewAxis::ManualSetAxis(double t1, double t2) {
    manualmode = true;
    int m = GetNominalTickCount();
    double delt = (t2-t1)/m;
    int n = floor(log10(delt));
    double rdelt = delt/pow(10.0,(double)n);
    int p = ceil(log2(rdelt));
    double edelt = pow(10.0,(double) n)*pow(2.0,(double) p);
    tStart = t1;
    tStop = t2;
    tBegin = edelt*ceil(t1/edelt);
    tEnd = floor(t2/edelt)*edelt;
    int mprime;
    mprime = ceil((tEnd-tBegin)/edelt);
    if ((tBegin+mprime*edelt) > t2)
      mprime--;
    tCount = mprime;
    tickLocations.clear();
    tlabels.clear();
    for (int i=0;i<mprime;i++) {
      tickLocations.push_back(tBegin+i*edelt);
      tlabels.push_back(TrimPrint(tBegin+i*edelt,false));
    }
  }

  void NewAxis::GetIntervals(double &t1, double &t2, int &tn) {
    t1 = tBegin;
    t2 = tEnd;
    tn = tCount;
  }
 
  void NewAxis::AutoSetAxis() {
    manualmode = false;
    int m = GetNominalTickCount();
    double delt = (tMax-tMin)/m;
    int n = floor(log10(delt));
    double rdelt = delt/pow(10.0,(double)n);
    int p = ceil(log2(rdelt));
    double edelt = pow(10.0,(double) n)*pow(2.0,(double) p);
    tStart = floor(tMin/edelt)*edelt;
    tStop = ceil(tMax/edelt)*edelt;
    tBegin = tStart;
    tEnd = tStop;
    int mprime;
    mprime = ceil((tEnd-tBegin)/edelt);
    if ((tBegin+(mprime-1)*edelt) > tMax)
      mprime--;
    tCount = mprime;
    tickLocations.clear();
    tlabels.clear();
    for (int i=0;i<mprime;i++) {
      tickLocations.push_back(tBegin+i*edelt);
      tlabels.push_back(TrimPrint(tBegin+i*edelt,false));
    }
  }

  void NewAxis::SetLogarithmic(bool logarithmic) {
    isLogarithmic = logarithmic;
    AutoSetAxis();
  }

  void NewAxis::GetAxisExtents(double&t1, double& t2) {
    t1 = tStart;
    t2 = tEnd;
  }

  double NewAxis::Normalize(double t) {
    return (t-tStart)/(tEnd-tStart);
  }
}
