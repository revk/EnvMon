// 3D case for Environmental monitor
// Copyright (c) 2019 Adrian Kennard, Andrews & Arnold Limited, see LICENSE file (GPL)

// Main component cube, may just be the PCB itself
// Bits stick out, usually on top or bottom

nooled=0;   // Set for design where OLED is not in use.

compw=45;
comph=37;
compt=1.6+(nooled?0:9+1.6);
compclear=0.5;

// Box thickness reference to component cube
base=(nooled?8:2.6);
top=4.6;
side=2.5;
sidet=0.1; // Gap in side clips

$fn=48;

// Note, origin is corner of box in all cases
// I.e. comp is offset side, side, base

// Parts
module esp32()
{ // ESP32-WROOM-32
    cube([25.500,18,1]);
    translate([1,1,0])
    cube([18,16,3]);
    translate([1,-1,0])
    cube([18,20,2]); // SOlder
}

module microusb()
{ // MicroUSB
    cube([8.150,5.080,4]);
    cube([8.150,5.080+1,0.5]);
}

module smd1206()
{ // 1206
    translate([-0.5,-0.5,0])
    cube([3.2+1,1.6+1,1]); // Larger to allow for position error
    translate([-1,-1,0])
    cube([2,1.6+1+1,0.9]); // Solder
    translate([3.2-1,-1,0])
    cube([2,1.6+1+1,0.9]); // Solder
}

module spox(n,hole=false)
{
    cube([(n-1)*2.5+4.9,4.9,4.9]);
    cube([(n-1)*2.5+4.9,5.9,3.9]);
    hull()
    {
        cube([(n-1)*2.5+4.9,7.9,0.5]);
        cube([(n-1)*2.5+4.9,7.4,1]);
    }
    translate([4.9/2-0.3,0,0])
    cube([(n-1)*2.5+0.6,6.6+0.3,2.38+0.3]);
    if(hole)
    translate([0,-side*2,0])
    cube([(n-1)*2.5+4.9,side*2,4.9]);
}

module pins(x,y,s,w,h)
{ // Solder pins, x/y are left/bottom edge, s is size
    translate([x+s/2,y+s/2,0])
    hull()
    {
        cylinder(d1=4,d2=s,h=3,$fn=8);
        translate([w,h,0])
        cylinder(d1=4,d2=s,h=3,$fn=8);
    }
}

// Draw component
// Exact dimensions as clerance added later
// hole: add holes for leads / viewing, etc
module comp(hole=false)
{
    translate([side,side,base])
    {
        // Basic cube
        cube([compw,comph,compt]);
        translate([-1,-1,0])
        { // Co-ordinates from PCB are 1mm border
            for(x=[3.75,43.25])
            for(y=[3.5,35.5])
            translate([x,y,0])
            {
                if(!nooled)
                    translate([0,0,1.6])
                cylinder(d=6,h=9);
                translate([0,0,-1])
                cylinder(d=5,h=compt+2);
            }
            translate([0,0,compt])
            { // PCB
                translate([26.070,10.348,0])
                esp32();
                // USB
                translate([11.800,1.000,0])
                microusb();
                pins(4.993,9.440,0.901,0,3*2.54);
                pins(7.818,2.820,0.901,0,3*2.54);
                pins(7.150,21.675,0.900,0,3*2.5);
                pins(32.000,7.150,0.900,2*2.5,0);
                pins(33.360,31.978,0.901,2*2.54,0);
                pins(33.360,34.837,0.901,2.54,0);
                translate([34.400,4.480,0])
                smd1206();
                if(hole)
                    translate([32.548,36.510,,0])
                    cube([5.232,1.440,top*2]);
                translate([30,1,-1.6])
                mirror([0,0,1])
                spox(3,hole);
                translate([1,19.715+12.4,-1.6])
                mirror([0,0,1])
                rotate([0,0,-90])
                spox(4,false); // Blocked off
                // CO2
                translate([7,2,-1.6-7])
                cube([23,35,7]);
                // Reg
                translate([30,21,-1.6-3])
                cube([10.160,12.700,3]);
            }
            // Connectors
            if(hole)
            {
                translate([11.800,-2,compt])
                cube([8.150,3,4]);
                translate([11.800-1,-side*2-2,compt-1])
                cube([10.3,side*2,6]);
                translate([19,-20,6.6])
                cube([3,25,1]); // Air hole
                for(y=[14,31])
                translate([16,y,compt])
                { // Screw
                    cylinder(d=7,h=1);
                    translate([0,0,1])
                    cylinder(d1=7,d2=3,h=2);
                    cylinder(d=3,h=top*2);
                }
            }
        }
    }
    if(!nooled)
    translate([side,side,base])
    { // OLED
        translate([5,0,-2])
        cube([35,37,2]);
        translate([2,7,-2])
        cube([3,14,2]);
        if(hole)
        hull()
        {
            translate([7.5,1.5,-2-1])
            cube([30,28,1]);
            translate([7.5-5,2-5,-2-1-20])
            cube([30+10,28+10,1]);
        }
    }
}

// Case
// Normally just a cube, but could have rounded corners.
// Typically solid, as comp is removed from it
module case()
{
    hull()
    {
        translate([side/2,side/2,0])
        cube([compw+side,comph+side,base+compt+top]);
        translate([0,0,side/2])
        cube([compw+side*2,comph+side*2,base+compt+top-side/2]);
    }
}

// Cut line
// Defines how case is cut, this is the solid area encompassing bottom half of case
module cut(a=false)
{
    difference()
    {
        translate([-10,-10,-10])
        cube([compw+side*2+20,comph+side*2+20,base+compt/2+side/4+10]);
        translate([side/2-(a?sidet:-sidet),side/2-(a?sidet:-sidet),base+compt/2+side/4-side/2-(a?sidet:-sidet)])
        cube([compw+side+2*(a?sidet:-sidet),comph+side+2*(a?sidet:-sidet),side*2]);
        translate([side-1+30,side-1+1-15,base+compt-1.6-1])
        hull()
        { // DS18B20
            cube([9.9,30,1]);
            translate([-2,0,10])
            cube([9.9+4,30,1]);
        }
    }
    translate([side-1+26.070,side-1+10.348,base+compt-10])
    hull()
    { // ESP32
        cube([30,18,10]);
        translate([0,-5,0])
        cube([30,18+10,1]);
    }
    translate([side-1+11.800,side-1+1.000-20,base+compt-7])
    hull()
    { // USB
        cube([8.15,30,7.001]);
        translate([-5,0,0])
        cube([8.15+10,30,1]);
    }
    translate([side-1+30,side-1+1-15,base+compt-1.6-10-4.9])
    hull()
    { // DS18B20
        cube([9.9,30,10]);
        translate([-2,0,0])
        cube([9.9+4,30,1]);
    }
}

module casecomp()
{
    difference()
    {
        case();
        minkowski()
        {
            comp(true);
            cube(compclear,center=true);
        }
    }
}

module a()
{
    intersection()
    {
        casecomp();
        cut(true);
    }
}

module b()
{
    translate([(compw+side*2)/2,(comph+side*2)/2,(base+compt+top)/2])
    rotate([180,0,0])
    translate([-(compw+side*2)/2,-(comph+side*2)/2,-(base+compt+top)/2])
    difference()
    {
        casecomp();
        cut(false);
    }
}

a();
translate([compw+side*2+5,0,0])
b();