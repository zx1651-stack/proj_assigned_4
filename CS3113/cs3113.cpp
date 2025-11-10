/**
* Author: Zirui Xu
* Assignment: Rise of the AI
* Date due: 2025-11-8, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#include "cs3113.h"

Color ColorFromHex(const char *hex)
{
    if (hex[0] == '#') hex++;

    unsigned int r = 0, g = 0, b = 0, a = 255;

    if (sscanf(hex, "%02x%02x%02x", &r, &g, &b) == 3)
    {
        return (Color){ 
            (unsigned char) r, 
            (unsigned char) g, 
            (unsigned char) b, 
            (unsigned char) a 
        };
    } 
    else if (sscanf(hex, "%02x%02x%02x%02x", &r, &g, &b, &a) == 4) 
    {
        return (Color){
            (unsigned char) r,
            (unsigned char) g,
            (unsigned char) b,
            (unsigned char) a
        };
    } 
    else { 
        return RAYWHITE;
    }
}
