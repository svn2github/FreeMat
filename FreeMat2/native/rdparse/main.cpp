#include "Scanner.hpp"
#include "Parser.hpp"
#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>


int main(int argc, char *argv[]) {
  FILE *fp = fopen(argv[1],"r");
  if (!fp) return 0;
  clearerr(fp);
  struct stat st;
  fstat(fileno(fp),&st);
  long cpos = st.st_size;
  // Allocate enough for the text, an extra newline, and null
  char *buffer = (char*) calloc(cpos+2,sizeof(char));
  int n = fread(buffer,sizeof(char),cpos,fp);
  buffer[n]='\n';
  buffer[n+1]=0;
  Scanner S(buffer);
  Parser G(S);
  try {
    tree a = G.Process();
    a.print();
  } catch(string s) {
    cout << "Error: " << s << "\n";
    cout << "Somewhere here:\n";
  } catch(ParseException &e) {
    cout << S.Context(G.LastPos());
    cout << "  Error: " << G.LastErr() << " at " << G.LastPos() << "\n";
  }
  return 0;
}
