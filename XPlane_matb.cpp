#include "XPlane/XPLM/XPLMDisplay.h"
#include "XPlane/XPLM/XPLMGraphics.h"
#include <XPlane/XPLM/XPLMUtilities.h>
#include <XPlane/XPLM/XPLMDataAccess.h>
#include <string.h>
#include <iostream>
#if IBM
#include <windows.h>
#endif
#if LIN
#include <GL/gl.h>
#elif __GNUC__
#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif

#ifndef XPLM300
#error This is made to be compiled against the XPLM300 SDK
#endif

static XPLMKeyFlags	gFlags = 0;
static char			gVirtualKey = 0;
static char			gChar = 0;



static int MyKeySniffer(
        char                 inChar,
        XPLMKeyFlags         inFlags,
        char                 inVirtualKey,
        void *               inRefcon);

PLUGIN_API int XPluginStart(
        char *		outName,
        char *		outSig,
        char *		outDesc)
{
    strcpy(outName, "XPlane MATB");
    strcpy(outSig, "This is a MATB Hello World Plugin");
    strcpy(outDesc, "MATB program");

    XPLMRegisterKeySniffer(
            MyKeySniffer, 				/* Our callback. */
            1, 			/* Receive input before plugin windows. */
            0);					/* Refcon - not used. */

    return 1;


}

PLUGIN_API void	XPluginStop(void)
{
    // Since we created the window, we'll be good citizens and clean it up

}

PLUGIN_API void XPluginDisable(void) { }
PLUGIN_API int  XPluginEnable(void)  { return 1; }
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMsg, void * inParam) {}

int MyKeySniffer(
        char                 inChar,
        XPLMKeyFlags         inFlags,
        char                 inVirtualKey,
        void *               inRefcon)
{
    /* First record the key data. */
    gVirtualKey = inVirtualKey;
    gFlags = inFlags;
    gChar = inChar;
    if ((int)gChar == 66 && (gFlags & xplm_ShiftFlag) && (gFlags & xplm_UpFlag)){
//        std::cout << "Key shift b activated" << std::endl;
        XPLMSetDataf(XPLMFindDataRef("sim/weather/rain_percent"), 0.9);
        XPLMSetDataf(XPLMFindDataRef("sim/weather/thunderstorm_percent"), 0.9);

    }
    return 1; // Should be 1, else other keys won't work
}


