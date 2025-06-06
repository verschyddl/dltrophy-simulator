//
// Created by qm210 on 06.06.2025.
//

#ifndef DLTROPHY_SIMULATOR_BUS_COMPAT_H
#define DLTROPHY_SIMULATOR_BUS_COMPAT_H

#include "compat.h"
#include "const.h"

class Bus {
public:
    Bus(uint8_t type, uint16_t start, uint8_t aw, uint16_t len = 1, bool reversed = false, bool refresh = false)
            : _type(type)
            , _bri(255)
            , _start(start)
            , _len(len)
            , _reversed(reversed)
            , _valid(false)
            , _needsRefresh(refresh)
            , _data(nullptr) // keep data access consistent across all types of buses
    {
        _autoWhiteMode = Bus::hasWhite(type) ? aw : RGBW_MODE_MANUAL_ONLY;
    };

    virtual ~Bus() {} //throw the bus under the bus

    virtual void     show() = 0;
    virtual bool     canShow()                   { return true; }
    virtual void     setStatusPixel(uint32_t c)  {}
    virtual void     setPixelColor(uint16_t pix, uint32_t c) = 0;
    virtual uint32_t getPixelColor(uint16_t pix) { return 0; }
    virtual void     setBrightness(uint8_t b)    { _bri = b; };
    virtual void     cleanup() = 0;
    virtual uint8_t  getPins(uint8_t* pinArray)  { return 0; }
    virtual uint16_t getLength()                 { return _len; }
    virtual void     setColorOrder()             {}
    virtual uint8_t  getColorOrder()             { return COL_ORDER_RGB; }
    virtual uint8_t  skippedLeds()               { return 0; }
    virtual uint16_t getFrequency()              { return 0U; }
    inline  void     setReversed(bool reversed)  { _reversed = reversed; }
    inline  uint16_t getStart()                  { return _start; }
    inline  void     setStart(uint16_t start)    { _start = start; }
    inline  uint16_t getEnd()                    { return _start + _len; }
    inline  uint8_t  getType()                   { return _type; }
    inline  bool     isOk()                      { return _valid; }
    inline  bool     isReversed()                { return _reversed; }
    inline  bool     isOffRefreshRequired()      { return _needsRefresh; }
    bool     containsPixel(uint16_t pix) { return pix >= _start && pix < _start+_len; }

    virtual bool hasRGB(void) { return Bus::hasRGB(_type); }
    static  bool hasRGB(uint8_t type) {
        if ((type >= TYPE_WS2812_1CH && type <= TYPE_WS2812_WWA) || type == TYPE_ANALOG_1CH || type == TYPE_ANALOG_2CH || type == TYPE_ONOFF) return false;
        return true;
    }
    virtual bool hasWhite(void) { return Bus::hasWhite(_type); }
    static  bool hasWhite(uint8_t type) {
        if ((type >= TYPE_WS2812_1CH && type <= TYPE_WS2812_WWA) || type == TYPE_SK6812_RGBW || type == TYPE_TM1814 || type == TYPE_UCS8904) return true; // digital types with white channel
        if (type > TYPE_ONOFF && type <= TYPE_ANALOG_5CH && type != TYPE_ANALOG_3CH) return true; // analog types with white channel
        if (type == TYPE_NET_DDP_RGBW) return true; // network types with white channel
        return false;
    }
    virtual bool hasCCT(void) { return Bus::hasCCT(_type); }
    static  bool hasCCT(uint8_t type) {
        if (type == TYPE_WS2812_2CH_X3 || type == TYPE_WS2812_WWA ||
            type == TYPE_ANALOG_2CH    || type == TYPE_ANALOG_5CH) return true;
        return false;
    }
    static void setCCT(uint16_t cct) {
        _cct = cct;
    }
    static void setCCTBlend(uint8_t b) {
        if (b > 100) b = 100;
        _cctBlend = (b * 127) / 100;
        //compile-time limiter for hardware that can't power both white channels at max
#ifdef WLED_MAX_CCT_BLEND
        if (_cctBlend > WLED_MAX_CCT_BLEND) _cctBlend = WLED_MAX_CCT_BLEND;
#endif
    }
    inline        void    setAutoWhiteMode(uint8_t m) { if (m < 5) _autoWhiteMode = m; }
    inline        uint8_t getAutoWhiteMode()          { return _autoWhiteMode; }
    inline static void    setGlobalAWMode(uint8_t m)  { if (m < 5) _gAWM = m; else _gAWM = AW_GLOBAL_DISABLED; }
    inline static uint8_t getGlobalAWMode()           { return _gAWM; }

protected:
    uint8_t  _type;
    uint8_t  _bri;
    uint16_t _start;
    uint16_t _len;
    bool     _reversed;
    bool     _valid;
    bool     _needsRefresh;
    uint8_t  _autoWhiteMode;
    uint8_t  *_data;
    static uint8_t _gAWM;
    static int16_t _cct;
    static uint8_t _cctBlend;

    uint32_t autoWhiteCalc(uint32_t c);
    uint8_t *allocData(size_t size = 1);
    void     freeData() { if (_data != nullptr) free(_data); _data = nullptr; }
};

#endif //DLTROPHY_SIMULATOR_BUS_COMPAT_H
