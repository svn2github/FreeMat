#include <vector>

template <class T>
class SpString {
  int start_row;
  std::vector<T> data;
};

typedef std::vector<SpString> SpStringList;

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





