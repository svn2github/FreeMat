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

#ifndef __Command_hpp__
#define __Command_hpp__

#include "Array.hpp"

namespace FreeMat {

  typedef enum {
    CMD_PlotSetXAxisLabel = 1024,
    CMD_PlotSetYAxisLabel,
    CMD_PlotSetTitle,
    CMD_PlotSetGrid,
    CMD_PlotSetHold,
    CMD_PlotNew,
    CMD_PlotNewAcq,
    CMD_PlotUse,
    CMD_PlotClose,
    CMD_PlotAdd,
    CMD_PlotPrint,
    CMD_PlotStartSequence,
    CMD_PlotStopSequence,
    CMD_PlotAxisTight,
    CMD_PlotAxisAuto,
    CMD_PlotAxisSet,
    CMD_PlotAxisGetRequest,
    CMD_PlotAxisGetAcq,
    CMD_ImageNew,
    CMD_ImageNewAcq,
    CMD_ImageUse,
    CMD_ImageClose,
    CMD_ImageSet,
    CMD_ImagePrint,
    CMD_ImageZoom,
    CMD_ImageColormap,
    CMD_VolumeNew,
    CMD_VolumeNewAcq,
    CMD_VolumeUse,
    CMD_VolumeClose,
    CMD_VolumeSet,
    CMD_FilePick,
    CMD_FilePickAcq,
    CMD_Point,
    CMD_PointAcq,
    CMD_HelpShow,
    CMD_GUIOutputMessage,
    CMD_GUIErrorMessage,
    CMD_GUIWarningMessage,
    CMD_GUIGetLine,
    CMD_GUIGetLineAcq,
    CMD_GUIGetWidth,
    CMD_GUIGetWidthAcq,
    CMD_Quit,
    CMD_SystemCapture,
    CMD_SystemCaptureAcq
  };

  /** A GUI command class
   * This class is used for communication between the graphics functions
   * (contained in libGraphics) and the App class, which routes these
   * commands to the graphics objects themselves.
   */
  class Command {
  public:
    /**
     * The number of the command.
     */
    int cmdNum;
    /**
     * The Array which stores data for the command.
     */
    Array data;
    /**
     * Empty constructor.
     */
    Command();
    /**
     * Construct a base class with the given command number.
     * (and no payload).
     */
    Command(int a);
    /**
     * Construct a command with the given command number and
     * the given Array object as a payload.
     */
    Command(int a, Array b);
    /**
     * Destructor.
     */
    virtual ~Command();
  };

  /**
   * Send the given command object to the GUI as an event,
   * so that it is processed by App::OnProcessCustom.
   */
  void SendGUICommand(Command *cmd);

  /**
   * Send the given command object as a reply to a GUI command.
   */
  void PostGUIReply(Command *reply);
  
  /**
   * Retrieve the reply for a GUI command.
   */
  Command* GetGUIResponse();

}

#endif
