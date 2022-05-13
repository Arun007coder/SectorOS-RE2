#include "keyboard.h"

static keyboard_handler_t keyboard_handler = NULL;

void init_keyboard(keyboard_handler_t handler)
{
    keyboard_handler = handler;
    register_interrupt_handler(IRQ(1), &keyboard_callback);

    while(inb(KEYBOARD_COMMAND_PORT) & 0x1)
        inb(KEYBOARD_DATA_PORT);
    outb(KEYBOARD_COMMAND_PORT, 0xAE);
    outb(KEYBOARD_COMMAND_PORT, 0x20);
    uint8_t status = (inb(KEYBOARD_DATA_PORT) | 1) & ~0x10;
    outb(KEYBOARD_COMMAND_PORT, 0x60);
    outb(KEYBOARD_DATA_PORT, status);

    outb(KEYBOARD_DATA_PORT, 0xF4);
}

void keyboard_callback(registers_t reg)
{
    uint8_t scancode;
    for(int i = 1000; i > 0; i++)
    {
        if((inb(KEYBOARD_COMMAND_PORT) & 1) == 0) continue;
        scancode = inb(KEYBOARD_DATA_PORT);
        break;
    }

    if(keyboard_handler != NULL)
    {
        keyboard_handler(scancode);
    }
    else
    {
        printf("No keyboard handler\n");
    }
}