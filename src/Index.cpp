/*******************************************************
*   Index©
*
*   @author  YNOP (ynop@acm.org)
*   @version beta
*   @date    Dec 17 1999
*******************************************************/
#include <AppKit.h>
#include <InterfaceKit.h>
#include <StorageKit.h>
#include <Alert.h>
#include <String.h>

#include <stdio.h>

#include "Index.h"
#include "IndexWindow.h"
#include "TPreferences.h"

/*******************************************************
*
*******************************************************/
Index::Index() : BApplication(APP_SIGNATURE){
   BRect defaultSize(50,50,450,450);
   BRect wind_pos;
   
   TPreferences prefs("Index_prefs");

   if (prefs.InitCheck() != B_OK) {
      (new BAlert(NULL,"WARNING\n\nThis app is made for advanced users \
who wish to edit the Indexs on there BVolumes. Deleting Indexs may cause \
problems with your system. Use this app at your won risk. If you find a \
bug Please report it to ynop@acm.org\nThanks","go away",NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT))->Go();
   }

   if(prefs.FindRect("window_pos", &wind_pos) != B_OK){
      wind_pos = defaultSize;
   }

   if(!wind_pos.Intersects(BScreen().Frame())){
      (new BAlert(NULL,"The window was somehow off the screen. We reset it position for you","Thanks"))->Go();
      theWin = new IndexWindow(defaultSize);
   }else{
      // this is the normal start up.
      theWin = new IndexWindow(wind_pos);
   }


}

/*******************************************************
*   This is our Hook for when the Programe is started
*   by double clicking on a file we support 
*******************************************************/
void Index::RefsReceived(BMessage *message) {
   entry_ref ref; // The entry_ref to open
   status_t err; // The error code
   int32 ref_num; // The index into the ref list
   BPath path;
   // Loop through the ref list and open each one
   ref_num = 0;
   //do {
      if ((err = message->FindRef("refs", ref_num, &ref)) != B_OK) {
         return;
      }
      BEntry entry(&ref, true);
      if(entry.GetPath(&path)==B_OK){
         if(entry.IsFile()){      


         }   
      }
   //ref_num++;
   //} while (1);
   
}

/*******************************************************
*
*******************************************************/
void Index::ReadyToRun(){

  // be_app->PostMessage(B_QUIT_REQUESTED);
}

/*******************************************************
*
*******************************************************/
void Index::ArgvReceived(int32 argc, char **argv){
   // get --help comand first .. then do this
   BString s;
   s.SetTo("");
   int32 i,help;
   for(i = 1;i <= argc;i++){
      s.Append(argv[i]); // pack all the args into one exept last.
   }
   if((help = s.IFindFirst("--help")) < 0){
      help = s.IFindFirst("-help");
   }
   if(help >= 0){  // Do help
      printf("   Useage: %s \n",argv[0]);
      printf("      Example:  %s\n",argv[0]); 
      printf("   This is a GUI app. No text base interface\n");
      printf("   If you want to use these types of tool from the trem\n");
      printf("   you may wish to take a look atlsindex/rmindex/mkindex \n");
      printf("   these should work for you.\n\n"); 
      be_app->PostMessage(B_QUIT_REQUESTED);
   }else{  

   }
}

/*******************************************************
*
*******************************************************/
void Index::AboutRequested(){
   (new BAlert("About Index","Index ©1999\n\nTheAbstractCompany\nynop@acm.org\n\nVersion: 1.0 Beta 1.x","Thats Nice"))->Go();
}

/*******************************************************
*
*******************************************************/
void Index::MessageReceived(BMessage *msg){
   switch(msg->what){
   default:
      BApplication::MessageReceived(msg);
      break; 
   }  
}

/*******************************************************
*
*******************************************************/
bool Index::QuitRequested(){
   theWin->Lock();
   if(theWin->QuitRequested()){
      theWin->Unlock(); 
      return true; // Ok .. fine .. leave then
   }else{
      theWin->Unlock();
      return false;
   }
}




