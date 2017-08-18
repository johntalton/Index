#ifndef _INDEX_LIST_VIEW_H
#define _INDEX_LIST_VIEW_H

#include <Application.h>
#include <AppKit.h>
#include <InterfaceKit.h>
#include <String.h>

class IndexListView : public BOutlineListView {
   public:
      IndexListView(BRect,uint32);
      virtual bool InitiateDrag(BPoint, int32, bool);
      virtual void MessageReceived(BMessage*);
   private:
 
};
#endif