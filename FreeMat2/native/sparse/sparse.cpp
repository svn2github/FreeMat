#include <vector>
#include <iostream>
#include <stdlib.h>

template <class T>
class SpString {
 public:
  int start_row;
  std::vector<T> data;
};

template <class T>
class SpMatrix {
  int m_rows;
  int m_cols;
  std::vector<SpString<T> > *colstrings;
 public:
  // Null constructor
  SpMatrix();
  // Empty constructor
  SpMatrix(int rows, int cols);
  // Construct a sparse matrix from a dense one
  SpMatrix(T *data, int rows, int cols);
  // Destructor
  ~SpMatrix();
  // Print me
  void PrintMe(std::ostream &o);
  // Print me in native form
  void PrintMeNative(std::ostream &o);
};

template <class T>
SpMatrix<T>::SpMatrix() {
  m_rows = 0;
  m_cols = 0;
  colstrings = NULL;
}

template <class T>
SpMatrix<T>::~SpMatrix() {
  delete[] colstrings;
}

template <class T>
SpMatrix<T>::SpMatrix(int rows, int cols) {
  m_rows = rows;
  m_cols = cols;
  colstrings = new std::vector<SpString<T> >[cols];
}

template <class T>
SpMatrix<T>::SpMatrix(T *data, int rows, int cols)  {
  m_rows = rows;
  m_cols = cols;
  colstrings = new std::vector<SpString<T> >[cols];
  int i, j, p;
  bool newstring;
  SpString<T> wstring;
  p = 0;
  for (i=0;i<cols;i++) {
    newstring = true;
    wstring.data.clear();
    for (j=0;j<rows;j++) {
      if ((data[p] == 0) && !newstring) {
	// add wstring to the current column's set of strings
	colstrings[i].push_back(wstring);
	newstring = true;
	wstring.data.clear();
      } else if ((data[p] != 0) && newstring) {
	wstring.start_row = j;
	wstring.data.push_back(data[p]);
	newstring = false;
      } else if (data[p] != 0) {
	wstring.data.push_back(data[p]);	
      }
      p++;
    }
    if (!newstring)
      colstrings[i].push_back(wstring);
  }    
}

template <class T>
void SpMatrix<T>::PrintMe(std::ostream &o) {
  
}

template <class T>
void SpMatrix<T>::PrintMeNative(std::ostream &o) {
  for (int i=0;i<m_cols;i++) {
    o << "Strings for column " << i << "\n";
    for (int j=0;j<colstrings[i].size();j++) {
      o << "    start row = " << colstrings[i][j].start_row << "\n";
      o << "    data = ";
      for (int k=0;k<colstrings[i][j].data.size();k++) {
	o << colstrings[i][j].data[k] << ",";
      }
      o << "\n";
    }
  }
}

#define TSIZE 5000
int main(int argc, char *argv[]) {
  // N = [0, 3, 4, 0, 2]
  //     [1, 0, 3, 0, 0]
  //     [2, 7, 2, 1, 0]
  float N[15] = {0,1,2,3,0,7,4,3,2,0,0,1,2,0,0};
  SpMatrix<float> A(N,3,5);
  A.PrintMeNative(std::cout);
  float *M;
  std::cout << "Allocating array\n";
  M = new float[TSIZE*TSIZE];
  int i, j;
  std::cout << "Filling array...\n";
  for (i=0;i<TSIZE;i++)
    for (j=0;j<TSIZE;j++) {
      double useelt = ((double) random())/RAND_MAX;
      if (useelt < 0.1)
	M[i+TSIZE*j] = useelt;
    }
  std::cout << "Converting array...\n";
  SpMatrix<float> B(M,TSIZE,TSIZE);
  std::cout << "Done.\n";
  
  return 0;

}


