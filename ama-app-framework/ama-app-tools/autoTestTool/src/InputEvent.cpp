/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <linux/input.h>
#include <thread>
#include <memory>

#include "InputEvent.h"

struct input_event event;

#define test_bit(bit) (mask[(bit)/8] & (1 << ((bit)%8)))

static bool sIsSetRun = false;
std::shared_ptr<std::thread> sPrintEventThread;

void showTouchEventDevice(void)
{
    char          buf[256] = { 0, };  /* RATS: Use ok */
    unsigned char mask[EV_MAX/8 + 1]; /* RATS: Use ok */
    int           version;
    int           fd = 0;
    int           i;

    if ((fd = open(TOUCH_DEVICE, O_RDONLY, 0)) >= 0) {
        ioctl(fd, EVIOCGVERSION, &version);
        ioctl(fd, EVIOCGNAME(sizeof(buf)), buf);
        ioctl(fd, EVIOCGBIT(0, sizeof(mask)), mask);
        printf("%s\n", TOUCH_DEVICE);
        printf("    evdev version: %d.%d.%d\n",
               version >> 16, (version >> 8) & 0xff, version & 0xff);
        printf("    name: %s\n", buf);
        printf("    features:");
        for (i = 0; i < EV_MAX; i++) {
            if (test_bit(i)) {
                const char *type = "unknown";
                switch(i) {
                case EV_KEY: type = "keys/buttons"; break;
                case EV_REL: type = "relative";     break;
                case EV_ABS: type = "absolute";     break;
                case EV_MSC: type = "reserved";     break;
                case EV_LED: type = "leds";         break;
                case EV_SND: type = "sound";        break;
                case EV_REP: type = "repeat";       break;
                case EV_FF:  type = "feedback";     break;
                }
                printf(" %s", type);
            }
        }
        printf("\n");
        close(fd);
    }else{
        printf("open touch device:%s failed! \n", TOUCH_DEVICE);
    }
}

void printEvent(input_event event)
{
    char          *tmp;

    printf("%-24.24s.%06lu type 0x%04x; code 0x%04x;"
           " value 0x%08x; ",
           ctime(&event.time.tv_sec),
           event.time.tv_usec,
           event.type, event.code, event.value);
    switch (event.type) {
    case EV_SYN:
        printf("SYNC EVENT");
        break;
    case EV_KEY:
        if (event.code > BTN_MISC) {
            printf("Button %d %s",
                   event.code & 0xff,
                   event.value ? "press" : "release");
        } else {
            printf("Key %d (0x%x) %s",
                   event.code & 0xff,
                   event.code & 0xff,
                   event.value ? "press" : "release");
        }
        break;
    case EV_REL:
        switch (event.code) {
        case REL_X:      tmp = "X";       break;
        case REL_Y:      tmp = "Y";       break;
        case REL_HWHEEL: tmp = "HWHEEL";  break;
        case REL_DIAL:   tmp = "DIAL";    break;
        case REL_WHEEL:  tmp = "WHEEL";   break;
        case REL_MISC:   tmp = "MISC";    break;
        default:         tmp = "UNKNOWN"; break;
        }
        printf("Relative %s %d", tmp, event.value);
        break;
    case EV_ABS:
        switch (event.code) {
        case ABS_X:        tmp = "X";        break;
        case ABS_Y:        tmp = "Y";        break;
        case ABS_Z:        tmp = "Z";        break;
        case ABS_RX:       tmp = "RX";       break;
        case ABS_RY:       tmp = "RY";       break;
        case ABS_RZ:       tmp = "RZ";       break;
        case ABS_THROTTLE: tmp = "THROTTLE"; break;
        case ABS_RUDDER:   tmp = "RUDDER";   break;
        case ABS_WHEEL:    tmp = "WHEEL";    break;
        case ABS_GAS:      tmp = "GAS";      break;
        case ABS_BRAKE:    tmp = "BRAKE";    break;
        case ABS_HAT0X:    tmp = "HAT0X";    break;
        case ABS_HAT0Y:    tmp = "HAT0Y";    break;
        case ABS_HAT1X:    tmp = "HAT1X";    break;
        case ABS_HAT1Y:    tmp = "HAT1Y";    break;
        case ABS_HAT2X:    tmp = "HAT2X";    break;
        case ABS_HAT2Y:    tmp = "HAT2Y";    break;
        case ABS_HAT3X:    tmp = "HAT3X";    break;
        case ABS_HAT3Y:    tmp = "HAT3Y";    break;
        case ABS_PRESSURE: tmp = "PRESSURE"; break;
        case ABS_DISTANCE: tmp = "DISTANCE"; break;
        case ABS_TILT_X:   tmp = "TILT_X";   break;
        case ABS_TILT_Y:   tmp = "TILT_Y";   break;
        case ABS_MISC:     tmp = "MISC";     break;
        case ABS_MT_POSITION_X: tmp = "Center X Touch pos";break;
        case ABS_MT_POSITION_Y: tmp = "Center Y Touch pos";break;
        case ABS_MT_TRACKING_ID: tmp = "Unique ID of initiated contact";break;
        default:           tmp = "UNKNOWN";  break;
        }
        printf("Absolute %s %d", tmp, event.value);
        break;
    case EV_MSC: printf("Misc"); break;
    case EV_LED: printf("Led");  break;
    case EV_SND: printf("Snd");  break;
    case EV_REP: printf("Rep");  break;
    case EV_FF:  printf("FF");   break;
        break;
    }
    printf("\n");
}

void printTouchEvent(void)
{
    int           fd = 0;
    int           rc;
    // int           i, j;

    if ((fd = open(TOUCH_DEVICE, O_RDWR, 0)) >= 0) {
        printf("%s: open, fd = %d\n", TOUCH_DEVICE, fd);

        while ((rc = read(fd, &event, sizeof(event))) > 0) {
            printEvent(event);
        }
        printf("rc = %d, (%s)\n", rc, strerror(errno));
        close(fd);
    }else{
        printf("open touch device:%s failed! \n", TOUCH_DEVICE);
    }
}

void printInputEventRun()
{
    int           fd = 0;
    int           rc;

    if ((fd = open(TOUCH_DEVICE, O_RDWR, 0)) >= 0) {
        printf("%s: open, fd = %d\n", TOUCH_DEVICE, fd);

        while(sIsSetRun){
            if((rc = read(fd, &event, sizeof(event))) > 0){
                if(!sIsSetRun){
                    break;
                }
                printEvent(event);
                if(!sIsSetRun){
                    break;
                }
            }
        }

        printf("rc = %d, (%s)\n", rc, strerror(errno));
        close(fd);
    }else{
        printf("open touch device:%s failed! \n", TOUCH_DEVICE);
    }
}

void startPrintInputEvent(){
    sIsSetRun = true;
    sPrintEventThread = std::make_shared<std::thread>(printInputEventRun);
}

void stopPrintInputEvent(){
    sIsSetRun = false;
}
