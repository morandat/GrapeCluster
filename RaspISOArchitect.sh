#!/bin/bash

############################Globals############################
LASTANSWER=""
IMAGE_NAME=""
WORKING_PATH=""
NUMBER_REGEX='^[0-9]+$'

###############################################################
#############################################################################################################
############################Used to ask a question and waiting answer true or false##########################
function askQuestion(){
    echo "$1 (y/n)"
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

function askNumberQuestion(){
    local range=$1
    echo $range
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

function menu(){
    case "$1" in
        main) echo "Welcome on RaspISO(IMG)Architect !"; echo "You gave the image $IMAGE_NAME";
              echo "1. Mount image manually (requires root password)";
              echo "2. Use kpartx (install it if not installed) to mount image";
              echo -n "Your choice : ";
              echo $(askNumberQuestion 2);
              ;;
    esac
}
################################################################################################################

function mountImage(){
    local image=$1
    local part=`fdisk -l -o Start $image | cut -d' ' -f1,3 | tail -n1`  
    WORKING_PATH="/tmp/raspiso/$IMAGE_NAME"

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

handleOptions $@

IMAGE_NAME=`basename $1`


menu "main"

#mountImage $1