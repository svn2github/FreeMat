#include <Carbon/Carbon.h>

WindowRef win;

#define NEVENT 6
EventTypeSpec commSpec[NEVENT] = {
  {kEventClassCommand, kEventProcessCommand},
  {kEventClassKeyboard, kEventRawKeyDown},
  {kEventClassKeyboard, 1234},
  {kEventClassWindow, kEventWindowResizeCompleted},
  {kEventClassWindow, kEventWindowZoomed},
  {kEventClassWindow, kEventWindowClosed},
};

pascal OSStatus WndProc(EventHandlerCallRef handlerRef,
			EventRef event,
			void *userData) {
  int yclass, kind;
  OSStatus err = noErr;
  
  yclass = GetEventClass(event);
  kind = GetEventKind(event);
  
  if (yclass = kEventClassWindow) {
    switch (kind) {
    case kEventWindowResizeCompleted:
    case kEventWindowZoomed:
      //      do_resize();
      break;
    case kEventWindowClosed:
      exit(0);
      break;
    default:
      err = eventNotHandledErr;
      break;
    }
  }
}

int main(int argc, char* argv[])
{
  Rect winSize;
  
  winSize.top = 50;
  winSize.left = 50;
  winSize.bottom = 400;
  winSize.right = 400;

  CreateNewWindow(kDocumentWindowClass,
		  kWindowStandardDocumentAttributes,
		  &winSize,
		  &win);
  InstallWindowEventHandler(win, NewEventHandlerUPP(WndProc),
			    NEVENT, commSpec, NULL, NULL);
  ShowWindow(win);
  RunApplicationEventLoop();
}
