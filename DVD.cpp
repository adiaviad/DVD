#include <stdio.h>
#include <chrono>
#include <thread>
#include <math.h>

typedef struct RGBColor {
    int r;
    int g;
    int b;
} RGBColor;


void hsv2rgb(float H, float S, float V, RGBColor* color) {
    float r, g, b;

    float h = H / 360;
    float s = S / 100;
    float v = V / 100;

    int i = floor(h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    switch (i % 6) {
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
    }

    (*color).r = r * 255;
    (*color).g = g * 255;
    (*color).b = b * 255;
}
const char* esc_save = "\u001b[s";
const char* esc_return = "\u001b[u";
const char* esc_erase_down = "\u001b[J";
/*
*   Cursor Down		<ESC>[{COUNT}B
*   Moves the cursor down by COUNT rows; the default count is 1.
*
*   Cursor Forward		<ESC>[{COUNT}C
*   Moves the cursor forward by COUNT columns; the default count is 1.
*/
void set_cursor_delta(int down, int right) {
    printf(esc_return);
    printf(esc_erase_down);
    printf("\u001b[%dB", down);
    printf("\u001b[%dC", right);
}
void set_text_color(int r, int g, int b) {
    printf("\u001b[38;2;%d;%d;%dm", r, g, b);
}
int main()
{

    int waitTimeMS = 50;
    float tSec = 0;
    float waitTimeSec = waitTimeMS / 1000.0;

    RGBColor color = { 0,0,0 };
    float hue = 0;
    float velHue = 0.5; // hue/tick

    float velFrequancyH = 1 / 100, velFrequancyV = 1/ 200;
    float velVertical =1* waitTimeSec, velHorizontal = 5* waitTimeSec; // tile/sec

    int useSineVel = 1;
    float velVmin = 0.33* waitTimeSec, velHmin = 2* waitTimeSec;
    float velVmax = 3* waitTimeSec, velHmax = 8* waitTimeSec;

    float velVavg = (velVmax + velVmin) / 2, velHavg = (velHmax + velHmin) / 2;
    float velHampl = velHavg - velHmin, velVampl = velVavg - velVmin;

    int dirH = 1, dirV = 1;
    float posHorizontal = 40, posVertical = 0; //starts pos and counters
    int boundVertical = 30, boundHorizontal =116; 

    printf(esc_save);
    while(1)
    {
        if (useSineVel==1) {
            tSec += waitTimeSec;
            //update velocities based on sine
            velHorizontal = velHavg + velHampl*sin(2 * 3.14*velFrequancyH * tSec);
            velVertical = velVavg +velVampl*sin(2 * 3.14*velFrequancyV * tSec);
        }
        //update position based on velocity and direction 
        posHorizontal += velHorizontal*dirH;
        posVertical += velVertical*dirV;
        //print at position
        set_cursor_delta((int)posVertical,(int)posHorizontal);
        printf("DVD");
        std::this_thread::sleep_for(std::chrono::milliseconds(waitTimeMS));
        //check hitting a wall
        if (posHorizontal >= boundHorizontal) {
            posHorizontal = boundHorizontal;
            dirH = -dirH;
        }else if (posHorizontal <= 0) {
            posHorizontal = 0;
            dirH = -dirH;

        }
        if (posVertical>= boundVertical) {
            posVertical = boundVertical;
            dirV= -dirV;
        }
        else if (posVertical <= 0) {
            posVertical = 0;
            dirV = -dirV;
        }
        //update color 
        hue += velHue;
        if (hue >= 360) { hue = 0; }
        hsv2rgb(hue, 80, 80, &color);
        set_text_color(color.r, color.g, color.b);

    }
    
}

