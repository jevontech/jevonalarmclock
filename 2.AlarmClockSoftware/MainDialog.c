#include <cstring>
#include "cmsis_os.h"
#include "AlarmClock.h"
#include "DIALOG.h"
#include "AlarmClockImage.h"

extern char TimeString[5];
extern int alarmOn;
extern enum DeviceState deviceState;
extern int show;
extern int previousAlarmOn;
extern enum DeviceState previousState;
extern int previousShow;
extern int textUpdated;

/*********************************************************************
 *
 *       Defines
 *
 **********************************************************************
 */
#define ID_WINDOW_0    (GUI_ID_USER + 0x00)
#define ID_TEXT_0    (GUI_ID_USER + 0x01)
#define ID_IMAGE_0  (GUI_ID_USER + 0x02)

#define ID_IMAGE_0_IMAGE_0  0x00

// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
 *
 *       Static data
 *
 **********************************************************************
 */

// USER START (Optionally insert additional static data)
// USER END
/*********************************************************************
 *
 *       _aDialogCreate
 */
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
		{ WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 0, 320, 240, 0, 0x0,
				0 }, { TEXT_CreateIndirect, "Time", ID_TEXT_0, 2, 67, 310, 99,
				0, 0x64, 0 }, { IMAGE_CreateIndirect, "Image", ID_IMAGE_0, 250,
				170, 64, 64, 0, 0, 0 }, };

/*********************************************************************
 *
 *       Static code
 *
 **********************************************************************
 */

static const void * _GetImageById(U32 Id, U32 * pSize) {
	switch (Id) {
	case ID_IMAGE_0_IMAGE_0:
		*pSize = sizeof(_acImage_0);
		return (const void *) _acImage_0;
	}
	return NULL;
}

/*********************************************************************
 *
 *       _cbDialog
 */

static void _cbDialog(WM_MESSAGE * pMsg) {
	const void * pData;
	WM_HWIN hItem;
	U32 FileSize;

	switch (pMsg->MsgId) {
	case WM_INIT_DIALOG:
		hItem = pMsg->hWin;
		WINDOW_SetBkColor(hItem, 0x00000000);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
		TEXT_SetText(hItem, "00:00");
		TEXT_SetFont(hItem, GUI_FONT_D60X80);
		TEXT_SetTextColor(hItem, 0x004080FF);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
		pData = _GetImageById(ID_IMAGE_0_IMAGE_0, &FileSize);
		IMAGE_SetBMP(hItem, pData, FileSize);
		break;
	case WM_PAINT:

		//TODO: use Memory Device to avoid flickering
		if (deviceState != previousState || show != previousShow) {
			if (deviceState == NORMAL) {
				hItem = pMsg->hWin;
				WINDOW_SetBkColor(hItem, 0x00000000);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
				TEXT_SetTextColor(hItem, 0x004080FF);
			}

			if (deviceState == SETALARM) {
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
				TEXT_SetTextColor(hItem, GUI_LIGHTBLUE);

			}

			if (deviceState == ALARM) {
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
				if (show == 1) {
					hItem = pMsg->hWin;
					WINDOW_SetBkColor(hItem, 0x00000000);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
					TEXT_SetTextColor(hItem, GUI_YELLOW);
				} else {
					hItem = pMsg->hWin;
					WINDOW_SetBkColor(hItem, GUI_YELLOW);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
					TEXT_SetTextColor(hItem, 0x004080FF);
				}
			}

			if (show == 1) {
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
				WM_ShowWindow(hItem);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
				WM_ShowWindow(hItem);
			} else {
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
				WM_HideWindow(hItem);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
				WM_HideWindow(hItem);
			}

			previousState = deviceState;
			previousShow = show;
		}

		if (alarmOn != previousAlarmOn) {
			if (alarmOn == 1) {
				hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
				WM_ShowWindow(hItem);
			} else {
				hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
				WM_HideWindow(hItem);
			}
			previousAlarmOn = alarmOn;
		}

		if (textUpdated == 1) {
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
			TEXT_SetText(hItem, TimeString);
			textUpdated = 0;
		}

	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

/*********************************************************************
 *
 *       Public code
 *
 **********************************************************************
 */
/*********************************************************************
 *
 *       CreateWindow
 */
WM_HWIN CreateWindow(void);
WM_HWIN CreateWindow(void) {
	WM_HWIN hWin;

	hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate),
			_cbDialog, WM_HBKWIN, 0, 0);
	return hWin;
}

// USER START (Optionally insert additional public code)

// USER END

/*************************** End of file ****************************/
