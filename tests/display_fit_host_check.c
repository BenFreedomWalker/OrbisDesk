/* SPDX-License-Identifier: LGPL-2.1-or-later */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include "pw_display_fit.h"

static int failures;
static void expect(const char *name,uint32_t ow,uint32_t oh,uint32_t dw,uint32_t dh,float ratio,int stretch,
                   uint32_t sl,uint32_t st,uint32_t sw,uint32_t sh,
                   uint32_t l,uint32_t t,uint32_t w,uint32_t h)
{
    struct pw_display_fit f;
    int ok=pw_display_fit_calculate(ow,oh,dw,dh,ratio,stretch,&f) &&
        f.safe_left==sl && f.safe_top==st && f.safe_width==sw && f.safe_height==sh &&
        f.left==l && f.top==t && f.width==w && f.height==h &&
        f.left>=f.safe_left && f.top>=f.safe_top &&
        (uint64_t)f.left+f.width<=(uint64_t)f.safe_left+f.safe_width &&
        (uint64_t)f.top+f.height<=(uint64_t)f.safe_top+f.safe_height;
    printf("%-32s %s\n",name,ok?"PASS":"FAIL");
    if(!ok) ++failures;
}
int main(void)
{
    expect("16:9 full-frame",1280,720,1920,1080,1.0f,0,0,0,1280,720,0,0,1280,720);
    expect("16:9 safe-area",1920,1080,1360,765,0.9f,0,96,54,1728,972,96,54,1728,972);
    expect("1360x768 safe-area",1360,768,1920,1080,0.9f,0,68,38,1224,691,68,39,1224,688);
    expect("4:3 centered",1360,768,1024,768,0.8f,0,136,77,1088,614,271,77,818,614);
    expect("odd output dimensions",1365,767,1920,1080,1.0f,0,0,0,1365,767,1,0,1363,767);
    expect("odd safe-area rounding",321,201,16,9,0.5f,1,80,50,161,101,80,50,161,101);
    expect("stretch fills safe area",1360,768,1920,1080,0.8f,1,136,77,1088,614,136,77,1088,614);
    expect("invalid ratio uses full frame",1360,768,1920,1080,0.1f,0,0,0,1360,768,0,1,1360,765);
    expect("NaN ratio uses full frame",1360,768,1920,1080,NAN,0,0,0,1360,768,0,1,1360,765);
    struct pw_display_fit f;
    if(pw_display_fit_calculate(0,768,1920,1080,1.0f,0,&f) ||
       pw_display_fit_calculate(1360,768,0,1080,1.0f,0,&f) ||
       pw_display_fit_calculate(1360,768,1920,1080,1.0f,0,NULL)) {
        puts("invalid dimensions/output FAIL");++failures;
    } else puts("invalid dimensions/output PASS");
    puts(failures?"FAIL":"PASS");
    return failures?1:0;
}
