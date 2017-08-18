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
#include <String.h>
#include <Path.h>
#include <Volume.h>
#include <VolumeRoster.h>
#include <stdio.h>
#include <fs_index.h>
#include <OutlineListView.h>
#include <ListView.h>

//#include <stdio.h>

#include "Globals.h"
#include "IndexView.h"
#include "IndexListView.h"

/*******************************************************
*   Setup the main view. Add in all the niffty components
*   we have made and get things rolling
*******************************************************/
IndexView::IndexView(BWindow *parentWin, BRect frame):BView(frame, "", B_FOLLOW_ALL_SIDES, B_FRAME_EVENTS){//B_PULSE_NEEDED|B_FRAME_EVENTS|B_WILL_DRAW
   SetViewColor(216,216,216,0);
   BRect b;
   b = Bounds();
   
   parentWindow = parentWin;
   
   StatusBar = new BBox(BRect(b.left-3,b.bottom-14,b.right,b.bottom+3),"StatusBar",B_FOLLOW_LEFT_RIGHT|B_FOLLOW_BOTTOM); 
   //StatusBar->SetViewColor(100,100,255,0);
   AddChild(StatusBar);
   
   b.bottom -= 15 + B_H_SCROLL_BAR_HEIGHT + 5 + 3;
   b.top += 5 + 3;
   b.left += 5 + 3;
   b.right = 175;
//   ollv = new BOutlineListView(b,"",B_SINGLE_SELECTION_LIST,B_FOLLOW_ALL_SIDES,B_WILL_DRAW);//B_NAVIGABLE|
   ollv = new IndexListView(b,B_FOLLOW_ALL_SIDES);
   BScrollView *sv = new BScrollView("",ollv,B_FOLLOW_TOP_BOTTOM,0,true,true);
   sv->SetViewColor(216,216,216);
   AddChild(sv);
   //ollv->SetInvocationMessage(new BMessage(ITEM_SELECTED));
   
   BRect a = sv->Bounds();
   a.bottom += 6;
   a.right += 6;
   a.top = a.bottom - B_H_SCROLL_BAR_HEIGHT - 1;
   a.left = a.right - B_V_SCROLL_BAR_WIDTH - 1;
   BBox *box = new BBox(a,"leftover",B_FOLLOW_BOTTOM); 
   AddChild(box);
      
   b.left = b.right + B_V_SCROLL_BAR_WIDTH + 10;
   
   AddButton = new BButton(b, "add index","Add Index", new BMessage(ADD_INDEX));
   AddButton->ResizeToPreferred();
   AddChild(AddButton);
   
   b.left = AddButton->Frame().right + 5;
   
   SubButton = new BButton(b, "sub index","Remove Index", new BMessage(SUB_INDEX));
   SubButton->ResizeToPreferred();
   AddChild(SubButton);
   
   b.left = sv->Bounds().right + 10 + 3;
   b.right = Bounds().right - 5 - 3;
   b.top = 40;
   b.bottom = Bounds().bottom - (21); // this should be 15 + 5 + 3 but its off
   BBox *otherbox = new BBox(b,"inbox",B_FOLLOW_BOTTOM); 
   AddChild(otherbox);
   
   b = otherbox->Bounds();
   
   BStringView *s1 = new BStringView(BRect(5,10,b.right-20,25),"","Should be in form MAJOR:minor");
   otherbox->AddChild(s1);
      
   b.top += 40;
   b.left += 5;
   b.right -= 5 + 3; // why do we need +3 here - we shouldn't
   
   IndexName = new BTextControl(b,"IndexName","","",new BMessage(ADD_INDEX));
   IndexName->SetDivider(0);
   otherbox->AddChild(IndexName);

   BVolumeRoster Vroster;
   BVolume vol;
   int32 DevNum;
   char name[B_FILE_NAME_LENGTH] ;
   BString Name;
   BMessage *devMsg = NULL;
   
   
   Vroster.GetBootVolume(&vol);
   vol.GetName(name);
   DevNum = vol.Device();
   devMsg = new BMessage(CHANGE_DEVICE);
   devMsg->AddInt32("devNumber",DevNum);
   VolList = new BPopUpMenu("Boot device");
   VolList->AddItem(new BMenuItem("Boot device",devMsg));
   
   VolList->AddItem(new BSeparatorItem());
   
   while(Vroster.GetNextVolume(&vol) == B_OK){
      vol.GetName(name);
      Name.SetTo(name);
      if(Name != ""){
        DevNum = vol.Device();
        devMsg = new BMessage(CHANGE_DEVICE);
        devMsg->AddInt32("devNumber",DevNum);
        VolList->AddItem(new BMenuItem(name,devMsg));
      }
   }
   
   b = otherbox->Bounds();
   b.left += 10;
   b.right -= 10;
   b.top = 80;
   
   VolMenu = new BMenuField(b,"","Select a Volume",VolList);
   //VolMenu->SetDivider(0);
   otherbox->AddChild(VolMenu);

   
}

/*******************************************************
*   
*******************************************************/
void IndexView::AllAttached(){
   AddButton->SetTarget(this);
   SubButton->SetTarget(this);
   ollv->SetTarget(this);
   IndexName->SetTarget(this);
   for(int32 i = 0;i < VolList->CountItems();i++){
      (VolList->ItemAt(i))->SetTarget(this);
   }
   
   MakeIndexes();
}

/*******************************************************
*   
*******************************************************/
void IndexView::MakeIndexes(){
   BVolumeRoster roster;
   BVolume vol;
   roster.GetBootVolume(&vol);
   DeviceNumber = vol.Device();
   MakeIndexes(DeviceNumber);
}

/*******************************************************
*   
*******************************************************/
void IndexView::MakeIndexes(int32 device){
//   BListItem *item;
   ollv->MakeEmpty();
   
   DeviceNumber = device;
   
   BVolume vol(device);
   if(!vol.KnowsQuery()){
      //(new BAlert(NULL,"Device Does not suport Querys","Oh"))->Go();
      return;
   }
   
   BString tmp;
   int i = 0;
   BString MainName;
   BString MinorName;   
   bool found = false;
   int foundAt = -1;
   DIR *d;
   //index_info info;
   struct dirent *ent;
  
   ItemCount = 0;

   d = fs_open_index_dir(device);
   if(!d){
      (new BAlert(NULL,"Uhmmm\nError, there is not indexes dir for this Volume","Oops"));
   }else{
      while((ent = fs_read_index_dir(d))){
         tmp.SetTo(ent->d_name);
         i = tmp.FindFirst(":");
         if(i < 0){
            ollv->AddItem(Items[ItemCount].item = new BStringItem(tmp.String()));
            Items[ItemCount].name = tmp.String();
            ItemCount++;
         }else{
            tmp.MoveInto(MainName,0,i);
            tmp.MoveInto(MinorName,1,tmp.CountChars()-1);
            found = false;
            foundAt = -1;
            while((!found) && (foundAt < (ItemCount-1))){
                if(Items[++foundAt].name == MainName){
                   found = true;
                }
            }
            if((found) && (Items[foundAt].item)){
               ollv->AddUnder(new BStringItem(MinorName.String()),Items[foundAt].item);
               ollv->Collapse(Items[foundAt].item);
            }else{
               ollv->AddItem(Items[ItemCount].item = new BStringItem(MainName.String()));
               Items[ItemCount].name.SetTo(MainName.String());
               ollv->AddUnder(new BStringItem(MinorName.String()),Items[ItemCount].item);
               ollv->Collapse(Items[ItemCount].item);
               ItemCount++;
            }
         }
      }
      fs_close_index_dir(d);
   }
}


/*******************************************************
*   Central messageing area for the whole app. All msgs
*   get sent here if they are one of ours, then they
*   get sent back to the apropriate place. This is the
*   command center of the app!
*******************************************************/
void IndexView::MessageReceived(BMessage *msg){
   BStringItem *li;
   BStringItem *si;
   BString key;
   bool found;
   BString MainName;
   BString MinorName;
   BString tmp;
   int32 foundAt;
   int32 answer;
   BStringItem *item = NULL;
   int32 i,j,k;
   int32 errorNum;
   bool error;
   int32 DevNum;
   BEntry entry;
   entry_ref ref;
   BPath path;
   
   switch(msg->what){
   case B_SIMPLE_DATA:
      if( msg->FindRef("refs", &ref) == B_OK ){
         entry.SetTo(&ref, true);
         (new BAlert(NULL,"Future ver will set a filter for the indexs based upon this files attributs","Ok"))->Go();
         if(entry.IsFile() && entry.GetPath(&path)==B_OK){
            //here we grab the addributs of a file and 
            // we filter our volume list so that it 
            // only applys to this file
         }
      }
      break;
   case CHANGE_DEVICE:
      if(msg->FindInt32("devNumber",&DevNum) == B_OK){
         MakeIndexes(DevNum);
      }else{
         MakeIndexes(); // do boot volume
      }
      break;
   case ADD_INDEX:
      (new BAlert(NULL,"Warning\nFiles that have this index value will not get index, you must ReIndex them (copy does this btw)","Ok",NULL,NULL,B_WIDTH_AS_USUAL,B_WARNING_ALERT))->Go();
      tmp.SetTo(IndexName->Text());
      foundAt = tmp.FindFirst(":");
      if(foundAt > 0){
         tmp.MoveInto(MainName,0,foundAt);
         tmp.MoveInto(MinorName,1,tmp.CountChars()-1);
      }else{
         MainName.SetTo(tmp);
      }
      if(MainName == ""){
         (new BAlert(NULL,"Error\nYou dont realy want to add \"\" to the indexes do you?","NO",NULL,NULL,B_WIDTH_AS_USUAL,B_WARNING_ALERT))->Go();
         break;
      }

      key.SetTo(MainName.String());
      key.Append(":");
      key.Append(MinorName.String());

       /*DO REAL WORK HERE NOW*/
      printf("Add kew to Volume: %s\n",key.String());
      
      errorNum = fs_create_index(DeviceNumber, key.String(), B_STRING_TYPE, 0);
      error = true;
      switch(errorNum){
      case B_BAD_VALUE:
         break;
      case B_NOT_ALLOWED:
         break;
      case B_NO_MEMORY:
         break;
      case B_FILE_EXISTS:
         break;
      case B_DEVICE_FULL:
         break;
      case B_FILE_ERROR:
         break;
      case B_OK:
         error = false;
         break;
      default:
         //no error
         break;
      }
      /*END OF THE READ WORK */

      if(!error){
         found = false;
         foundAt = -1;
         while((!found) && (foundAt < (ItemCount-1))){
            if(Items[++foundAt].name == MainName){
               found = true;
            }
         }
         if((found) && (Items[foundAt].item)){
            if(MinorName != ""){
               ollv->AddUnder(new BStringItem(MinorName.String()),Items[foundAt].item);
               ollv->Expand(Items[foundAt].item);
            }
         }else{
            ollv->AddItem(Items[ItemCount].item = new BStringItem(MainName.String()));
            Items[ItemCount].name.SetTo(MainName.String());
            if(MinorName != ""){
               ollv->AddUnder(new BStringItem(MinorName.String()),Items[ItemCount].item);
               ollv->Expand(Items[ItemCount].item);
            }
            ItemCount++;
         }
      }
      break;
   case SUB_INDEX:
      answer = (new BAlert(NULL,"Warning deleteing a index means that you will no longer be able to use it as a search key. Be verry carfull","Cancel","Ok",NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT))->Go();
      if(answer == 1){
         li = (BStringItem*)ollv->ItemAt(ollv->CurrentSelection());
         if(li){
            si = (BStringItem*)ollv->Superitem(li);
            if(si){
               MainName.SetTo(si->Text());
               key.SetTo(MainName.String());
               key.Append(":");
               MinorName.SetTo(li->Text());
               key.Append(MinorName.String());
            }else{
               MainName.SetTo(li->Text());
               key.Append(MainName.String());
            }
            found = false;
            foundAt = -1;
            while((!found) && (foundAt < (ItemCount-1))){
               if(Items[++foundAt].name == MainName){
                  found = true;
               }
            }
            if(foundAt >= 0){
               if(MinorName != ""){
                  if((key.Compare("BEOS:APP_SIG") == 0) ||
                     (key.Compare("last_modified") == 0) ||
                     (key.Compare("name") == 0) ||
                     (key.Compare("size") == 0)
                    ){
                     (new BAlert(NULL,"Ahh!\nThis key is a system key. Don't do that!","Oh"))->Go();
                     break;
                  }
               
                  /*DO REAL WORK HERE NOW*/
                  printf("Removeing key from Volume: %s\n",key.String());
                  errorNum = fs_remove_index(DeviceNumber,key.String());
                  error = true;
                  switch(errorNum){
                  case B_FILE_ERROR:
                     break;
                  case B_BAD_VALUE:
                     break;
                  case B_NOT_ALLOWED:
                     break;
                  case B_NO_MEMORY:
                     break;
                  case B_ENTRY_NOT_FOUND:
                     break;
                  case B_OK:
                     error = false;
                     break;
                  default:
                     break;
                  }
                  /*END OF THE READ WORK */
               
                  if(!error){
                     i = ollv->IndexOf(Items[foundAt].item);
                     j = ollv->CountItemsUnder(Items[foundAt].item,true);
                     i++;
                     found = false;
                     for(k = 0;k < j;k++){
                        item = (BStringItem*)ollv->ItemAt(i);
                        if((item->Text() == MinorName)){
                           found = true;
                           ollv->RemoveItem(item);
                        }
                        i++;
                     }
                  }
               }else{
                  (new BAlert(NULL,"Can not delete a whole group, mabey in next ver.","Oh"))->Go();
               }
            }else{
               (new BAlert(NULL,"We did not find that Item in the list!","What?"))->Go();
            }
         }else{
            (new BAlert(NULL,"Seclect something fool","oh"))->Go();
         }
      }
      break;
   case ITEM_SELECTED:
      break;
   default:
      BView::MessageReceived(msg);
   }
}






