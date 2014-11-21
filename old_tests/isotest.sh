#!/bin/bash
for ISO in 400 500 640 800
  do
   for SHUTTER in 1000
   do
    for AWB in off auto sun cloud shade tungsten fluorescent incandescent flash horizon
    do
     echo Testing ISO:$ISO shutter:$SHUTTER AWB:$AWB
     raspistill -ISO $ISO -awb $AWB -vf -o isotest-AWB$AWB-ISO$ISO-shutter$SHUTTER.jpeg
     done
   done
  done

