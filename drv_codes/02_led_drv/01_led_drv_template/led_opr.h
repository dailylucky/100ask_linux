#ifndef __LED_OPR_H
#define __LED_OPR_H

struct led_operations {
    int (*init)(int which); /* Init led, which: which led */
    int (*ctl)(int which, char status); /* Control led specified by which */
};

struct led_operations *get_board_led_opr(void);

#endif /* __LED_OPR_H */


