#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/multicore.h"

#include "Bluepad_main.h"
#include "SwitchDescriptors.h"

#include "bsp/board.h"
#include "tusb.h"

void blink_task(uint8_t times)
{
    for (uint8_t i = 0; i < times; i++)
    {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(100);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(100);
    }
    sleep_ms(500);
}

void send_switch_hid_report(uint16_t buttons)
{
    try
    {
        tud_task();
        if (tud_suspended())
        {
            blink_task(2);
            tud_remote_wakeup();
        }
        if (tud_hid_ready())
        {
            SwitchOutReport out_report = {
                .buttons = buttons,
                .hat = SWITCH_HAT_NOTHING,
                .lx = SWITCH_JOYSTICK_MID,
                .ly = SWITCH_JOYSTICK_MID,
                .rx = SWITCH_JOYSTICK_MID,
                .ry = SWITCH_JOYSTICK_MID};

            tud_hid_report(0, &out_report, sizeof(out_report));
        }
    }
    catch (int e)
    {
        tud_task();
        if (tud_suspended())
        {
            blink_task(3);
            tud_remote_wakeup();
        }
    }
}

int main()
{
    stdio_init_all();

    init_bluepad();
    sleep_ms(1000);

    tusb_init();

    // int conn = 0;
    uint16_t buttons;
    while (true)
    {
        get_button_state(&buttons);

        send_switch_hid_report(buttons);
    }

    return 0;
}
