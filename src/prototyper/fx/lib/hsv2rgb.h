#ifndef __INC_HSV2RGB_H
#define __INC_HSV2RGB_H

#include "pixeltypes.h"

void hsv2rgb_rainbow( const struct CHSV& hsv, struct CRGB& rgb);
void hsv2rgb_rainbow( const struct CHSV* phsv, struct CRGB * prgb, int numLeds);
#define HUE_MAX_RAINBOW 255

void hsv2rgb_spectrum( const struct CHSV& hsv, struct CRGB& rgb);
void hsv2rgb_spectrum( const struct CHSV* phsv, struct CRGB * prgb, int numLeds);
#define HUE_MAX_SPECTRUM 255

void hsv2rgb_raw(const struct CHSV& hsv, struct CRGB & rgb);
void hsv2rgb_raw(const struct CHSV* phsv, struct CRGB * prgb, int numLeds);
#define HUE_MAX 191

CHSV rgb2hsv_approximate( const CRGB& rgb);

#endif
