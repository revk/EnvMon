// 3D case for Environmental monitor
// Copyright (c) 2019 Adrian Kennard, Andrews & Arnold Limited, see LICENSE file (GPL)

// Main component cube, may just be the PCB itself
// Bits stick out, usually on top or bottom

compw=45;
comph=37;
compt=1.6+9+1.6;

// Box thickness reference to component cube
base=5;
top=5;
side=2;

// Note, origin is corner of box in all cases
// I.e. comp is offset side, side, base

// Draw component
// Exact dimensions as clerance added later
// hole: add holes for leads / viewing, etc
module comp(hole=false)
{
    
}

// Case
// Normally just a cube, but could have rounded corners.
// Typically solid, as comp is removed from it
module case()
{
}

// Cut line
// Defines how case is cut, this is the solid area encompassing bottom half of case
module cut()
{
}

// Now to put the bits together
comp();