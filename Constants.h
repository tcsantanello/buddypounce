#pragma once

#include <windows.h>

#define szInValid "Invalid Plugin"
const static TCHAR *szAddNewPounce = "Add A New Pounce";
const static TCHAR *szEditExisting = "Edit Existing Pounce";

const static TCHAR *szDonate =
  "https://www.paypal.com/cgi-bin/"
  "webscr?cmd=_xclick&business=tsantane%40nycap%2err%2ecom&item_name=TCSantanello%20-%20Buddy%"
  "20Pounce&item_number=Buddy%20Pounce%20Donation&no_shipping=1&cn=Comments&tax=0&currency_code="
  "USD&charset=UTF%2d8&charset=UTF%2d8";
#define DonateCheckSum 56525

const static short  sOpMethod[]    = { -1, 0, 1 };
const static int    iOperations[]  = { 0, SC_CLOSE, SW_MINIMIZE };
const static TCHAR *szOperations[] = { "Nothing", "Close Window", "Minimize" };

const static TCHAR *szConfigFile  = "BuddyPounce.ini";
static TCHAR *      szPluginTitle = "Buddy Pounce";
static TCHAR *      szPluginName  = szPluginTitle;
static TCHAR *      szPluginDesc  = "Buddy Pounce plugin for Trillian Pro";
static TCHAR *      szEventName   = "Buddy Pounce: Message Sent";
const static TCHAR *szCompany     = "TCSantanello (AIM)";
const static TCHAR *szVersion     = "2.5";
const static TCHAR *szDescription = "Adds Buddy Pounce functionality to Trillian Pro";
const static TCHAR *szFatalError =
  "Buddy Pounce was unable to create the necessary threads\nStartup Aborted!\n";

static TCHAR *szDefaultPrefix =
  "<i><b>This message was saved for you with <A "
  "HREF=\"http://buddypounce.ath.cx/buddy_pounce.zip\">Buddy Pounce</a> for <A "
  "HREF=\"http://www.trillian.cc\">Trillian Pro</A> on %d %n %D, %y at %H:%m:%s %a: </i></b>";
static TCHAR *szDefaultSuffix = "";

const static TCHAR *szAwaystr      = "away";
const static TCHAR *szOnlinestr    = "online";
const static TCHAR *szOfflinestr   = "offline";
const static TCHAR *szInvisiblestr = "invisible";
const static TCHAR *szDNDstr       = "dnd";
const static TCHAR *szMetaContact  = "MetaContact";
const static TCHAR *szMetaAccount  = "Meta-Sender";

static TCHAR *BPGUID     = "{706EAF26-56EB-4eed-AC69-288C22DAD911}";
static TCHAR *MYGUID     = BPGUID;
static TCHAR *szRemoveBP = "buddypounceRemove";

const static TCHAR szSendOn[ 14 ]      = "Only Send On";
const static TCHAR szSendOnAfter[ 14 ] = "Send On/After";

const static TCHAR *szTrillianOfflineMessage =
  "NOTE: This user is offline.  Your messages will most likely *not* be received!";
const static TCHAR *szTrillianICQOfflineMessage =
  "This user is offline.  Your messages will be received when he/she logs into ICQ.";
const static TCHAR *szTrillianOfflineMessage_Error =
  "Error while sending IM: This user is currently not logged on";
const static TCHAR *szBuddyPounceOfflineMessage =
  "NOTE: This user is offline.  Your message will be queued as a Buddy Pounce!";
const static TCHAR *szBuddyPounceOfflineMessage_Error =
  "Message queued; it will be sent when the user comes online.";

const static TCHAR *szTrillianContainer    = "Trillian container";
static TCHAR *      szTrillianPrivMesgBase = "Trillian private ";

static const TCHAR *szNotAvail    = "None available";
static const DWORD  dwSleep       = 1000;
static const DWORD  dwBaseExtSize = 1760;
static const short  sDelThreshold = 10;

#define BP_TIMER 1911

#define WM_UPDATELIST WM_USER + 1

#define MAX_MESSAGE 1025
#define MAX_IGNORE_LIST 1024
#define CUT_MESSAGE_AT 400

#define NANOSECONDS 10000000
#define DAYINNANO 864000000000
#define MENU_REMOVE_POUNCE 9999
