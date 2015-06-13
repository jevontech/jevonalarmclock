#include <time.h>
#include "cmsis_os.h"  
#include <cstring>
//#include "GUI.h"
#include "dialog.h"
#include "AlarmClock.h"


/**
  ******************************************************************************
  * @file    GUI_thread
  * @author  Jeroen Vennegoor op Nijhuis
  * @version V1.0
  * @brief   GUI thread: takes care of updating the LCD
  ******************************************************************************
*/
void GUIThread (void const *argument);              // thread function
osThreadId tid_GUIThread;                           // thread id
osThreadDef (GUIThread, osPriorityIdle, 1, 2048);   // thread object
WM_HWIN hWin;
int GuiUpdate;

extern osMutexId mutex;
extern void StartTimeoutTimer(void);
extern time_t ReadRTC(void);
extern void WriteRTC(time_t t);

extern time_t clocktime;
extern time_t GetAlarmTime(void);
extern int alarmtime_hours;
extern int alarmtime_minutes;
//extern int alarmtime_minutes;
int previousalarmtime_hours;
int previousalarmtime_minutes;
//extern time_t previousalarmtime;
extern int alarmon;

int TextUpdated=1;

char TimeString[5] = "";
//char TimeString[5] = "";
const int BlinkOnTime=600;
const int BlinkOffTime=400;
int BlinkCounter=0;
int BlinkMode=0;
//int PreviousShow=1;
int Show=1;
int PreviousMinute=-1;
char PreviousTimeString[5]="--:--";
int Previousalarmon=-1;
enum DeviceState PreviousState=NORMAL;
int PreviousShow=-1;

//enum DeviceState PreviousState=NORMAL;

extern int GuiNeedsUpdate(void);
extern enum DeviceState deviceState;
extern WM_HWIN CreateWindow(void);
extern int GetUpButtonState(void);
extern int GetDownButtonState(void);
//extern int TimeUpdate;

int Init_GUIThread (void)
{
    tid_GUIThread = osThreadCreate (osThread(GUIThread), NULL);
    if(!tid_GUIThread) return(-1);
	
		GuiUpdate=0;

    return(0);
}

void GUIThread (void const *argument)
{

    GUI_Init(); /* Initialize the Graphics Component */
    hWin=CreateWindow();

    while (1)
    {

			  //osMutexWait( mutex, osWaitForever );
			if(deviceState==SETALARM){
				BlinkMode=1;
			}
			if(deviceState==NORMAL || GetUpButtonState()>0 || GetDownButtonState()>0){
				BlinkMode=0;
				Show=1;	
			}
			
			if(BlinkMode>0){
				if(Show==1){
					if(BlinkCounter>=BlinkOnTime){
							Show=0;
						BlinkCounter=0;
					}
					
				}
				else{
					if(BlinkCounter>=BlinkOffTime){
							Show=1;
						BlinkCounter=0;
					}
					
				}
				BlinkCounter++;
			}
			else{
					Show=1;		
			}
			
		
			
			
			//detect if anything has changed and we need UI update
			//if(PreviousShow!=Show){
			//		GuiUpdate=3;
			//		PreviousShow=Show;
			//}
			//if(PreviousState!=State){
			//		GuiUpdate=3;
			//		PreviousState=State;
			//}
			  //osMutexRelease( mutex );

			//if(GuiUpdate==5){
			if(GuiUpdate==1){
				WM_InvalidateWindow (hWin);
			}
			//	GuiUpdate=0;
			//}
			//else{
			//	GuiUpdate++;
			//}
				
				//if(GuiUpdate>0){
				//	if(GuiUpdate==1){
						
				//		GuiUpdate=0;
				//	}
				//	else{
				//	GuiUpdate--;
				//	}
				//}
				
        GUI_Exec();             /* Execute all GUI jobs ... Return 0 if nothing was done. */
				//osDelay (2);
				//GUI_Delay(100);
				//if(GuiUpdate==1){
				//	GuiUpdate=0;
				//}
        GUI_X_ExecIdle();       /* Nothing left to do for the moment ... Idle processing */
				
       
    }
}

int GuiNeedsUpdate(){
	
	int value=0;
	
	if(alarmon!=Previousalarmon)
		value=1;
	if(deviceState!=PreviousState || Show!=PreviousShow)
	value=1;
	if(TextUpdated==1)
		value=1;
	//if(strcmp (PreviousTimeString,TimeString) != 0 && State==NORMAL)
	//	value=1;
	//if(previousalarmtime!=alarmtime && State==SETALARM)
	//value=1;
	
	return value;
	
}



void UpdateGUI(){
	   // osMutexWait( mutex, osWaitForever );
    clocktime=ReadRTC();
    if(PreviousMinute!=localtime(&clocktime)->tm_min && deviceState==NORMAL)
		//	if(PreviousMinute!=localtime(&rawtime)->tm_sec)
    {
        //UpdateGUI();
        strftime(TimeString, 5, "%H:%M", localtime(&clocktime));
			  //strftime(TimeString, 5, "%M:%S", localtime(&rawtime));
        PreviousMinute=localtime(&clocktime)->tm_min;
				TextUpdated=1;
    }
		if(deviceState!=NORMAL){
			PreviousMinute=-1;
		}
    //osMutexRelease( mutex );

	
			if( (previousalarmtime_hours!=alarmtime_hours || previousalarmtime_minutes!=alarmtime_minutes)  && deviceState==SETALARM){
				time_t time =   GetAlarmTime(); 
				strftime(TimeString, 5, "%H:%M", localtime(&time));
				  previousalarmtime_hours=alarmtime_hours;
				  previousalarmtime_minutes=alarmtime_minutes;
					TextUpdated=1;
			}
		if(deviceState!=SETALARM){
			previousalarmtime_hours=-1;
				previousalarmtime_minutes=-1;
		}
		
		GuiUpdate=GuiNeedsUpdate();
}

void ForceGUIupdate(){
		previousalarmtime_hours=-1;
		previousalarmtime_minutes=-1;
	UpdateGUI();
	
}
