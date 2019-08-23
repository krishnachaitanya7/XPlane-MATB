#include "XPlane/XPLM/XPLMDisplay.h"
//#include "XPlane/XPLM/XPLMGraphics.h"
//#include <XPlane/XPLM/XPLMUtilities.h>
#include <XPlane/XPLM/XPLMDataAccess.h>
#include <XPlane/XPLM/XPLMUtilities.h>
#include <XPlane/XPLM/XPLMPlanes.h>
#include <XPlane/XPLM/XPLMProcessing.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <regex>
#include <ctime>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <vector>
#include "rest_dialog.h"
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#define PORT 50000
#define DTTMFMT "%Y-%m-%d %H:%M:%S"
#define DTTMSZ 21
#if LIN
#include <GL/gl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <QtWidgets/QApplication>

#elif __GNUC__
#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif

#ifndef XPLM300
#error This is made to be compiled against the XPLM300 SDK
#endif
// Namespaces
namespace pt = boost::property_tree;
// Start Global Static Variables
static std::string wind_percent_string {"Wind_Percent"};
static std::string rain_percent_string {"Rain_Percent"};
static std::string duration_string {"Duration"};
static std::string day_or_night_string {"Day_or_Night"};
static std::string external_manipulation_string {"External_Manipulation"};
static std::string departure_airport_string {"Departure_Airport"};
static std::string arrival_airport_string {"Arrival_Airport"};
static std::string airport_code_string {"Airport_Code"};
static std::string airport_height_string {"Airport_Base_Height"};
static std::string action_list_string {"Actions_List"};
std::string homeDir;
static XPLMKeyFlags	gFlags = 0;
static char	gVirtualKey = 0;
static char	gChar = 0;
static std::vector<std::string> actions;
std::string low_difficulty {"Low Difficulty"};
std::string moderate_difficulty {"Moderate Difficulty"};
std::string high_difficulty {"High Difficulty"};
std::string insert_tlx {"Fill Out Survey"};
bool rest_next {false};
int rain_ld, wind_ld, duration_ld, rain_md, wind_md, duration_md, rain_hd, wind_hd, duration_hd;
std::string day_night_ld, day_night_md, day_night_hd;
std::string external_manipulation_ld, external_manipulation_md, external_manipulation_hd;
float min_cruise_height;
int start_sim_immediately {10000};
static std::fstream log_file;
std::string plugin_log_file {""};
std::string current_config_file;
static int rest_time {0};
bool aircraftloaded {false};
std::string departing_airport;
void add_actions();
void set_airport();
void change_weather(int &rain, int &wind, int &duration_time, std::string &day_or_night);
static char *getDtTm (char *buff);
void write_to_log(std::string &write_text);
void write_to_log(std::string &&write_text);
void sleep_for_me(int &duration);
void sleep_for_me(int &&duration);
int start_rest_screen(int &countdown_seconds);
void show_rest_screen(int &countdown_seconds);
std::string get_config_file();
float DefaultAircraftLoopCB(float elapsedMe, float elapsedSim, int counter, void * refcon);

template <typename T> std::vector<T> as_vector(pt::ptree const& pt, pt::ptree::key_type const& key){
    std::vector<T> r;
    for (auto& item : pt.get_child(key))
        r.push_back(item.second.get_value<T>());
    return r;
}

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

    current_config_file = get_config_file();
    char buff[DTTMSZ];
    plugin_log_file = "";
    plugin_log_file += getDtTm(buff);
    plugin_log_file += "_" + current_config_file + ".log";
    log_file.open(plugin_log_file, std::fstream::in | std::fstream::out | std::fstream::app);
    homeDir = getenv("HOME");
    homeDir += "/";
    if (!log_file )
    {
        std::cout << "Cannot open file, file does not exist. Creating new file..";
        log_file.open(plugin_log_file,  std::fstream::in | std::fstream::out | std::fstream::trunc);
        log_file.close();

    }
    log_file << "---------------------------Start of new log----------------------------------------" << std::endl;
    log_file.close();
    set_airport();
    XPLMRegisterFlightLoopCallback(DefaultAircraftLoopCB, 1.0, NULL);
    XPLMRegisterKeySniffer(
            MyKeySniffer, 				/* Our callback. */
            1, 			/* Receive input before plugin windows. */
            0);					/* Refcon - not used. */
    sleep_for_me(1);
    return 1;
}

PLUGIN_API void	XPluginStop(void){
    actions.clear();
}
PLUGIN_API void XPluginDisable(void) {}
PLUGIN_API int  XPluginEnable(void)  { return 1; }
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMsg, void * inParam){}

void set_airport(){
    std::ifstream in(current_config_file);
    if (!in.good()){
        std::cout << "Unable to read file for airport data: "<< current_config_file << std::endl;
    }
    pt::ptree root;
    pt::read_json(current_config_file, root);
    for (pt::ptree::value_type &each_element : root) {
        std::string element_name = each_element.first;
        if (element_name == departure_airport_string) {
            for (pt::ptree::value_type &second_layer_elements : root.get_child(element_name)) {
                if (second_layer_elements.first == airport_code_string) {
                    departing_airport = second_layer_elements.second.data();

                } else if(second_layer_elements.first == airport_height_string){
                        min_cruise_height = std::stoi(second_layer_elements.second.data()) + 500;

                }
            }
        }
    }
}

void add_actions(){
    std::string filename_in_stdstring = current_config_file;
    pt::ptree root;
    pt::read_json(filename_in_stdstring, root);
    for (pt::ptree::value_type &each_element : root) {
        std::string element_name = each_element.first;
        if(element_name == low_difficulty){
            for(pt::ptree::value_type &second_layer_elements : root.get_child(element_name)){
                if(second_layer_elements.first == wind_percent_string){
                    int wind_percent {std::stoi(second_layer_elements.second.data())};
                    wind_ld = wind_percent;
                } else if(second_layer_elements.first == rain_percent_string){
                    int rain_percent {std::stoi(second_layer_elements.second.data())};
                    rain_ld = rain_percent;
                } else if(second_layer_elements.first == duration_string){
                    int duration {std::stoi(second_layer_elements.second.data())};
                    duration_ld = duration;
                } else if(second_layer_elements.first == day_or_night_string){
                    day_night_ld = second_layer_elements.second.data();
                } else if(second_layer_elements.first == external_manipulation_string){
                    external_manipulation_ld = second_layer_elements.second.data();
                }
            }
        } else if(element_name == moderate_difficulty){
            for(pt::ptree::value_type &second_layer_elements : root.get_child(element_name)){
                if(second_layer_elements.first == wind_percent_string){
                    int wind_percent {std::stoi(second_layer_elements.second.data())};
                    wind_md = wind_percent;
                } else if(second_layer_elements.first == rain_percent_string){
                    int rain_percent {std::stoi(second_layer_elements.second.data())};
                    rain_md = rain_percent;
                } else if(second_layer_elements.first == duration_string){
                    int duration {std::stoi(second_layer_elements.second.data())};
                    duration_md = duration;
                } else if(second_layer_elements.first == day_or_night_string){
                    day_night_md = second_layer_elements.second.data();
                } else if(second_layer_elements.first == external_manipulation_string){
                    external_manipulation_md = second_layer_elements.second.data();
                }
            }
        } else if(element_name == high_difficulty){
            for(pt::ptree::value_type &second_layer_elements : root.get_child(element_name)){
                if(second_layer_elements.first == wind_percent_string){
                    int wind_percent {std::stoi(second_layer_elements.second.data())};
                    wind_hd = wind_percent;
                } else if(second_layer_elements.first == rain_percent_string){
                    int rain_percent {std::stoi(second_layer_elements.second.data())};
                    rain_hd = rain_percent;
                } else if(second_layer_elements.first == duration_string){
                    int duration {std::stoi(second_layer_elements.second.data())};
                    duration_hd = duration;
                } else if(second_layer_elements.first == day_or_night_string){
                    day_night_hd = second_layer_elements.second.data();
                } else if(second_layer_elements.first == external_manipulation_string){
                    external_manipulation_hd = second_layer_elements.second.data();
                }
            }
        }
    }
    actions = as_vector<std::string>(root, action_list_string);
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
        if(rest_next){
            show_rest_screen(rest_time);
            rest_next = false;
        } else {
            if (!actions.empty()) {
                std::string present_action = actions.front();
                actions.erase(actions.begin());
                if (present_action == low_difficulty) {
                    write_to_log(low_difficulty + " External Manipulation: "+external_manipulation_ld);
                    std::cout << "Changing weather to: " << low_difficulty + " External Manipulation: "+external_manipulation_ld << std::endl;
                    change_weather(rain_ld, wind_ld, duration_ld, day_night_ld);
                    rest_next = !(actions.front() == insert_tlx);
                } else if (present_action == moderate_difficulty) {
                    write_to_log(moderate_difficulty + " External Manipulation: "+external_manipulation_md);
                    std::cout << "Changing weather to: " << moderate_difficulty + " External Manipulation: "+external_manipulation_md << std::endl;
                    change_weather(rain_md, wind_md, duration_md, day_night_md);
                    rest_next = !(actions.front() == insert_tlx);
                } else if (present_action == high_difficulty) {
                    write_to_log(high_difficulty + " External Manipulation: "+external_manipulation_hd);
                    std::cout << "Changing weather to: " << high_difficulty + " External Manipulation: "+external_manipulation_hd << std::endl;
                    change_weather(rain_hd, wind_hd, duration_hd, day_night_hd);
                    rest_next = !(actions.front() == insert_tlx);
                } else if (present_action == insert_tlx) {
                    write_to_log(insert_tlx);
                    std::cout << "Please Complete the Survey" << std::endl;
                    XPLMCommandOnce(XPLMFindCommand("sim/operation/pause_toggle"));
                }
            } else {
                std::cout << "Weather Changing Complete.Add your thank you note here" << std::endl;
            }
        }
    }
    else if ((int)gChar == 86 and (gFlags & xplm_ShiftFlag) and (gFlags & xplm_UpFlag)){
        float current_height = XPLMGetDataf(XPLMFindDataRef("sim/flightmodel/misc/h_ind"));
        if (current_height > min_cruise_height){
            std::cout << "The cruise height reached is: " << current_height << std::endl;
            write_to_log("Minimum Cruise level Achieved: "+std::to_string(current_height));
            add_actions();
            sleep_for_me(2);
        } else{
            std::cout << "The height is: " << current_height << std::endl;
            sleep_for_me(1);
        }
    }
    else if ((int)gChar == 80 and (gFlags & xplm_ShiftFlag) and (gFlags & xplm_UpFlag)){
        // Shift P
        show_rest_screen(rest_time);
    }
    return 1; // Should be 1, else other keys won't work
}

int start_rest_screen(int &countdown_seconds){
    int argc {1};
    char test_argv {'Q'};
    char *y = &test_argv;
    char **argv = &y;
    QApplication a(argc, argv);
    rest_dialog r(nullptr, countdown_seconds);
    r.show();
    return r.exec();
}

void show_rest_screen(int &countdown_seconds){
    if(XPLMGetDatai(XPLMFindDataRef("sim/time/sim_speed")) != 0){
        XPLMCommandOnce(XPLMFindCommand("sim/operation/pause_toggle"));
    }
    start_rest_screen(countdown_seconds);
    XPLMCommandOnce(XPLMFindCommand("sim/operation/pause_toggle"));
    sleep_for_me(start_sim_immediately);
}

void sleep_for_me(int &&duration){
    sleep_for_me(duration);
}
void sleep_for_me(int &duration){
    int sock = 0;
    struct sockaddr_in serv_addr;
    std::string s = std::to_string(duration);
    const char *hello = s.c_str();
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
        sleep_for_me(duration);
    }
    send(sock , hello , strlen(hello) , 0 );

}

void change_weather(int &rain, int &wind, int &duration_time, std::string &day_or_night){
    float rain_percent = (float)rain / (float)100;
    float wind_percent = (float)wind / (float)100;
    XPLMSetDataf(XPLMFindDataRef("sim/weather/rain_percent"), rain_percent);
    XPLMSetDataf(XPLMFindDataRef("sim/weather/thunderstorm_percent"), wind_percent);
    XPLMSetDatai(XPLMFindDataRef("sim/weather/cloud_type[0]"), 5);
    sleep_for_me(duration_time);
}

static char *getDtTm (char *buff) {
    time_t t = time (nullptr);
    strftime (buff, DTTMSZ, DTTMFMT, localtime (&t));
    return buff;
}

void write_to_log(std::string &&write_text){
    write_to_log(write_text);
}

void write_to_log(std::string &write_text){
    log_file.open(plugin_log_file, std::fstream::in | std::fstream::out | std::fstream::app);
    char buff[DTTMSZ];
    log_file << getDtTm (buff) << " " << write_text << std::endl;
    log_file.close();
}

float DefaultAircraftLoopCB(float elapsedMe, float elapsedSim, int counter, void * refcon){
    if(!aircraftloaded){
        XPLMPlaceUserAtAirport(departing_airport.c_str());
        aircraftloaded = true;
    }
    return 1.0;
}

std::string get_config_file(){
    const std::string &extension = ".json";
    const std::string path {"."};
    boost::filesystem::path dir(path);
    if(boost::filesystem::exists(path) && boost::filesystem::is_directory(path)){
        boost::filesystem::directory_iterator it(path);
        boost::filesystem::directory_iterator endit;
        while (it != endit) {
            if(boost::filesystem::is_regular_file(*it) && (extension=="")?true:it->path().extension() == extension) {
//                return it->path().string();
                boost::filesystem::path p(it->path().string());
                return p.filename().string();
            }
            ++it;
        }

    }
}