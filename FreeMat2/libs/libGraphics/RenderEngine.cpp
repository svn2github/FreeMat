#include "RenderEngine.hpp"

namespace FreeMat {
  RenderEngine::SymbolType StringToSymbol(std::string name) {
    if (name == "+")
      return RenderEngine::Plus;
    if (name == "o")
      return RenderEngine::Circle;
    if (name == "*")
      return RenderEngine::Star;
    if (name == ".")
      return RenderEngine::Dot;
    if (name == "x")
      return RenderEngine::Times;
    if ((name == "square") || (name == "s"))
      return RenderEngine::Square;
    if ((name == "diamond") || (name == "d"))
      return RenderEngine::Diamond;
    if (name == "^")
      return RenderEngine::Up;
    if (name == "v")
      return RenderEngine::Down;
    if (name == ">")
      return RenderEngine::Right;
    if (name == "<")
      return RenderEngine::Left;
    return RenderEngine::None;
  }
  
  void DrawSymbol(RenderEngine& gc, RenderEngine::SymbolType symb,
		  double x, double y, double z, double sze,
		  std::vector<double> edgecolor, 
		  std::vector<double> fillcolor, 
		  double width) {
    bool stroke, fill;
    stroke = edgecolor[0] != -1;
    fill = fillcolor[0] != -1;
    gc.lineWidth(width);
    gc.setLineStyle("-");
    switch (symb) {
    case RenderEngine:: Plus:
      if (stroke) {
	gc.color(edgecolor);
	gc.line(x-sze,y,z,x+sze,y,z);
	gc.line(x,y-sze,z,x,y+sze,z);
      }
      break;
    case RenderEngine:: Circle:
      if (fill) {
	gc.color(fillcolor);
	gc.circleFill(x,y,sze);
      }
      if (stroke) {
	gc.color(edgecolor);
	gc.circle(x,y,sze);
      } 
      break;
    case RenderEngine:: Star:
      if (stroke) {
	gc.color(edgecolor);
	gc.line(x-sze,y-sze,z,x+sze,y+sze,z);
	gc.line(x+sze,y-sze,z,x-sze,y+sze,z);
	gc.line(x-sze,y,z,x+sze,y,z);
	gc.line(x,y-sze,z,x,y+sze,z);
      }
      break;
    case RenderEngine:: Dot:
      if (stroke) {
	gc.color(edgecolor);
	gc.circleFill(x,y,sze/3.0);
      }
      break;
    case RenderEngine:: Times:
      if (stroke) {
	gc.color(edgecolor);
	gc.line(x-sze,y-sze,z,x+sze,y+sze,z);
	gc.line(x+sze,y-sze,z,x-sze,y+sze,z);
      }
      break;
    case RenderEngine:: Square:
      if (fill) {
	gc.color(fillcolor);
	gc.rectFill(x-sze,y-sze,x+sze,y+sze);
      }
      if (stroke) {
	gc.color(edgecolor);
	gc.rect(x-sze,y-sze,x+sze,y+sze);
      }
      break;
    case RenderEngine:: Diamond:
      if (fill) {
	gc.color(fillcolor);
	gc.quad(x,y-sze,0,
		x+sze,y,0,
		x,y+sze,0,
		x-sze,y,0);
      }
      if (stroke) {
	gc.color(edgecolor);
	gc.quadline(x,y-sze,0,
		    x+sze,y,0,
		    x,y+sze,0,
		    x-sze,y,0);
      }
      break;
    case RenderEngine:: Up:
      if (fill) {
	gc.color(fillcolor);
	gc.tri(x,y+sze,0,
	       x+sze,y-sze,0,
	       x-sze,y-sze,0);
      }
      if (stroke) {
	gc.color(edgecolor);
	gc.triLine(x,y+sze,0,
		   x+sze,y-sze,0,
		   x-sze,y-sze,0);
      }
      break;
    case RenderEngine:: Down:
      if (fill) {
	gc.color(fillcolor);
	gc.tri(x,y-sze,0,
	       x+sze,y+sze,0,
	       x-sze,y+sze,0);
      }
      if (stroke) {
	gc.color(edgecolor);
	gc.triLine(x,y-sze,0,
		   x+sze,y+sze,0,
		   x-sze,y+sze,0);
      }
      break;      
    case RenderEngine:: Right:
      if (fill) {
	gc.color(fillcolor);
	gc.tri(x-sze,y+sze,0,
	       x+sze,y,0,
	       x-sze,y-sze,0);
      }
      if (stroke) {
	gc.color(edgecolor);
	gc.triLine(x-sze,y+sze,0,
		   x+sze,y,0,
		   x-sze,y-sze,0);
      }
      break;      
    case RenderEngine:: Left:
      if (fill) {
	gc.color(fillcolor);
	gc.tri(x+sze,y+sze,0,
	       x-sze,y,0,
	       x+sze,y-sze,0);
      }
      if (stroke) {
	gc.color(edgecolor);
	gc.triLine(x+sze,y+sze,0,
		   x-sze,y,0,
		   x+sze,y-sze,0);
      }
      break;      
    case RenderEngine:: Pentagram:
    case RenderEngine:: Hexagram:
      break;
    }
  }
}
