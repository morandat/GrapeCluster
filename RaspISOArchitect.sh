#!/bin/bash

############################Globals############################
LASTANSWER=""

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
            *) echo "Could'nt recognize option, exitting ..."; exit 0
            ;;
            --*) echo "Could'nt recognize option, exitting ..."; exit 0
            ;;
        esac
        shift
    done
}
################################################################################################################
################################################################################################################


echo "Welcome on RaspISO(IMG)Architect !"
if [ -z "$1" ]
then
    usage "launch"
    exit 1;
fi

handleOptions $@