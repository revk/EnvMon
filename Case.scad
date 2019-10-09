// 3D case for encironmental monitor
// Copyright (c) 2019 Adrian Kennard, Andrews & Arnold Limited, see LICENSE file (GPL)

use <PCBCase/case.scad>

display=true; // has display

width=45;
height=37;

// Box thickness reference to component cube
base=(display?13:8);
top=5;

$fn=48;

module pcb(s=0)
{
    translate([-1,-1,0])
    { // 1mm ref edge of PCB vs SVG design
        esp32(s,26.170,10.448,-90);        
        if(display)
        {
            oled(s,1,1,d=6,h=9,nopads=true);
            pads(5.443,9.89,ny=4);
        }
        d24v5f3(s,30,21,180);
        spox(s,1,19.715,-90,4,hidden=true);
        usbc(s,13.065,0.510,0);
        co2(s,7,2);
        smd1206(s,13.735,9.010);
        smd1206(s,18.135,9.010);
    }
}

case(width,height,base,top){pcb(0);pcb(-1);pcb(1);};

