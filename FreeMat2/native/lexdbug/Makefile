all: newlex


Parser.o: Parser.cpp

NewLex.o: NewLex.cpp
	$(CXX) -c -o NewLex.o -g NewLex.cpp

Parser.cpp: Parser.yxx
	bison -d -o Parser.cpp Parser.yxx

newlex: NewLex.o AST.o Reserved.o
	$(CXX) -o newlex -g NewLex.o AST.o Reserved.o

clean:
	rm *.o

