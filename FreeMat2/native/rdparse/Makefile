all: parseplay treetest


CXXFLAGS=-g

OBJECTS = Tree.o Token.o Scanner.o Parser.o main.o

parseplay: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o parseplay $(OBJECTS)

clean:
	-rm $(OBJECTS)

TOBJECTS = treetest.o

treetest: $(TOBJECTS)
	$(CXX) $(CXXFLAGS) -o treetest $(TOBJECTS)
