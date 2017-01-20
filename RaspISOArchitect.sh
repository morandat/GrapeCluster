#!/bin/bash

############################Globals############################
LASTANSWER=""
IMAGE_NAME=""
WORKING_PATH=""
NUMBER_REGEX='^[0-9]+$'
QUESTIONSRANGE=0

###############################################################
#############################################################################################################
############################Used to ask a question and waiting answer true or false##########################
function askQuestion(){
    echo -n "$1 (y/n) "
    read answer
    while [ "$answer" != "y" ] && [ "$answer" != "n" ] && [ "$answer" != "Y" ] && [ "$answer" != "N" ]
    do
        echo "Bad anwser, retry or quit (Ctrl-C or q)"
        echo $1
        read answer
        if [ "$answer" == "q" ] 
        then
            echo "Exitting"
            exit 1 
        fi
    done

    if [ "$answer" == "y" ] || [ "$answer" == "Y" ]
    then
        LASTANSWER=true
    elif [ "$answer" == "n" ] || [ "$answer" == "N" ]
    then
        LASTANSWER=false
    fi
}

function askContinue(){
    askQuestion "Do you wish to continue ?"
    if [ $LASTANSWER == false ]
    then
        echo "Exitting ..."
        exit
    fi
}

function askNumberQuestion(){
    local range=$1
    read answer
    while ! [[ $answer =~ $NUMBER_REGEX ]]
    do
        echo "Bad anwser, retry or quit (Ctrl-C or q)"
        echo $1
        read answer
        if [ "$answer" == "q" ] 
        then
            echo "Exitting"
            exit 1 
        fi
    done
    while [ "$answer" -gt "$range" ] || [ "$answer" -lt "$((1))" ]
    do
        echo "Bad anwser, retry or quit (Ctrl-C or q)"
        echo $1
        read answer
        if [ "$answer" == "q" ] 
        then
            echo "Exitting"
            exit 1 
        fi
    done

    echo "$answer"
}
################################################################################################################
########################################-help function##########################################################
function usage(){
    if [ "$1" == "launch" ]
    then
        echo "Usage : RaspISOArchitect /path/to/image/or/iso";
    fi
}
################################################################################################################
########################################To handle options ######################################################
function handleOptions(){
    while [ $# -gt 0 ]
    do  
        case "$1" in
            --help) usage "launch"; exit 0
            ;;
            -h) usage "launch"; exit 0
            ;;
            -*) echo "Could'nt recognize option, exitting ..."; exit 0
            ;;
            --*) echo "Could'nt recognize option, exitting ..."; exit 0
            ;;
        esac
        shift
    done
}

function showMenu(){
    case "$1" in
            main) echo "Welcome on RaspISO(IMG)Architect !"; echo "You gave the image $IMAGE_NAME";
                echo "1. Mount image manually (requires root password)";
                echo "2. Use kpartx (install it if not installed) to mount image";
                echo -n "Your choice : ";
                ;;
    esac
} >&2

function menu(){
    showMenu $1
    echo $(askNumberQuestion 2);
}

################################################################################################################

function mountImage(){
    local image=$1
    local part=`fdisk -l -o Start $image | cut -d' ' -f1,3 | tail -n1`  
    WORKING_PATH="/tmp/raspiso/$IMAGE_NAME"

    echo $part
    mkdir -p $WORKING_PATH
    sudo mount -v -o offset=$((512*$part)) -t ext4 $image $WORKING_PATH

    ls $WORKING_PATH
}
################################################################################################################


if [ -z "$1" ]
then
    usage "launch"
    exit 1;
fi

if [ "$EUID" -ne 0 ]
  then 
    echo "Please run as root. This script is going to mount, modify and unmount ISO images in /tmp/raspiso."
    echo "No modification to the system will be done."
    exit
fi

handleOptions $@

fullfile=`basename $1`
IMAGE_NAME="${fullfile%.*}"


choice="`menu main`"
echo $choice

mountImage $1

while [ true ]
do
    read cmd
    $cmd
done