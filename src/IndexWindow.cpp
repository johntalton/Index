/*******************************************************
*   Index©
*
*   @author  YNOP (ynop@acm.org)
*   @version beta
*   @date    Dec 17 1999
*******************************************************/
#include <Application.h>
#include <AppKit.h>
#include <InterfaceKit.h>
#include <StorageKit.h>
#include <TranslationKit.h>
#include <TranslationUtils.h>

#include "Globals.h"
#include "IndexWindow.h"
#include "TPreferences.h"
#include "BugOutDef.h"

extern BugOut db;
/*******************************************************
*   Our wonderful BWindow, ya its kewl like that.
*   we dont do much here but set up the menubar and 
*   let the view take over.  We also nead some message
*   redirection and handling
*******************************************************/
IndexWindow::IndexWindow(BRect frame) : BWindow(frame,"Index  ",B_DOCUMENT_WINDOW,B_ASYNCHRONOUS_CONTROLS|B_NOT_RESIZABLE){//B_NOT_ANCHORED_ON_ACTIVATE|
   BRect r;
   
   r = Bounds();
   // Creat a standard menubar
   menubar = new BMenuBar(r, "MenuBar");
   // Standard File menu
   menu = new BMenu("File");
   menu->AddItem(item=new BMenuItem("About Index", new BMessage(B_ABOUT_REQUESTED), 'A'));
   item->SetTarget(be_app);
   menu->AddItem(new BSeparatorItem());
   menu->AddItem(item=new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED), 'Q'));
   menubar->AddItem(menu);
   
   menu = new BMenu("Help");
   menu->AddItem(new BMenuItem("BeBook Index section",new BMessage(INFO)));
   menu->AddItem(new BMenuItem("Help",new BMessage(HELP)));
   menubar->AddItem(menu);
   
   // Attach the menu bar to he window
   AddChild(menubar);
   
   // Do a little claculating to take menubar int account
   r = Bounds();
   r.bottom = r.bottom - menubar->Bounds().bottom;
   View = new IndexView(this,r);
   View->MoveBy(0, menubar->Bounds().Height() + 1);
   AddChild(View);
   db.SendMessage("About to show window");
   Show();
}

/*******************************************************
*
*******************************************************/
void IndexWindow::FrameResized(float,float){
}

/*******************************************************
*   To make things a little nicer and more organized
*   we are gona let View be the message handler for 
*   the whole app. All messages that are ours send to
*   View for redistribution.  But we must handle our
*   own BWindow messages or else (crash)
*******************************************************/
void IndexWindow::MessageReceived(BMessage* msg){
   BString address;
   char *ad;
   
   switch(msg->what){
   case HELP:
      address.SetTo("http://www.latech.edu/~jta001/BeOS/index.html");
      ad = (char*)address.String();
      be_roster->Launch("application/x-vnd.Be-NPOS",1,&ad);
      break;
   case INFO:
      address.SetTo("file:///boot/beos/documentation/Be%20Book/The%20Storage%20Kit/IndexFuncs.html");
      ad = (char*)address.String();
      be_roster->Launch("application/x-vnd.Be-NPOS",1,&ad);
      break;
   default:
      BWindow::MessageReceived(msg);
   }
}

/*******************************************************
*   Someone asked us nicely to quit. I gess we should
*   so clean up. save our setings (position size of win)
*   and tell the main be_app to shut it all down .. bye
*******************************************************/
bool IndexWindow::QuitRequested(){
   BMessage SPMessage;
   BPath path;
   
   TPreferences prefs("Index_prefs");
   if (prefs.InitCheck() != B_OK) {
   }
   prefs.SetRect("window_pos", Frame());

   be_app->Lock();
   be_app->Quit();
   be_app->Unlock();
   return true;
}
