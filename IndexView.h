#ifndef _IMAGESHOW_VIEW_H
#define _IMAGESHOW_VIEW_H

#include <Application.h>
#include <AppKit.h>
#include <InterfaceKit.h>
#include <String.h>

#include "IndexListView.h"

/*******************************************************
*   Our quick little struct for adding list stuff.
*******************************************************/
struct MainItems{
   BListItem *item;
   BString name;
};


class IndexView : public BView {
   public:
      IndexView(BWindow*,BRect);
      BMessage* GetSplitPaneState();
      virtual void AllAttached();
      virtual void MessageReceived(BMessage*);
      void MakeIndexes();
      void MakeIndexes(int32);
   private:
      int32 DeviceNumber;
      
      BWindow *parentWindow;
      BBox *StatusBar;
      
      //BOutlineListView *ollv;
      IndexListView *ollv;
      
      MainItems Items[2000];
      int32 ItemCount;
      BButton *AddButton;
      BButton *SubButton;
    
      BTextControl *IndexName;
      
      BPopUpMenu *VolList;
      BMenuField *VolMenu;
};
#endif