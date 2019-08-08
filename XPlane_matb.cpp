#include "XPlane/XPLM/XPLMDisplay.h"
#include "XPlane/XPLM/XPLMGraphics.h"
#include <XPlane/XPLM/XPLMUtilities.h>
#include <XPlane/XPLM/XPLMDataAccess.h>
#include <XPlane/XPLM/XPLMProcessing.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <regex>
#include <chrono>
#include <thread>
#include <future>
#include <unistd.h>
#include <ctime>
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

bool change_weather(int rain_percent, int wind_percent, int duration_time, std::string day_or_night);
void sleep_for_me(int duration);
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
    std::ifstream in("XPlane.conf");
    if (!in.good()){
        std::cout << "Unable to read file" << std::endl;
    }
    std::string str;
    std::regex rgx("((l|m|h)d) Rain % is (\\d*) Wind % is (\\d*) for (\\d*) seconds Time of the day is (Day|Night)");
    std::smatch matches;
    std::string low_difficulty {"Low Difficulty"};
    std::string moderate_difficulty {"Moderate Difficulty"};
    std::string high_difficulty {"High Difficulty"};
    std::string insert_tlx {"Do TLX Activity"};
    int rain_ld, wind_ld, duration_ld, rain_md, wind_md, duration_md, rain_hd, wind_hd, duration_hd;
    std::string day_night_ld, day_night_md, day_night_hd;

    if ((int)gChar == 66 && (gFlags & xplm_ShiftFlag) && (gFlags & xplm_UpFlag)){
        while (std::getline(in, str)) {
            if (str.size() > 0) {
                if(std::regex_search(str, matches, rgx)){
                    if(matches[1] == "ld"){
                        rain_ld = std::stoi(matches[3]);
                        wind_ld = std::stoi(matches[4]);
                        duration_ld = std::stoi(matches[5]);
                        day_night_ld = matches[6];
                    } else if(matches[1] == "md"){
                        rain_md = std::stoi(matches[3]);
                        wind_md = std::stoi(matches[4]);
                        duration_md = std::stoi(matches[5]);
                        day_night_md = matches[6];
                    } else if(matches[1] == "hd"){
                        rain_hd = std::stoi(matches[3]);
                        wind_hd = std::stoi(matches[4]);
                        duration_hd = std::stoi(matches[5]);
                        day_night_hd = matches[6];

                    }
                } else{
                    if(str == low_difficulty || str == moderate_difficulty || str == high_difficulty || str == insert_tlx){
                        if(str == low_difficulty){
                            change_weather(rain_ld, wind_ld, duration_ld, day_night_ld);

                        } else if(str == moderate_difficulty){
                            change_weather(rain_md, wind_md, duration_md, day_night_md);

                        } else if(str == high_difficulty){
                            change_weather(rain_hd, wind_hd, duration_hd, day_night_hd);

                        } else{
                            XPLMSetDatai(XPLMFindDataRef("sim/time/sim_speed"), 0); //Pause Sim
                        }
                    } else{
                        std::cout << "The config file doesn't match requirements" << std::endl;
                    }
                }

            }
        }
    }
    return 1; // Should be 1, else other keys won't work
}

void sleep_for_me(int duration){
    std::cout << "Sleeping for " << duration << " seconds" << std::endl;
    float end_time = XPLMGetElapsedTime() + (float)duration;
    while (end_time > XPLMGetElapsedTime());
}

bool change_weather(int rain, int wind, int duration_time, std::string day_or_night){
    float rain_percent = (float)rain / (float)100;
    float wind_percent = (float)wind / (float)100;
    XPLMSetDataf(XPLMFindDataRef("sim/weather/rain_percent"), rain_percent);
    XPLMSetDataf(XPLMFindDataRef("sim/weather/thunderstorm_percent"), wind_percent);
    XPLMSetDatai(XPLMFindDataRef("sim/weather/cloud_type[0]"), 5);
    sleep_for_me(duration_time);
//    std::thread t1(sleep_for_me, duration_time);
//    t1.join();
    return true;
}




