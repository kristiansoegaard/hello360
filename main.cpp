// hello360 - Xbox 360 homebrew demo (libxenon / Free60)
// Shows live controller state on a text console.
// Hold BACK + START for about 1 second to reboot.
//
// Written in plain C++98 style (no exceptions, no RTTI) so it builds on
// the older GCC that libxenon's toolchain uses.

#include <stdio.h>
#include <string.h>

extern "C" {
#include <xenos/xenos.h>
#include <console/console.h>
#include <input/input.h>
#include <usb/usbmain.h>
#include <time/time.h>
#include <xenon_smc/xenon_smc.h>
}

static const int  MAX_PADS        = 4;     // controller ports checked
static const int  FRAME_MS        = 16;    // ~60 polls per second
static const int  EXIT_HOLD_MS    = 1000;  // hold Back+Start this long to reboot
static const int  STICK_DEADZONE  = 4000;  // ignore tiny stick drift (range +-32767)
static const int  FORCE_REDRAW_MS = 1000;  // redraw at least this often

static int deadzone(int v) {
    return (v > -STICK_DEADZONE && v < STICK_DEADZONE) ? 0 : v;
}

class Demo {
public:
    Demo() : running(true), exitHeldMs(0), sinceRedrawMs(0),
             activePad(-1), haveLast(false) {
        memset(&last, 0, sizeof(last));
    }

    void init() {
        xenos_init(VIDEO_MODE_AUTO);   // video output
        console_init();                // text console
        usb_init();                    // USB stack
        usb_do_poll();
        printf("hello360 starting...\n");
    }

    void run() {
        while (running) {
            usb_do_poll();             // must be called every loop for input

            controller_data_s pad;
            int port = findController(pad);

            // Controller plugged in, removed, or moved to another port
            bool portChanged = (port != activePad);
            activePad = port;

            if (port < 0) {
                exitHeldMs = 0;
                if (portChanged || sinceRedrawMs >= FORCE_REDRAW_MS) {
                    drawNoController();
                    haveLast = false;
                    sinceRedrawMs = 0;
                }
            } else {
                bool changed = !haveLast || portChanged ||
                               memcmp(&pad, &last, sizeof(pad)) != 0;
                if (changed || sinceRedrawMs >= FORCE_REDRAW_MS) {
                    draw(pad, port);
                    last = pad;
                    haveLast = true;
                    sinceRedrawMs = 0;
                }

                // Require Back+Start held, so a stray press can't reboot
                if (pad.back && pad.start) exitHeldMs += FRAME_MS;
                else                       exitHeldMs = 0;
                if (exitHeldMs >= EXIT_HOLD_MS) running = false;
            }

            sinceRedrawMs += FRAME_MS;
            mdelay(FRAME_MS);
        }
        shutdown();
    }

private:
    bool running;
    int  exitHeldMs;
    int  sinceRedrawMs;
    int  activePad;
    bool haveLast;
    controller_data_s last;

    // Returns the first port with a controller, or -1. Zeroes pad first so
    // we never act on uninitialised data.
    int findController(controller_data_s &pad) {
        for (int i = 0; i < MAX_PADS; i++) {
            memset(&pad, 0, sizeof(pad));
            if (get_controller_data(&pad, i)) return i;
        }
        memset(&pad, 0, sizeof(pad));
        return -1;
    }

    void drawNoController() {
        console_clrscr();
        printf("hello360\n");
        printf("========\n\n");
        printf("No controller detected.\n");
        printf("Connect a wired Xbox 360 controller.\n");
        printf("(Checking ports 0-%d)\n", MAX_PADS - 1);
    }

    void draw(const controller_data_s &p, int port) {
        console_clrscr();
        printf("hello360  -  controller on port %d\n", port);
        printf("================================\n\n");

        printf("Left stick : x=%6d  y=%6d\n", deadzone(p.s1_x), deadzone(p.s1_y));
        printf("Right stick: x=%6d  y=%6d\n", deadzone(p.s2_x), deadzone(p.s2_y));
        printf("Triggers   : LT=%3d  RT=%3d\n\n", (int)p.lt, (int)p.rt);

        printf("Buttons: %s%s%s%s %s%s %s%s\n",
               p.a ? "[A]" : "", p.b ? "[B]" : "",
               p.x ? "[X]" : "", p.y ? "[Y]" : "",
               p.lb ? "[LB]" : "", p.rb ? "[RB]" : "",
               p.start ? "[START]" : "", p.back ? "[BACK]" : "");
        printf("D-pad  : %s%s%s%s\n\n",
               p.up ? "UP " : "", p.down ? "DOWN " : "",
               p.left ? "LEFT " : "", p.right ? "RIGHT" : "");

        printf("Hold BACK + START for 1 second to reboot.\n");
    }

    void shutdown() {
        console_clrscr();
        printf("Rebooting...\n");
        mdelay(500);
        xenon_smc_power_reboot();
        // If the reboot call ever returns, don't fall off the end of main:
        // stay alive instead of crashing.
        for (;;) {
            printf("Reboot request failed. Power off manually.\n");
            mdelay(2000);
        }
    }
};

int main() {
    Demo app;
    app.init();
    app.run();
    return 0;
}
