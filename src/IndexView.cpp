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
#include "BugOutDef.h"

extern BugOut db;
/*******************************************************
*   Setup the main view. Add in all the niffty components
*   we have made and get things rolling
*******************************************************/
IndexView::IndexView(BWindow *parentWin, BRect frame):BView(frame, "", B_FOLLOW_ALL_SIDES, B_FRAME_EVENTS){//B_PULSE_NEEDED|B_FRAME_EVENTS|B_WILL_DRAW
   SetViewColor(216,216,216,0);
   BRect b;
   b = Bounds();
   
   db.SendMessage(BUG_MESSAGE,"IndexView init");
   
   IndexType = B_INT32_TYPE;
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
      
   
   TypeList = new BPopUpMenu("B_INT32_TYPE");
   TypeList->AddItem(new BMenuItem("B_INT32_TYPE",new BMessage(B_INT32_TYPE)));
   TypeList->AddItem(new BMenuItem("B_INT64_TYPE",new BMessage(B_INT64_TYPE)));
   TypeList->AddItem(new BMenuItem("B_FLOAT_TYPE",new BMessage(B_FLOAT_TYPE)));
   TypeList->AddItem(new BMenuItem("B_DOUBLE_TYPE",new BMessage(B_DOUBLE_TYPE)));
   TypeList->AddItem(new BMenuItem("B_STRING_TYPE",new BMessage(B_STRING_TYPE)));
   TypeList->AddItem(new BMenuItem("B_MIME_STRING_TYPE",new BMessage(B_MIME_STRING_TYPE)));
   
   //TypeList->AddItem(new BSeparatorItem());
   
   b.top += 30;
   b.left += 5;
   b.right -= 5 + 3; // why do we need +3 here - we shouldn't
    
   TypeMenu = new BMenuField(b,"","Index Type",TypeList);
   otherbox->AddChild(TypeMenu);
   
   b.top += 30;
  
   
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
   for(int32 i = 0;i < TypeList->CountItems();i++){
      (TypeList->ItemAt(i))->SetTarget(this);
   }
   db.SendMessage(BUG_WARNING,"just leting you know we are about to do real work");
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
   BStringItem *li = NULL;
   BStringItem *si = NULL;
   BString key;
   bool found;
   BString MainName;
   BString MinorName;
   BString tmp;
   int32 foundAt;
   bool isGroup;
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
   case B_INT32_TYPE:
   case B_INT64_TYPE:
   case B_FLOAT_TYPE:
   case B_DOUBLE_TYPE:
   case B_STRING_TYPE:
   case B_MIME_STRING_TYPE:
      IndexType = msg->what;
      break;
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
      
      errorNum = fs_create_index(DeviceNumber, key.String(), IndexType, 0);
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
      isGroup = false;
      answer = (new BAlert(NULL,"Warning deleteing a index means that you will no longer be able to use it as a search key. Be verry carfull","Cancel","Ok",NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT))->Go();
      if(answer != 1){ break; }
      
      li = (BStringItem*)ollv->ItemAt(ollv->CurrentSelection());
      if(li){
         si = (BStringItem*)ollv->Superitem(li);
      }
      if(si == NULL){
         //then we are a group
         isGroup = true;
      }else{
         // we are a sub key
         isGroup = false;
      }     
      
      // creat MainName. Always fill out the MainName
      //   string so we can use it to find stuff.
      //   if it is a group then fill out both      
      if(isGroup){
         tmp.SetTo(li->Text());
         MainName.SetTo(li->Text());
         MinorName.SetTo("");
         key.SetTo(li->Text());
      }else{
         tmp.SetTo(li->Text());
         MainName.SetTo(si->Text());
         MinorName.SetTo(li->Text());
         
         // Creat full key
         key.SetTo(MainName.String());
         key.Append(":");
         key.Append(MinorName.String());
         
      }
      
      
      
      // CHECK FOR SYSTEM KEY
      // this is importante so tha we do not delete
      // somethign that is needed by the system. Like I did when beta testing :(
      if((key.ICompare("BEOS:APP_SIG") == 0) ||
         (key.ICompare("BEOS") == 0) ||
         (key.ICompare("last_modified") == 0) ||
         (key.ICompare("name") == 0) ||
         (key.ICompare("size") == 0) ||
         (key.ICompare("_trk/qrylastchange") == 0) ||
         (key.ICompare("_trk/recentQuery") == 0) ||
         (key.ICompare("be:deskbar_item_status") == 0) ||
         (key.ICompare("be") == 0)
        ){
         (new BAlert(NULL,"Ahh!\nThis key is a system key. Don't do that!","Oops"))->Go();
         break;
      }
      
      
      // find where the item is
      found = false;
      foundAt = -1;
      while((!found) && (foundAt < (ollv->CountItems()-1))){
         item = (BStringItem*)ollv->ItemAt(++foundAt);
         if(!tmp.Compare(item->Text())){
            found = true;
         }
      }
      
      //make shur its in the list
      if(!found){
         // the item was not in the list. Oops thats not good.
         (new BAlert(NULL,"ERROR:We did not find that Item in the list!","What?"))->Go();
         break;
      }
      
      // Ok so its in the list. its a group/or not and the user is shre, and its not a system key
      
      error = false;
      if(isGroup){
         i = ollv->IndexOf(li);
         j = ollv->CountItemsUnder(li,true);
         for(k = 0; k < j; k++){
             item = (BStringItem*)ollv->ItemUnderAt(li,true,0);
             tmp.SetTo(MainName.String());
             tmp.Append(":");
             tmp.Append(item->Text());
             
             if(RemoveKey(tmp) == B_OK){
                ollv->RemoveItem(item);
             }else{
                error = true;
                (new BAlert(NULL,"ERROR: Could not remove key","Ok"))->Go();
             }
         }
         if(!error){
            ollv->RemoveItem(li);
         }
      }else{
         // do a real remove here 
         if(RemoveKey(key) == B_OK){
            j = ollv->CountItemsUnder(si,true);
            if(j == 1){
               ollv->RemoveItem(si);
            }else{
               ollv->RemoveItem(ollv->ItemAt(foundAt));
            }
         }else{
            error = true;
            (new BAlert(NULL,"ERROR: Could not remove key","Ok"))->Go();
         }
      }
      break;
   case ITEM_SELECTED:
      break;
   default:
      BView::MessageReceived(msg);
   }
}


/*******************************************************
*   
*******************************************************/
status_t IndexView::RemoveKey(BString key){
      /*DO REAL WORK HERE NOW*/
      
      //printf("Removeing key from Volume: %s\n",key.String());
      return fs_remove_index(DeviceNumber,key.String());
      
      /*END OF THE READ WORK */
      //(new BAlert(NULL,key.String(),"Ok"))->Go();
      //return B_OK;
}



