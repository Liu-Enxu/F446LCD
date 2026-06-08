#include "calc.h"

#define hor_res 480
#define ver_res 320
#define SCALE 100000

static const char * btnm_map[] = {"(", ")", "C", "CE", "\n",
								"7", "8", "9", "/", "\n",
								"4", "5", "6", "*", "\n",
								"1", "2", "3", "-", "\n",
								"0", ".", "=", "+", ""};
/* ================================================================== */
/*  Parser / evaluator                                                  */
/* ================================================================== */
 
static uint8_t precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}
 
/* a and b are both scaled by SCALE */
static int32_t apply_op(int32_t a, int32_t b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return (int32_t)(((int64_t)a * b) / SCALE);  // (a*100 * b*100) / 100 = result*100
        case '/':
            if (b == 0) return INT32_MAX;
            return (int32_t)(((int64_t)a * SCALE) / b);        // (a*100 * 100) / (b*100) = result*100
    }
    return 0;
}
 
static uint8_t is_operator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/');
}
 
/* parse a number from expr at position *i, return scaled value */
static int32_t parse_number(const char* expr, uint8_t* i) {
    int32_t integer  = 0;
    int32_t frac     = 0;
    int32_t frac_div = 1;
    uint8_t neg      = 0;
 
    if (expr[*i] == '-') { neg = 1; (*i)++; }
 
    while (isdigit((unsigned char)expr[*i])) {
        integer = integer * 10 + (expr[*i] - '0');
        (*i)++;
    }
    if (expr[*i] == '.') {
        (*i)++;
        while (isdigit((unsigned char)expr[*i]) && frac_div < SCALE) {
            frac     = frac * 10 + (expr[*i] - '0');
            frac_div = frac_div * 10;
            (*i)++;
        }
        /* skip any excess decimal digits */
        while (isdigit((unsigned char)expr[*i])) (*i)++;
    }
    /* normalize frac to SCALE decimal places */
    int32_t val = integer * SCALE + frac * (SCALE / frac_div);
    return neg ? -val : val;
}
 
/* Returns INT32_MAX on error (div by zero or malformed) */
static int32_t calc_evaluate(const char* expr) {
    int32_t operands[CALC_STACK_SIZE];
    char    operators[CALC_STACK_SIZE];
    int8_t  num_top = -1;
    int8_t  op_top  = -1;
    uint8_t i = 0;
 
    while (expr[i] != '\0') {
        if (expr[i] == ' ') { i++; continue; }
 
        /* number or leading minus */
        if (isdigit((unsigned char)expr[i]) ||
            (expr[i] == '-' && (i == 0 || expr[i-1] == '('))) {
            if (expr[i] == '-' && !isdigit((unsigned char)expr[i+1])) return INT32_MAX;
            if (num_top >= CALC_STACK_SIZE - 1) return INT32_MAX;
            operands[++num_top] = parse_number(expr, &i);
            continue;
        }
 
        /* open bracket */
        if (expr[i] == '(') {
            if (op_top >= CALC_STACK_SIZE - 1) return INT32_MAX;
            operators[++op_top] = '(';
            i++; continue;
        }
 
        /* close bracket */
        if (expr[i] == ')') {
            while (op_top >= 0 && operators[op_top] != '(') {
                if (num_top < 1) return INT32_MAX;
                int32_t b = operands[num_top--];
                int32_t a = operands[num_top--];
                int32_t r = apply_op(a, b, operators[op_top--]);
                if (r == INT32_MAX) return INT32_MAX;
                operands[++num_top] = r;
            }
            if (op_top < 0) return INT32_MAX;   /* mismatched bracket */
            op_top--;                            /* pop '(' */
            i++; continue;
        }
 
        /* operator */
        if (is_operator(expr[i])) {
            while (op_top >= 0 && operators[op_top] != '(' &&
                   precedence(operators[op_top]) >= precedence(expr[i])) {
                if (num_top < 1) return INT32_MAX;
                int32_t b = operands[num_top--];
                int32_t a = operands[num_top--];
                int32_t r = apply_op(a, b, operators[op_top--]);
                if (r == INT32_MAX) return INT32_MAX;
                operands[++num_top] = r;
            }
            if (op_top >= CALC_STACK_SIZE - 1) return INT32_MAX;
            operators[++op_top] = expr[i];
            i++; continue;
        }
 
        i++; /* unknown char, skip */
    }
 
    /* drain remaining operators */
    while (op_top >= 0) {
        if (num_top < 1) return INT32_MAX;
        int32_t b = operands[num_top--];
        int32_t a = operands[num_top--];
        int32_t r = apply_op(a, b, operators[op_top--]);
        if (r == INT32_MAX) return INT32_MAX;
        operands[++num_top] = r;
    }
 
    return (num_top == 0) ? operands[0] : INT32_MAX;
}
 
/* format scaled result into buf, strip trailing zeros */
static void format_result(int32_t val, char* buf, uint8_t len) {
    uint8_t neg = (val < 0);
    if (neg) val = -val;
    int32_t integer = val / SCALE;
    int32_t frac    = val % SCALE;
    if (frac == 0) {
        snprintf(buf, len, neg ? "-%ld" : "%ld", (long)integer);
    } else {
        /* strip trailing zero: 3.10 ?? 3.1 */
        if (frac % 10 == 0) {
            snprintf(buf, len, neg ? "-%ld.%01ld" : "%ld.%01ld",
                     (long)integer, (long)(frac / 10));
        } else {
            snprintf(buf, len, neg ? "-%ld.%02ld" : "%ld.%02ld",
                     (long)integer, (long)frac);
        }
    }
}
 
/* ================================================================== */
/*  Input validation helpers                                            */
/* ================================================================== */
 
static uint8_t brackets_balanced(const char* expr, uint8_t len) {
    int8_t depth = 0;
    for (uint8_t i = 0; i < len; i++) {
        if (expr[i] == '(') depth++;
        if (expr[i] == ')') depth--;
        if (depth < 0) return 0;
    }
    return (depth == 0);
}
 
static uint8_t can_append(calc_app_t* calc, const char* btn) {
    if (calc->curr_len == 0) {
        return (isdigit((unsigned char)btn[0]) || btn[0] == '(' || btn[0] == '-');
    }
    char last = calc->curr_expr[calc->curr_len - 1];
 
    /* dot: only one per number, only after digit */
    if (btn[0] == '.') {
        int8_t j = (int8_t)calc->curr_len - 1;
        while (j >= 0 && isdigit((unsigned char)calc->curr_expr[j])) j--;
        if (j >= 0 && calc->curr_expr[j] == '.') return 0;
        return isdigit((unsigned char)last);
    }
 
    /* operator */
    if (is_operator(btn[0])) {
        if (is_operator(last)) return 0;
        if (last == '.') return 0;              /* no operator after bare dot */
        if (last == '(') return (btn[0] == '-');
        return 1;
    }
 
    /* ')' */
    if (btn[0] == ')') {
        if (last == '(' || is_operator(last) || last == '.') return 0;
        uint8_t open = 0, close = 0;
        for (uint8_t i = 0; i < calc->curr_len; i++) {
            if (calc->curr_expr[i] == '(') open++;
            if (calc->curr_expr[i] == ')') close++;
        }
        return (open > close);
    }
 
    /* '(' ?? not after digit or ')' to keep things simple */
    if (btn[0] == '(') {
        return (!isdigit((unsigned char)last) && last != ')' && last != '.');
    }
 
    return 1;
}
 
/* ================================================================== */
/*  UI update                                                           */
/* ================================================================== */
 
static void update_display(calc_app_t* calc) {
    lv_label_set_text(calc->lbl_last, calc->last_expr);
    lv_label_set_text(calc->lbl_curr,
        calc->curr_len > 0 ? calc->curr_expr : "0");
}
 
/* ================================================================== */
/*  Button callback                                                     */
/* ================================================================== */
 
static void calc_btnm_cb(lv_event_t* e) {
    if (lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED) return;
    lv_obj_t*    obj  = lv_event_get_target(e);
    calc_app_t*  calc = (calc_app_t*)lv_event_get_user_data(e);
    uint16_t     btn  = lv_btnmatrix_get_selected_btn(obj);
    const char*  txt  = lv_btnmatrix_get_btn_text(obj, btn);
    if (txt == NULL) return;
 
    /* CE ?? clear everything */
    if (strcmp(txt, "CE") == 0) {
        memset(calc->curr_expr, 0, CALC_EXPR_LEN);
        memset(calc->last_expr, 0, CALC_EXPR_LEN);
        calc->curr_len = 0;
        calc->state    = CALC_STATE_INPUT;
        update_display(calc);
        return;
    }
 
    /* C ?? backspace */
    if (strcmp(txt, "C") == 0) {
        if (calc->state == CALC_STATE_RESULT || calc->state == CALC_STATE_ERROR) {
            memset(calc->curr_expr, 0, CALC_EXPR_LEN);
            calc->curr_len = 0;
            calc->state    = CALC_STATE_INPUT;
        } else if (calc->curr_len > 0) {
            calc->curr_expr[--calc->curr_len] = '\0';
        }
        update_display(calc);
        return;
    }
 
    /* = ?? evaluate */
    if (strcmp(txt, "=") == 0) {
        if (calc->curr_len == 0) return;
        if (!brackets_balanced(calc->curr_expr, calc->curr_len)) {
            strncpy(calc->last_expr, calc->curr_expr, CALC_EXPR_LEN - 1);
            strncpy(calc->curr_expr, "Bracket err", CALC_EXPR_LEN - 1);
            calc->curr_len = strlen(calc->curr_expr);
            calc->state    = CALC_STATE_ERROR;
            update_display(calc);
            return;
        }
        int32_t result = calc_evaluate(calc->curr_expr);
        strncpy(calc->last_expr, calc->curr_expr, CALC_EXPR_LEN - 1);
        memset(calc->curr_expr, 0, CALC_EXPR_LEN);
        if (result == INT32_MAX) {
            strncpy(calc->curr_expr, "Error", CALC_EXPR_LEN - 1);
            calc->state = CALC_STATE_ERROR;
        } else {
            format_result(result, calc->curr_expr, CALC_EXPR_LEN);
            calc->state = CALC_STATE_RESULT;
        }
        calc->curr_len = strlen(calc->curr_expr);
        update_display(calc);
        return;
    }
 
    /* digit / operator / bracket input */
    if (calc->state == CALC_STATE_RESULT) {
        if (isdigit((unsigned char)txt[0]) || txt[0] == '(') {
            memset(calc->curr_expr, 0, CALC_EXPR_LEN);
            calc->curr_len = 0;
        }
        calc->state = CALC_STATE_INPUT;
    }
    if (calc->state == CALC_STATE_ERROR) {
        memset(calc->curr_expr, 0, CALC_EXPR_LEN);
        calc->curr_len = 0;
        calc->state    = CALC_STATE_INPUT;
    }
 
    if (!can_append(calc, txt)) return;
 
    uint8_t tlen = (uint8_t)strlen(txt);
    if (calc->curr_len + tlen >= CALC_EXPR_LEN - 1) {
        lv_label_set_text(calc->lbl_last, "Expr full!");
        return;
    }
    strcat(calc->curr_expr, txt);
    calc->curr_len += tlen;
    update_display(calc);
}

static void calc_app_load(app_t* self, lv_obj_t* parent){
	calc_app_t* calc = (calc_app_t*)self;

	// last expression label
	calc->lbl_last = lv_label_create(parent);
	lv_label_set_text(calc->lbl_last, " ");
	lv_label_set_long_mode(calc->lbl_last, LV_LABEL_LONG_CLIP);
	
	lv_obj_set_size(calc->lbl_last, hor_res, LV_SIZE_CONTENT);   // width grows with content
	lv_obj_set_style_text_align(calc->lbl_last, LV_TEXT_ALIGN_RIGHT, 0);
	lv_obj_set_style_align(calc->lbl_last, LV_ALIGN_TOP_RIGHT, 0);
	lv_obj_set_y(calc->lbl_last, 5);

	lv_obj_add_style(calc->lbl_last,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(calc->lbl_last, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(calc->lbl_last, LV_OBJ_FLAG_EVENT_BUBBLE);

	// current expression label
	calc->lbl_curr = lv_label_create(parent);
	lv_label_set_text(calc->lbl_curr, "0");
	lv_label_set_long_mode(calc->lbl_curr, LV_LABEL_LONG_CLIP);
	
	lv_obj_set_size(calc->lbl_curr, hor_res, 20);   // width grows with content
	lv_obj_set_style_text_align(calc->lbl_curr, LV_TEXT_ALIGN_RIGHT, 0);
	lv_obj_set_style_align(calc->lbl_curr, LV_ALIGN_TOP_RIGHT, 0);
	lv_obj_set_y(calc->lbl_curr, 30);

	lv_obj_add_style(calc->lbl_curr,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(calc->lbl_curr, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(calc->lbl_curr, LV_OBJ_FLAG_EVENT_BUBBLE);

	// snapshot, works really well but takes 7.5kb
	// lv_obj_t * curr_expr_img = lv_img_create(parent);
	// lv_img_dsc_t * snapshot = lv_snapshot_take(calc->lbl_curr, LV_IMG_CF_TRUE_COLOR_ALPHA);
	// lv_img_set_src(curr_expr_img, snapshot);
	// lv_obj_set_style_align(curr_expr_img, LV_ALIGN_TOP_RIGHT, 0);
	// lv_obj_set_y(curr_expr_img, 50);
	// lv_obj_set_style_transform_zoom(curr_expr_img, 512, LV_PART_MAIN | LV_STATE_DEFAULT);  // 2x zoom (256 = 1x)
	// lv_obj_set_style_transform_pivot_x(curr_expr_img, (snapshot->header.w), LV_PART_MAIN | LV_STATE_DEFAULT);
	// lv_obj_set_style_transform_pivot_y(curr_expr_img, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

	// btnm
	lv_obj_t* btnm = lv_btnmatrix_create(parent);
	lv_obj_add_style(btnm,&lv_app_styles.color_combo1,LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_btnmatrix_set_map(btnm, btnm_map);
	lv_obj_set_size(btnm, hor_res-10, ver_res-50-40);
	lv_obj_align(btnm, LV_ALIGN_CENTER, 0, 30);

	lv_obj_add_style(btnm, &lv_app_styles.btnm_main, LV_PART_MAIN);
	lv_obj_add_event_cb( btnm, calc_btnm_cb, LV_EVENT_VALUE_CHANGED, calc);
	lv_obj_clear_flag( btnm, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_flag( btnm, LV_OBJ_FLAG_EVENT_BUBBLE);
	// lv_obj_add_event_cb( btnm, btnm_draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);

}

static void calc_app_exit(app_t* self){
	// TODO
}

calc_app_t* create_calc_app(void){
	calc_app_t *calc = lv_mem_alloc(sizeof(calc_app_t));
	if(calc == NULL){
		printf("Failed to allocate memory for calc_app_t\r\n");
		return NULL;
	}
	lv_memset_00(calc, sizeof(calc_app_t));

	assert_param(strlen("Calc") < sizeof(calc->app_base.app_name)-1);	// ensure name fits in buffer
	assert_param(strlen("\xEF\x87\xAC") < sizeof(calc->app_base.app_icon)-1);	// ensure icon fits in buffer
	strcpy(calc->app_base.app_name, "Calc");
	memcpy(calc->app_base.app_icon, "\xEF\x87\xAC", sizeof("\xEF\x87\xAC"));

	calc->app_base.app_t_load = calc_app_load;
	calc->app_base.app_t_exit = calc_app_exit;
	calc->curr_len = 0;
	calc->state = CALC_STATE_INPUT;

	return calc;
}

