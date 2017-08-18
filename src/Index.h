#ifndef _Index_H
#define _Index_H

#include <Application.h>
#include <AppKit.h>
#include <InterfaceKit.h>
#include <Path.h>

extern const char *APP_SIGNATURE;

class Index : public BApplication {
   public:
      Index();
      virtual void RefsReceived(BMessage*);
      virtual void AboutRequested();
      virtual void ArgvReceived(int32,char**);
      virtual void ReadyToRun();
      virtual void MessageReceived(BMessage*);
      virtual bool QuitRequested();
   private:   
      BWindow *theWin;
};
#endif
