// Copyright (c) 2002, 2003 Samit Basu
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

#ifndef __AST_hpp__
#define __AST_hpp__
#include <vector>
#include <string>
#include "Types.hpp"

namespace FreeMat {

  typedef enum {
    non_terminal,
    id_node,
    const_int_node,
    const_double_node,
    const_float_node,
    string_const_node,
    null_node,
    reserved_node,
    context_node
  } NODE_TYPE;

  typedef enum {
    OP_BLOCK, // block
    OP_CASEBLOCK, //caseblock
    OP_RHS,
    OP_CSTAT,
    OP_ELSEIFBLOCK,
    OP_ASSIGN,
    OP_MULTICALL,
    OP_COLON,
    OP_PLUS,
    OP_SUBTRACT,
    OP_TIMES,
    OP_RDIV,
    OP_LDIV,
    OP_OR,
    OP_AND,
    OP_LT,
    OP_LEQ,
    OP_GT,
    OP_GEQ,
    OP_EQ,
    OP_NEQ,
    OP_DOT_TIMES,
    OP_DOT_RDIV,
    OP_DOT_LDIV,
    OP_NEG,
    OP_NOT,
    OP_POWER,
    OP_DOT_POWER,
    OP_TRANSPOSE,
    OP_DOT_TRANSPOSE,
    OP_EMPTY,
    OP_EMPTY_CELL,
    OP_PARENS,
    OP_BRACES,
    OP_BRACKETS,
    OP_DOT,
    OP_ALL,
    OP_INDEX_LIST,
    OP_ROW_DEF,
    OP_SEMICOLON,
    OP_NULL,
    OP_RSTATEMENT,
    OP_QSTATEMENT,
    OP_SCALL,
    OP_CCALL,
    OP_SAVE,
    OP_LOAD,
    OP_KEYWORD
  } OP_TYPE;

  /** The abstract syntax tree class
   * This class represents an abstract syntax tree class (AST).  The AST node has 
   * both a "down" and "right" child.
   */
  class AST {
  public:
    NODE_TYPE type;
    char* text;
    int tokenNumber;
    AST* down;
    AST* right;
    OP_TYPE opNum;
  
    /** Default constructor
     * Creates an empty AST node.  All pointers are initialized to NULL,
     * the type is set to non_terminal.
     */
    AST(void);
    /** Text constructor
     * Creates a node of the specified type with the text field set to a copy
     * of the name argument.
     */
    AST(NODE_TYPE ntype, char* name);
    /** Token constructor
     * Creates a node of the specified type with the tokenNumber field set to
     * the token argument.  This constructor is useful for nodes that are represented
     * by a single, numeric token (as opposed to a string).
     */
    AST(NODE_TYPE ntype, int token);
    /** Nonterminal constructor with two arguments
     * Creates a non-terminal node with the text set to a copy of the name argument
     * with the given left and right AST nodes.  The resulting tree fragment
     * looks like:
     *\dotfile ASTdot1.dot
     */
    AST(OP_TYPE op, AST* lt, AST* rt);
    /** Nonterminal constructor with three arguments
     * Creates a non-terminal node with the text set to a copy of the name argument
     * with the given three AST nodes.  The resulting tree fragment
     * looks like:
     *\dotfile ASTdot2.dot
     */
    AST(OP_TYPE op, AST* lt, AST* md, AST* rt);
    /** Nonterminal constructor with a single argument
     * Creates a non-terminal node with the text set to a copy of the name argument
     * with the given AST node.  The resulting tree fragment
     * looks like:
     *\dotfile ASTdot3.dot
     */
    AST(OP_TYPE op, AST* arg);
    /** Destructor
     */
    ~AST();

    /** Test for a match
     * Returns true if textual content of this node matches the supplied argument
     * (and is not NULL).
     */
    bool match(OP_TYPE test);
    /** Add the given tree as a child to the current node
     * Adds the argument tree as a child to the current node.  If the current node
     * has no "children" (i.e., down = NULL) then the supplied tree is placed as 
     * a child.  If there are children, then the node is added as a peer to the last
     * such child.  
     *\dotfile ASTdot4.dot
     */
    void addChild(AST* arg);
    /** Add the given tree as a peer to the current node
     * Adds the argument tree as a peer to the current node.  If the current node
     * has no "peers" (i.e., right = NULL) then the supplied tree is placed as a
     * peer.  If there are peers, then the node is added as a peer to the last such
     * peer.
     *\dotfile ASTdot5.dot
     */
    void addPeer(AST* arg);
    /** Count children
     * Returns the number of children to the current tree.  This is only the number of
     * children that are immediately below the current node (i.e., children of children
     * are not counted).
     */
    int childCount();
    /** Count peers
     * Returns the number of peers to the current tree.  This is only the number of
     * peers that are adjacent to the current node (i.e., children of peers are 
     * not counted).
     */
    int peerCount();
    /** Convert this into a string list.
     * Builds a list of strings from the current node and its children.
     */
    stringVector toStringList();
    /** Test for an empty tree
     * Returns true if the current node is unallocated or is not a reserved node
     * with an empty text string.
     */
    bool isEmpty();
  };

  typedef AST* ASTPtr;
  typedef std::vector<ASTPtr> ASTPtrVector;
  /** Print out the tree
   * Print out the tree using a tab-level scheme.  Peers are printed at the same
   * tab level, children are printed at a higher tab level.
   */
  void printAST(ASTPtr t);
}

#endif
