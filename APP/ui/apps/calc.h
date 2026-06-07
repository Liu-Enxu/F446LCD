#ifndef __CALC_H
#define __CALC_H

#include "app_base.h"
#include "lv_app_conf.h"
#include <stdint.h>
#include <ctype.h>

typedef struct calc_app_t calc_app_t;

typedef enum {
    CALC_STATE_INPUT = 0,      // normal input
    CALC_STATE_RESULT = 1,     // after = pressed, showing result
    CALC_STATE_ERROR = 2,      // division by zero, overflow etc
} calc_state_t;

#define CALC_EXPR_LEN 32
#define CALC_STACK_SIZE 16
struct calc_app_t{
	app_t app_base;
	char last_expr[CALC_EXPR_LEN];	
	char curr_expr[CALC_EXPR_LEN];	
	uint8_t  curr_len;
    calc_state_t state;

	lv_obj_t* lbl_last;
	lv_obj_t* lbl_curr;
};
calc_app_t* create_calc_app(void);



#endif /*__CALC_H*/

