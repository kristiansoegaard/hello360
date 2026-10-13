// Xbox 360 homebrew demo (libxenon / Free60)
// Shows a text console and live controller state.
// Back + Start together reboots the console.

#include <stdio.h>
#include <xenos/xenos.h>
#include <console/console.h>
#include <input/input.h>
#include <usb/usbmain.h>
#include <time/time.h>
#include <xenon_smc/xenon_smc.h>

class Demo {
public:
    Demo() : frame(0), running(true) {}

    void init() {
        xenos_init(VIDEO_MODE_AUTO);   // set up GPU / video output
        console_init();                // text console on top of framebuffer
        usb_init();                    // USB stack (controllers)
        usb_do_poll();
    }

    void run() {
        while (running) {
            usb_do_poll();             // must be called regularly for input
            struct controller_data_s pad;

            if (get_controller_data(&pad, 0)) {
                draw(pad);
                if (pad.back && pad.start) running = false;
            } else {
                console_clrscr();
                printf("Xbox 360 Homebrew Demo\n\n");
                printf("No controller detected on port 0.\n");
                printf("Connect a wired controller.\n");
            }

            frame++;
            mdelay(16);                // ~60 updates per second
        }
        printf("\nRebooting...\n");
        mdelay(500);
        xenon_smc_power_reboot();
    }

private:
    unsigned long frame;
    bool running;

    void draw(const controller_data_s &p) {
        console_clrscr();
        printf("Xbox 360 Homebrew Demo  (frame %lu)\n", frame);
        printf("================================\n\n");

        printf("Left stick : x=%6d  y=%6d\n", p.s1_x, p.s1_y);
        printf("Right stick: x=%6d  y=%6d\n", p.s2_x, p.s2_y);
        printf("Triggers   : LT=%3d  RT=%3d\n\n", p.lt, p.rt);

        printf("Buttons: %s%s%s%s %s%s %s%s\n",
               p.a ? "[A]" : "", p.b ? "[B]" : "",
               p.x ? "[X]" : "", p.y ? "[Y]" : "",
               p.lb ? "[LB]" : "", p.rb ? "[RB]" : "",
               p.start ? "[START]" : "", p.back ? "[BACK]" : "");
        printf("D-pad  : %s%s%s%s\n\n",
               p.up ? "UP " : "", p.down ? "DOWN " : "",
               p.left ? "LEFT " : "", p.right ? "RIGHT" : "");

        printf("Press BACK + START to reboot.\n");
    }
};

int main() {
    Demo app;
    app.init();
    app.run();
    return 0;
}
