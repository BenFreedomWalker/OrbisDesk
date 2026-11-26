/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* Pure output geometry shared by the PS4 sink and its host-side checks. */
#ifndef PW_DISPLAY_FIT_H
#define PW_DISPLAY_FIT_H

#include <stdint.h>

struct pw_display_fit {
    uint32_t safe_left,safe_top,safe_width,safe_height;
    uint32_t left,top,width,height;
};

/* Returns 1 for usable dimensions. Invalid ratios fall back to the full output. */
static inline int pw_display_fit_calculate(uint32_t out_width,uint32_t out_height,
                                           uint32_t desktop_width,uint32_t desktop_height,
                                           float safe_ratio,int stretch,
                                           struct pw_display_fit *fit)
{
    if(!fit || !out_width || !out_height || !desktop_width || !desktop_height) return 0;
    if(!(safe_ratio>=0.5f && safe_ratio<=1.0f)) safe_ratio=1.0f;

    /* Round to the nearest output pixel instead of systematically shrinking the safe rectangle. */
    uint32_t safe_width=(uint32_t)((double)out_width*safe_ratio+0.5);
    uint32_t safe_height=(uint32_t)((double)out_height*safe_ratio+0.5);
    if(!safe_width) safe_width=out_width;
    if(!safe_height) safe_height=out_height;

    uint32_t width,height;
    /* Compare before integer division: floor() must not hide a fractional one-pixel overflow. */
    if((uint64_t)desktop_height*safe_width>(uint64_t)desktop_width*safe_height) {
        height=safe_height;
        width=(uint32_t)((uint64_t)desktop_width*safe_height/desktop_height);
    } else {
        width=safe_width;
        height=(uint32_t)((uint64_t)desktop_height*safe_width/desktop_width);
    }
    if(stretch) {width=safe_width;height=safe_height;}
    if(!width) width=1;
    if(!height) height=1;

    fit->safe_width=safe_width;fit->safe_height=safe_height;
    fit->safe_left=(out_width-safe_width)/2;fit->safe_top=(out_height-safe_height)/2;
    fit->width=width;fit->height=height;
    fit->left=fit->safe_left+(safe_width-width)/2;
    fit->top=fit->safe_top+(safe_height-height)/2;
    return 1;
}

#endif
