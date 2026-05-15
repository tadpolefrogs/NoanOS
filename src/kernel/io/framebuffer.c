#include "io/framebuffer.h"
#include "io/kprint.h"
#include "core/types.h"

static fb_info_t fb;

void fb_init(void) {
    /* Check for a VBE ModeInfoBlock written by the bootloader at 0x9000.
     * ModeInfoBlock physical base pointer is at offset 0x40 (per VBE spec).
     * X/Y resolution at offsets 0x12/0x14, BytesPerScanLine at 0x0E, BitsPerPixel at 0x19.
     */
    volatile u8* mib = (volatile u8*)0x9000;
    u32 phys = *(u32*)(0x9000 + 0x40);
    if (phys != 0) {
        fb.addr = (void*)phys;
        fb.width = *(u16*)(0x9000 + 0x12);
        fb.height = *(u16*)(0x9000 + 0x14);
        fb.pitch = *(u16*)(0x9000 + 0x0E);
        fb.bpp = *(u8*)(0x9000 + 0x19);
        /* If pitch is zero, fall back to width * (bpp/8) */
        if (fb.pitch == 0) fb.pitch = fb.width * ((fb.bpp + 7) / 8);
        fb_fill(0);
        return;
    }

    /* Fallback: VGA mode 13h linear buffer at 0xA0000 (320x200x8). */
    fb.addr = (void*)0xA0000;
    fb.width = 320;
    fb.height = 200;
    fb.pitch = 320;
    fb.bpp = 8;
    fb_fill(0);
}

void fb_put_pixel(u32 x, u32 y, u32 color) {
    if (x >= fb.width || y >= fb.height) return;
    if (fb.bpp == 8) {
        volatile u8* p = (volatile u8*)fb.addr + y * fb.pitch + x;
        *p = (u8)color;
    } else if (fb.bpp == 32) {
        volatile u32* p = (volatile u32*)fb.addr + y * (fb.pitch/4) + x;
        *p = (u32)color;
    }
}

void fb_fill(u32 color) {
    if (fb.bpp == 8) {
        volatile u8* p = (volatile u8*)fb.addr;
        for (u32 y = 0; y < fb.height; y++) {
            for (u32 x = 0; x < fb.pitch; x++) {
                p[y * fb.pitch + x] = (u8)color;
            }
        }
    } else if (fb.bpp == 32) {
        volatile u32* p = (volatile u32*)fb.addr;
        u32 pixels = fb.pitch / 4;
        for (u32 y = 0; y < fb.height; y++) {
            for (u32 x = 0; x < pixels; x++) {
                p[y * pixels + x] = color;
            }
        }
    }
}

fb_info_t* fb_get_info(void) {
    return &fb;
}
