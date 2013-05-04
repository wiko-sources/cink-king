#ifndef FTM_CUST_FONT_H
#define FTM_CUST_FONT_H

#include "cust.h"


#if defined(FEATURE_FTM_FONT_16x30)
#define CHAR_WIDTH      16
#define CHAR_HEIGHT     30
#elif defined(FEATURE_FTM_FONT_16x28)
#define CHAR_WIDTH      16
#define CHAR_HEIGHT     28
#elif defined(FEATURE_FTM_FONT_12x22)
#define CHAR_WIDTH      12
#define CHAR_HEIGHT     22
#elif defined(FEATURE_FTM_FONT_10x18)
#define CHAR_WIDTH      10
#define CHAR_HEIGHT     18
#elif defined(FEATURE_FTM_FONT_8x14)
#define CHAR_WIDTH      8
#define CHAR_HEIGHT     14
#elif defined(FEATURE_FTM_FONT_6x10)
#define CHAR_WIDTH      6
#define CHAR_HEIGHT     10

#elif defined(FEATURE_FTM_FONT_32x32)
#define CHAR_WIDTH      32
#define CHAR_HEIGHT     32
#elif defined(FEATURE_FTM_FONT_28x28)
#define CHAR_WIDTH      28
#define CHAR_HEIGHT     28
#elif defined(FEATURE_FTM_FONT_26x26)
#define CHAR_WIDTH      26
#define CHAR_HEIGHT     26
#elif defined(FEATURE_FTM_FONT_24x24)
#define CHAR_WIDTH      24
#define CHAR_HEIGHT     24
#elif defined(FEATURE_FTM_FONT_20x20)
#define CHAR_WIDTH      20
#define CHAR_HEIGHT     20
#elif defined(FEATURE_FTM_FONT_18x18)
#define CHAR_WIDTH      18
#define CHAR_HEIGHT     18
#elif defined(FEATURE_FTM_FONT_16x16)
#define CHAR_WIDTH      16
#define CHAR_HEIGHT     16
#elif defined(FEATURE_FTM_FONT_12x12)
#define CHAR_WIDTH      12
#define CHAR_HEIGHT     12

#else
//#error "font size is not definied"
//Comment it or /alps/mediatek/source/external/mhal/src/core/scenario/6575/factory/multimediaFactoryTest.cpp
//will use cust.h, that will error because FEATURE_FTM_FONT_XxX was defined in makefile rather than in cust.h .
#if defined(SUPPORT_GB2312)
#define CHAR_WIDTH      26
#define CHAR_HEIGHT     26
#else
#define CHAR_WIDTH      12
#define CHAR_HEIGHT     22
#endif


#endif

#endif /* FTM_CUST_H */
