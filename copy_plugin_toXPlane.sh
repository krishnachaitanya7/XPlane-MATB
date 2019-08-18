#!/bin/bash
executable_name="cmake-build-debug/qt_xplane_test"
XPlane_Directory="$HOME/X-Plane 11/"
#aircraft_folder="Aircraft/Laminar Research/Cessna 172SP/plugins/qt_xplane_test/64/"
aircraft_folder="Aircraft/Laminar Research/FA-18F/plugins/qt_xplane_test/64/"
plugin_name="lin.xpl"
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
echo $DIR
cd $DIR
if [[ -f "$XPlane_Directory$aircraft_folder$plugin_name" ]]; then
    rm "$XPlane_Directory$aircraft_folder$plugin_name"
fi
cp "$executable_name" "$XPlane_Directory$aircraft_folder$plugin_name"