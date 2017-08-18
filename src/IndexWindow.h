#ifndef _IMAGESHOW_WINDOW_H
#define _IMAGESHOW_WINDOW_H

#include <Application.h>
#include <AppKit.h>
#include <InterfaceKit.h>

#include "IndexView.h"

class IndexWindow : public BWindow {
   public:
      IndexWindow(BRect);
      virtual void MessageReceived(BMessage*);
      virtual bool QuitRequested();
      virtual void FrameResized(float,float);
      void FilterImages(bool);
      void ViewThumbs(bool);
   private:
      BMenuBar *menubar;
      BMenu *menu;
      BMenu *submenu;
      BMenuItem *item;
      IndexView *View;
  
};
#endif