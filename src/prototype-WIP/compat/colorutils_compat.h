//
// Created by qm210 on 06.06.2025.
//

#ifndef DLTROPHY_SIMULATOR_COLORUTILS_COMPAT_H
#define DLTROPHY_SIMULATOR_COLORUTILS_COMPAT_H

#include "pixeltypes.h"

/// RGB color palette with 16 discrete values
class CRGBPalette16 {
public:
    CRGB entries[16];  ///< @copydoc CHSVPalette16::entries

    /// @copydoc CRGB::CRGB()
    CRGBPalette16() {};

    /// Create palette from 16 CRGB values
    CRGBPalette16( const CRGB& c00,const CRGB& c01,const CRGB& c02,const CRGB& c03,
                   const CRGB& c04,const CRGB& c05,const CRGB& c06,const CRGB& c07,
                   const CRGB& c08,const CRGB& c09,const CRGB& c10,const CRGB& c11,
                   const CRGB& c12,const CRGB& c13,const CRGB& c14,const CRGB& c15 )
    {
        entries[0]=c00; entries[1]=c01; entries[2]=c02; entries[3]=c03;
        entries[4]=c04; entries[5]=c05; entries[6]=c06; entries[7]=c07;
        entries[8]=c08; entries[9]=c09; entries[10]=c10; entries[11]=c11;
        entries[12]=c12; entries[13]=c13; entries[14]=c14; entries[15]=c15;
    };

    /// Copy constructor
    CRGBPalette16( const CRGBPalette16& rhs)
    {
        memmove8( (void *) &(entries[0]), &(rhs.entries[0]), sizeof( entries));
    }
    /// Create palette from array of CRGB colors
    CRGBPalette16( const CRGB rhs[16])
    {
        memmove8( (void *) &(entries[0]), &(rhs[0]), sizeof( entries));
    }
    /// @copydoc CRGBPalette16(const CRGBPalette16&)
    CRGBPalette16& operator=( const CRGBPalette16& rhs)
    {
        memmove8( (void *) &(entries[0]), &(rhs.entries[0]), sizeof( entries));
        return *this;
    }
    /// Create palette from array of CRGB colors
    CRGBPalette16& operator=( const CRGB rhs[16])
    {
        memmove8( (void *) &(entries[0]), &(rhs[0]), sizeof( entries));
        return *this;
    }

    /// Create palette from CHSV palette
    CRGBPalette16( const CHSVPalette16& rhs)
    {
        for( uint8_t i = 0; i < 16; ++i) {
            entries[i] = rhs.entries[i]; // implicit HSV-to-RGB conversion
        }
    }
    /// Create palette from array of CHSV colors
    CRGBPalette16( const CHSV rhs[16])
    {
        for( uint8_t i = 0; i < 16; ++i) {
            entries[i] = rhs[i]; // implicit HSV-to-RGB conversion
        }
    }
    /// @copydoc CRGBPalette16(const CHSVPalette16&)
    CRGBPalette16& operator=( const CHSVPalette16& rhs)
    {
        for( uint8_t i = 0; i < 16; ++i) {
            entries[i] = rhs.entries[i]; // implicit HSV-to-RGB conversion
        }
        return *this;
    }
    /// Create palette from array of CHSV colors
    CRGBPalette16& operator=( const CHSV rhs[16])
    {
        for( uint8_t i = 0; i < 16; ++i) {
            entries[i] = rhs[i]; // implicit HSV-to-RGB conversion
        }
        return *this;
    }

    /// Create palette from palette stored in PROGMEM
    CRGBPalette16( const TProgmemRGBPalette16& rhs)
    {
        for( uint8_t i = 0; i < 16; ++i) {
            entries[i] =  FL_PGM_READ_DWORD_NEAR( rhs + i);
        }
    }
    /// @copydoc CRGBPalette16(const TProgmemRGBPalette16&)
    CRGBPalette16& operator=( const TProgmemRGBPalette16& rhs)
    {
        for( uint8_t i = 0; i < 16; ++i) {
            entries[i] =  FL_PGM_READ_DWORD_NEAR( rhs + i);
        }
        return *this;
    }

    /// @copydoc CHSVPalette16::operator==
    bool operator==( const CRGBPalette16 &rhs) const
    {
        const uint8_t* p = (const uint8_t*)(&(this->entries[0]));
        const uint8_t* q = (const uint8_t*)(&(rhs.entries[0]));
        if( p == q) return true;
        for( uint8_t i = 0; i < (sizeof( entries)); ++i) {
            if( *p != *q) return false;
            ++p;
            ++q;
        }
        return true;
    }
    /// @copydoc CHSVPalette16::operator!=
    bool operator!=( const CRGBPalette16 &rhs) const
    {
        return !( *this == rhs);
    }
    /// @copydoc CHSVPalette16::operator[]
    inline CRGB& operator[] (uint8_t x) __attribute__((always_inline))
    {
        return entries[x];
    }
    /// @copydoc CHSVPalette16::operator[]
    inline const CRGB& operator[] (uint8_t x) const __attribute__((always_inline))
    {
        return entries[x];
    }

    /// @copydoc CHSVPalette16::operator[]
    inline CRGB& operator[] (int x) __attribute__((always_inline))
    {
        return entries[(uint8_t)x];
    }
    /// @copydoc CHSVPalette16::operator[]
    inline const CRGB& operator[] (int x) const __attribute__((always_inline))
    {
        return entries[(uint8_t)x];
    }

    /// Get the underlying pointer to the CHSV entries making up the palette
    operator CRGB*()
    {
        return &(entries[0]);
    }

    /// @copydoc CHSVPalette16::CHSVPalette16(const CHSV&)
    CRGBPalette16( const CHSV& c1)
    {
        fill_solid( &(entries[0]), 16, c1);
    }
    /// @copydoc CHSVPalette16::CHSVPalette16(const CHSV&, const CHSV&)
    CRGBPalette16( const CHSV& c1, const CHSV& c2)
    {
        fill_gradient( &(entries[0]), 16, c1, c2);
    }
    /// @copydoc CHSVPalette16::CHSVPalette16(const CHSV&, const CHSV&, const CHSV&)
    CRGBPalette16( const CHSV& c1, const CHSV& c2, const CHSV& c3)
    {
        fill_gradient( &(entries[0]), 16, c1, c2, c3);
    }
    /// @copydoc CHSVPalette16::CHSVPalette16(const CHSV&, const CHSV&, const CHSV&, const CHSV&)
    CRGBPalette16( const CHSV& c1, const CHSV& c2, const CHSV& c3, const CHSV& c4)
    {
        fill_gradient( &(entries[0]), 16, c1, c2, c3, c4);
    }

    /// @copydoc CHSVPalette16::CHSVPalette16(const CHSV&)
    CRGBPalette16( const CRGB& c1)
    {
        fill_solid( &(entries[0]), 16, c1);
    }
    /// @copydoc CHSVPalette16::CHSVPalette16(const CHSV&, const CHSV&)
    CRGBPalette16( const CRGB& c1, const CRGB& c2)
    {
        fill_gradient_RGB( &(entries[0]), 16, c1, c2);
    }
    /// @copydoc CHSVPalette16::CHSVPalette16(const CHSV&, const CHSV&, const CHSV&)
    CRGBPalette16( const CRGB& c1, const CRGB& c2, const CRGB& c3)
    {
        fill_gradient_RGB( &(entries[0]), 16, c1, c2, c3);
    }
    /// @copydoc CHSVPalette16::CHSVPalette16(const CHSV&, const CHSV&, const CHSV&, const CHSV&)
    CRGBPalette16( const CRGB& c1, const CRGB& c2, const CRGB& c3, const CRGB& c4)
    {
        fill_gradient_RGB( &(entries[0]), 16, c1, c2, c3, c4);
    }

    /// Creates a palette from a gradient palette in PROGMEM.
    ///
    /// Gradient palettes are loaded into CRGBPalettes in such a way
    /// that, if possible, every color represented in the gradient palette
    /// is also represented in the CRGBPalette.
    ///
    /// For example, consider a gradient palette that is all black except
    /// for a single, one-element-wide (1/256th!) spike of red in the middle:
    ///   @code
    ///     0,   0,0,0
    ///   124,   0,0,0
    ///   125, 255,0,0  // one 1/256th-palette-wide red stripe
    ///   126,   0,0,0
    ///   255,   0,0,0
    ///   @endcode
    /// A naive conversion of this 256-element palette to a 16-element palette
    /// might accidentally completely eliminate the red spike, rendering the
    /// palette completely black.
    ///
    /// However, the conversions provided here would attempt to include a
    /// the red stripe in the output, more-or-less as faithfully as possible.
    /// So in this case, the resulting CRGBPalette16 palette would have a red
    /// stripe in the middle which was 1/16th of a palette wide -- the
    /// narrowest possible in a CRGBPalette16.
    ///
    /// This means that the relative width of stripes in a CRGBPalette16
    /// will be, by definition, different from the widths in the gradient
    /// palette.  This code attempts to preserve "all the colors", rather than
    /// the exact stripe widths at the expense of dropping some colors.
    CRGBPalette16( TProgmemRGBGradientPalette_bytes progpal )
    {
        *this = progpal;
    }
    /// @copydoc CRGBPalette16(TProgmemRGBGradientPalette_bytes)
    CRGBPalette16& operator=( TProgmemRGBGradientPalette_bytes progpal )
    {
        TRGBGradientPaletteEntryUnion* progent = (TRGBGradientPaletteEntryUnion*)(progpal);
        TRGBGradientPaletteEntryUnion u;

        // Count entries
        uint16_t count = 0;
        do {
            u.dword = FL_PGM_READ_DWORD_NEAR(progent + count);
            ++count;
        } while ( u.index != 255);

        int8_t lastSlotUsed = -1;

        u.dword = FL_PGM_READ_DWORD_NEAR( progent);
        CRGB rgbstart( u.r, u.g, u.b);

        int indexstart = 0;
        uint8_t istart8 = 0;
        uint8_t iend8 = 0;
        while( indexstart < 255) {
            ++progent;
            u.dword = FL_PGM_READ_DWORD_NEAR( progent);
            int indexend  = u.index;
            CRGB rgbend( u.r, u.g, u.b);
            istart8 = indexstart / 16;
            iend8   = indexend   / 16;
            if( count < 16) {
                if( (istart8 <= lastSlotUsed) && (lastSlotUsed < 15)) {
                    istart8 = lastSlotUsed + 1;
                    if( iend8 < istart8) {
                        iend8 = istart8;
                    }
                }
                lastSlotUsed = iend8;
            }
            fill_gradient_RGB( &(entries[0]), istart8, rgbstart, iend8, rgbend);
            indexstart = indexend;
            rgbstart = rgbend;
        }
        return *this;
    }
    /// Creates a palette from a gradient palette in dynamic (heap) memory.
    /// @copydetails CRGBPalette16::CRGBPalette16(TProgmemRGBGradientPalette_bytes)
    CRGBPalette16& loadDynamicGradientPalette( TDynamicRGBGradientPalette_bytes gpal )
    {
        TRGBGradientPaletteEntryUnion* ent = (TRGBGradientPaletteEntryUnion*)(gpal);
        TRGBGradientPaletteEntryUnion u;

        // Count entries
        uint16_t count = 0;
        do {
            u = *(ent + count);
            ++count;
        } while ( u.index != 255);

        int8_t lastSlotUsed = -1;


        u = *ent;
        CRGB rgbstart( u.r, u.g, u.b);

        int indexstart = 0;
        uint8_t istart8 = 0;
        uint8_t iend8 = 0;
        while( indexstart < 255) {
            ++ent;
            u = *ent;
            int indexend  = u.index;
            CRGB rgbend( u.r, u.g, u.b);
            istart8 = indexstart / 16;
            iend8   = indexend   / 16;
            if( count < 16) {
                if( (istart8 <= lastSlotUsed) && (lastSlotUsed < 15)) {
                    istart8 = lastSlotUsed + 1;
                    if( iend8 < istart8) {
                        iend8 = istart8;
                    }
                }
                lastSlotUsed = iend8;
            }
            fill_gradient_RGB( &(entries[0]), istart8, rgbstart, iend8, rgbend);
            indexstart = indexend;
            rgbstart = rgbend;
        }
        return *this;
    }

};

#endif //DLTROPHY_SIMULATOR_COLORUTILS_COMPAT_H
