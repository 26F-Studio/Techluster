#!/bin/bash

readonly CMakeDownloadUrl="https://github.com/Kitware/CMake/releases/download/v3.20.5/cmake-3.20.5-linux-x86_64.sh"
readonly InfoColor="\e[0;36m"
readonly SuccessColor="\e[0;32m"
readonly ErrorColor="\e[0;31m"
readonly NoColor="\e[0m"

InterruptProgress(){
    echo -e "${ErrorColor}------- Initialization Failed -------${NoColor}"
    exit 1
}

echo -e "${InfoColor}---------- Update Packages ----------${NoColor}"

if which yum >/dev/null 2>&1;
then
    yum install -y git gcc g++ pkg-config || InterruptProgress
elif which apt >/dev/null 2>&1;
then
    apt install -y git gcc g++ pkg-config || InterruptProgress
elif which pacman >/dev/null 2>&1;
then
    pacman -S git gcc g++ pkg-config || InterruptProgress
fi

echo -e "${InfoColor}--------- Downloading CMake ---------${NoColor}"

cd /tmp || InterruptProgress
wget -q --show-progress --progress=bar:force 2>&1 ${CMakeDownloadUrl} || InterruptProgress

echo -e "${InfoColor}---------- Installing CMake ---------${NoColor}"

chmod +x ./cmake-3.20.5-linux-x86_64.sh || InterruptProgress
sh ./cmake-3.20.5-linux-x86_64.sh -y || InterruptProgress
cp -r ./cmake-3.20.5-linux-x86_64/. /usr || InterruptProgress

echo -e "${InfoColor}--------- Initializing Vcpkg --------${NoColor}"

cd "$(cd "$(dirname "$0")" || exit; pwd)" || InterruptProgress

git submodule update --init --recursive || InterruptProgress

cd vcpkg || InterruptProgress
./bootstrap-vcpkg.sh -disableMetrics || InterruptProgress

#echo -e "${InfoColor}------ Installing Dependencies ------${NoColor}"
#
#./vcpkg --triplet="x64-linux" install || InterruptProgress

echo -e "${SuccessColor}------ Initialization Succeeded -----${NoColor}"
exit 0