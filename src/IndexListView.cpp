/*******************************************************
*   Index©
*
*   @author  YNOP (ynop@acm.org)
*   @version beta
*   @date    Dec 17 1999
*******************************************************/
#include <AppKit.h>
#include <InterfaceKit.h>
#include <OutlineListView.h>
#include <ListView.h>

//#include <stdio.h>

#include "Globals.h"
#include "IndexListView.h"
#include "BugOutDef.h"

extern BugOut db;

/*******************************************************
*   Setup the main view. Add in all the niffty components
*   we have made and get things rolling
*******************************************************/
IndexListView::IndexListView(BRect frame,uint32 mode):BOutlineListView(frame,"",B_SINGLE_SELECTION_LIST,mode,B_WILL_DRAW){

}

/*******************************************************
*   
*******************************************************/
bool IndexListView::InitiateDrag(BPoint p, int32 index, bool selected){
   return true;
}

/*******************************************************
*   Central messageing area for the whole app. All msgs
*   get sent here if they are one of ours, then they
*   get sent back to the apropriate place. This is the
*   command center of the app!
*******************************************************/
void IndexListView::MessageReceived(BMessage *msg){
 
   switch(msg->what){
   default:
      BView::MessageReceived(msg);
   }
}






