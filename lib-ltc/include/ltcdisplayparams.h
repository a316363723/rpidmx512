/**
 * @file ltcdisplayparams.h
 *
 */
/* Copyright (C) 2019-2020 by Arjan van Vught mailto:info@orangepi-dmx.nl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef LTCDISPLAYPARAMS_H_
#define LTCDISPLAYPARAMS_H_

#include <ltcdisplayrgb.h>

#include <stdint.h>

#include "ltcdisplaymax7219.h"
#include "ws28xx.h"
#include "rgbmapping.h"

struct TLtcDisplayParams {
	uint32_t nSetList;
	uint8_t nMax7219Type;
	uint8_t nMax7219Intensity;
	uint8_t nWS28xxLedType;
	uint8_t nGlobalBrightness;
	uint8_t nWS28xxRgbMapping;
	uint8_t nDisplayRgbIntensity;
	uint8_t nDisplayRgbColonBlinkMode;
	uint32_t aDisplayRgbColour[LTCDISPLAYWS28XX_COLOUR_INDEX_LAST];
	uint8_t nWS28xxDisplayType;
} __attribute__((packed));

struct LtcDisplayParamsMask {
	static constexpr auto MAX7219_TYPE = (1U << 0);
	static constexpr auto MAX7219_INTENSITY = (1U << 1);
	static constexpr auto WS28XX_LED_TYPE = (1U << 2);
	static constexpr auto GLOBAL_BRIGHTNESS = (1U << 3);
	static constexpr auto WS28XX_RGB_MAPPING = (1U << 4);
	static constexpr auto DISPLAYRGB_INTENSITY = (1U << 5);
	static constexpr auto DISPLAYRGB_COLON_BLINK_MODE = (1U << 6);
	static constexpr auto DISLAYRGB_COLOUR_INDEX = (1U << 7);
	static constexpr auto WS28XX_DISPLAY_TYPE = (1U << 8);
};

class LtcDisplayParamsStore {
public:
	virtual ~LtcDisplayParamsStore() {
	}

	virtual void Update(const struct TLtcDisplayParams *ptLtcDisplayParams)=0;
	virtual void Copy(struct TLtcDisplayParams *ptLtcDisplayParams)=0;
};

class LtcDisplayParams {
public:
	LtcDisplayParams(LtcDisplayParamsStore *pLtcDisplayParamsStore = nullptr);

	bool Load();
	void Load(const char *pBuffer, uint32_t nLength);

	void Builder(const struct TLtcDisplayParams *ptLtcDisplayParams, char *pBuffer, uint32_t nLength, uint32_t &nSize);
	void Save(char *pBuffer, uint32_t nLength, uint32_t &nSize);

	void Set(LtcDisplayRgb *pLtcDisplayWS28xx);

	void Dump();

	uint8_t GetGlobalBrightness() const {
		return m_tLtcDisplayParams.nGlobalBrightness;
	}

	TLtcDisplayMax7219Types GetMax7219Type() const {
		return static_cast<TLtcDisplayMax7219Types>(m_tLtcDisplayParams.nMax7219Type);
	}

	uint8_t GetMax7219Intensity() const {
		return m_tLtcDisplayParams.nMax7219Intensity;
	}

	TWS28XXType GetLedType() const {
		return static_cast<TWS28XXType>(m_tLtcDisplayParams.nWS28xxLedType);
	}

	LtcDisplayRgbWS28xxType GetWS28xxDisplayType() const {
		return static_cast<LtcDisplayRgbWS28xxType>(m_tLtcDisplayParams.nWS28xxDisplayType);
	}

    static void staticCallbackFunction(void *p, const char *s);

private:
    void callbackFunction(const char *pLine);
    bool isMaskSet(uint32_t nMask) const {
    	return (m_tLtcDisplayParams.nSetList & nMask) == nMask;
    }

private:
	LtcDisplayParamsStore *m_pLtcDisplayParamsStore;
	struct TLtcDisplayParams m_tLtcDisplayParams;
};

#endif /* LTCDISPLAYPARAMS_H_ */
