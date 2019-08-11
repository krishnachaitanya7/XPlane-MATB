#include "XPlane/XPLM/XPLMDisplay.h"
#include "XPlane/XPLM/XPLMGraphics.h"
#include <XPlane/XPLM/XPLMUtilities.h>
#include <XPlane/XPLM/XPLMDataAccess.h>
#include <XPlane/XPLM/XPLMProcessing.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <regex>
#include <unistd.h>
#include <ctime>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <vector>
#include <fstream>
#define PORT 50000
#define DTTMFMT "%Y-%m-%d %H:%M:%S "
#define DTTMSZ 21
#if LIN
#include <GL/gl.h>
#include <sys/socket.h>
#include <netinet/in.h>

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
static std::vector<std::string> actions;
std::string low_difficulty {"Low Difficulty"};
std::string moderate_difficulty {"Moderate Difficulty"};
std::string high_difficulty {"High Difficulty"};
std::string insert_tlx {"Do TLX Activity"};
int rain_ld, wind_ld, duration_ld, rain_md, wind_md, duration_md, rain_hd, wind_hd, duration_hd;
std::string day_night_ld, day_night_md, day_night_hd;
const float min_cruise_height {500};
static std::fstream log_file;
static std::string plugin_log_file = "ShineLabPlugin_log.txt";
void add_actions();
bool change_weather(int &rain, int &wind, int &duration_time, std::string &day_or_night);
static char *getDtTm (char *buff);
void write_to_log(std::string &write_text);
void sleep_for_me(int &duration);
void sleep_for_me(int &&duration);
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

    log_file.open(plugin_log_file, std::fstream::in | std::fstream::out | std::fstream::app);
    if (!log_file )
    {
        std::cout << "Cannot open file, file does not exist. Creating new file..";

        log_file.open(plugin_log_file,  std::fstream::in | std::fstream::out | std::fstream::trunc);
        log_file.close();

    }
    log_file <<"---------------------------Start of new log----------------------------------------" << std::endl;
    log_file.close();
    XPLMRegisterKeySniffer(
            MyKeySniffer, 				/* Our callback. */
            1, 			/* Receive input before plugin windows. */
            0);					/* Refcon - not used. */
    sleep_for_me(1);
    return 1;
}

PLUGIN_API void	XPluginStop(void){}
PLUGIN_API void XPluginDisable(void) { }
PLUGIN_API int  XPluginEnable(void)  { return 1; }
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMsg, void * inParam) {}

void add_actions(){
    std::ifstream in("XPlane.conf");
    if (!in.good()){
        std::cout << "Unable to read file" << std::endl;
    }
    std::string str;
    std::regex rgx("((l|m|h)d) Rain % is (\\d*) Wind % is (\\d*) for (\\d*) seconds Time of the day is (Day|Night)");
    std::smatch matches;


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
                        actions.push_back(low_difficulty);
//                        change_weather(rain_ld, wind_ld, duration_ld, day_night_ld);

                    } else if(str == moderate_difficulty){
                        actions.push_back(moderate_difficulty);
//                        change_weather(rain_md, wind_md, duration_md, day_night_md);

                    } else if(str == high_difficulty){
                        actions.push_back(high_difficulty);
//                        change_weather(rain_hd, wind_hd, duration_hd, day_night_hd);

                    } else if(str == insert_tlx){
                        actions.push_back(insert_tlx);
                    } else{
                        std::cout << "Only God knows what this string is: " << str << std::endl;
                    }
                } else{
                    std::cout << "The config file doesn't match requirements" << std::endl;
                }
            }

        }
    }
}

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
        if (!actions.empty()) {
            std::string present_action = actions.front();
            actions.erase(actions.begin());
            if (present_action == low_difficulty) {
                write_to_log(low_difficulty);
                std::cout << "Changing weather to: " << low_difficulty << std::endl;
                change_weather(rain_ld, wind_ld, duration_ld, day_night_ld);

            } else if (present_action == moderate_difficulty) {
                write_to_log(moderate_difficulty);
                std::cout << "Changing weather to: " << moderate_difficulty << std::endl;
                change_weather(rain_md, wind_md, duration_md, day_night_md);
            } else if (present_action == high_difficulty) {
                write_to_log(high_difficulty);
                std::cout << "Changing weather to: " << high_difficulty << std::endl;
                change_weather(rain_hd, wind_hd, duration_hd, day_night_hd);
            } else if(present_action == insert_tlx){
                write_to_log(insert_tlx);
                std::cout << "Please Do NASA TLX from IPAD " << std::endl;
                XPLMSetDatai(XPLMFindDataRef("sim/time/sim_speed"), 0);

            }
        } else{
            std::cout << "Weather Changing Complete. So starting again" << std::endl;
            add_actions();
            sleep_for_me(10);

        }
    }
    else if ((int)gChar == 86 && (gFlags & xplm_ShiftFlag) && (gFlags & xplm_UpFlag)){
        float current_height = XPLMGetDataf(XPLMFindDataRef("sim/flightmodel/misc/h_ind"));
        if (current_height > min_cruise_height){
            std::cout << "The cruise height reached is: " << current_height << std::endl;
            std::string write_this_to_log = "Minimum Cruise level Achieved: "+std::to_string(current_height);
            write_to_log(write_this_to_log);
            sleep_for_me(2);
        } else{
            std::cout << "The height is: " << current_height << std::endl;
            sleep_for_me(1);
        }
    }
    return 1; // Should be 1, else other keys won't work
}
void sleep_for_me(int &&duration){
    sleep_for_me(duration);
}
void sleep_for_me(int &duration){
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    std::string s = std::to_string(duration);
    const char *hello = s.c_str();
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");

    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");

    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
    }
    send(sock , hello , strlen(hello) , 0 );

}

bool change_weather(int &rain, int &wind, int &duration_time, std::string &day_or_night){
    float rain_percent = (float)rain / (float)100;
    float wind_percent = (float)wind / (float)100;
    XPLMSetDataf(XPLMFindDataRef("sim/weather/rain_percent"), rain_percent);
    XPLMSetDataf(XPLMFindDataRef("sim/weather/thunderstorm_percent"), wind_percent);
    XPLMSetDatai(XPLMFindDataRef("sim/weather/cloud_type[0]"), 5);
    sleep_for_me(duration_time);
    return true;
}

static char *getDtTm (char *buff) {
    time_t t = time (0);
    strftime (buff, DTTMSZ, DTTMFMT, localtime (&t));
    return buff;
}

void write_to_log(std::string &write_text){
    log_file.open(plugin_log_file, std::fstream::in | std::fstream::out | std::fstream::app);
    char buff[DTTMSZ];
    log_file << getDtTm (buff) << write_text << std::endl;
    log_file.close();
}