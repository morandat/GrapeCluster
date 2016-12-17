#!/bin/bash

LASTANSWER=""

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

askQuestion "Do you want to terminate this program ?"
if [ $LASTANSWER ] 
then
    echo "yes"
else
    echo "nope"
fi