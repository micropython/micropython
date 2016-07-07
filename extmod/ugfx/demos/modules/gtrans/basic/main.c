/*
 * Copyright (c) 2012, 2013, Joel Bodenmann aka Tectu <joel@unormal.org>
 * Copyright (c) 2012, 2013, Andrew Hannam aka inmarket
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the <organization> nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include "gfx.h"

#define COLOR_BACKGROUND    Silver
#define COLOR_TEXT          Black

font_t font;

// English Translation
static const char* EnglishStrings[] = {
    "Welcome",
    "The temperature is %d degrees",
    "Goodbye",
    "This is a translated uGFX application"
};
static const transTable EnglishTranslation = { sizeof(EnglishStrings)/sizeof(EnglishStrings[0]), EnglishStrings };

// German translation
static const char* GermanStrings[] = {
    "Herzlich Willkommen",
    "Die Temperatur beträgt %d Grad",
    "Auf Wiedersehen",
    "Das ist eine übersetzte uGFX Anwendung"
};
static const transTable GermanTranslation = { sizeof(GermanStrings)/sizeof(GermanStrings[0]), GermanStrings };

// French translation
static const char* FrenchStrings[] = {
    "Bienvenue",
    "La température est de %d degrés",
    "Au revoir",
    "Ceci est une application traduit uGFX"
};
static const transTable FrenchTranslation = { sizeof(FrenchStrings)/sizeof(FrenchStrings[0]), FrenchStrings };

void updateText()
{
    coord_t width = 400;
    coord_t height = 30;

    // Translate some basic strings
    gdispFillStringBox(20,  20, width, height, gt("Welcome"), font, COLOR_TEXT, COLOR_BACKGROUND, justifyLeft);
    gdispFillStringBox(20,  60, width, height, gt("This is a translated uGFX application"), font, COLOR_TEXT, COLOR_BACKGROUND, justifyLeft);
    gdispFillStringBox(20, 100, width, height, gt("Goodbye"), font, COLOR_TEXT, COLOR_BACKGROUND, justifyLeft);

    // A more complex example using string formatting
    char buffer[128];
    sprintf(buffer, gt("The temperature is %d degrees"), 18);
    gdispFillStringBox(20, 140, width, height, buffer, font, COLOR_TEXT, COLOR_BACKGROUND, justifyLeft);
}

int main(void)
{
    // Initialize the uGFX library
    gfxInit();
    gdispClear(COLOR_BACKGROUND);

    // Take the first font we find
    font = gdispOpenFont("*");

    // Set the base language of the application
    gtransSetBaseLanguage(&EnglishTranslation);

    // Loop through the languages
    while (TRUE) {
        // English
        gtransSetLanguage(&EnglishTranslation);
        updateText();
        gfxSleepMilliseconds(1000);

        // German
        gtransSetLanguage(&GermanTranslation);
        updateText();
        gfxSleepMilliseconds(1000);

        // French
        gtransSetLanguage(&FrenchTranslation);
        updateText();
        gfxSleepMilliseconds(1000);
    }

    return 0;
}
