/*******************************************************
*   Index©
*
*   @author  YNOP (ynop@acm.org)
*   @version beta
*   @date    Dec 17 1999
*******************************************************/
#include <AppKit.h>
#include <KernelKit.h>

#include "Index.h"

// Application's signature
const char *APP_SIGNATURE = "application/x-vnd.Abstract-Index";

/*******************************************************
*   Main app. So launch off that ImageShow and lets
*   get down to buizness
*******************************************************/
int main(){
   Index *app = new Index();

   app->Run();
   delete app;
   return B_NO_ERROR;
}