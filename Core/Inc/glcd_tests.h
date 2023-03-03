#ifndef __GLCD_TEST_H__
#define __GLCD_TEST_H__

#include <_ansi.h>

_BEGIN_STD_C

void glcd_TestBorder(void);
void glcd_TestFonts(void);
void glcd_TestFPS(void);
void glcd_TestAll(void);
void glcd_TestLine(void);
void glcd_TestRectangle(void);
void glcd_TestRectangleFill();
void glcd_TestCircle(void);
void glcd_TestArc(void);
void glcd_TestPolyline(void);
void glcd_TestDrawBitmap(void);

_END_STD_C

#endif // __GLCD_TEST_H__
