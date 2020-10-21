/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#ifdef   __cplusplus
extern "C"
{
#endif

#include <ctype.h>

#ifndef __ICCARM__
#include "stdbool.h"
#endif
#include "FreeRTOS.h"
#include "task.h"
#include "r_typedefs.h"
#include "r_vdc_portsetting.h"
#include "compiler_settings.h"
#include "r_rvapi_header.h"
#include "r_display_init.h"
#include "string.h"

/******************************************************************************
 Macro definitions
 ******************************************************************************/
/* diplay layer for rectangle */
#if defined FRAME_BUFFER_BITS_PER_PIXEL_16
#define DATA_SIZE_PER_PIC      (2u)
#elif defined FRAME_BUFFER_BITS_PER_PIXEL_32
#define DATA_SIZE_PER_PIC      (4u)
#else
#error "Set bits per pixel"
#endif

#define FRAMEBUFFER_WIDTH   480
#define FRAMEBUFFER_HEIGHT  272

#define FRAMEBUFFER_LAYER_NUM      (2u)
#define FRAMEBUFFER_STRIDE  (((FRAMEBUFFER_WIDTH * DATA_SIZE_PER_PIC) + 31u) & ~31u)

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/

/******************************************************************************
 Imported global variables and functions (from other files)
 ******************************************************************************/

/****************************************************************************/
/* VRAM definitions                                                         */
/****************************************************************************/
static volatile int32_t vsync_count = 0;
static int draw_buffer_index = 0;

#if __ICCARM__ == 1
#pragma data_alignment=8
uint8_t framebuffer[FRAMEBUFFER_LAYER_NUM][FRAMEBUFFER_STRIDE * FRAMEBUFFER_HEIGHT] @ "VRAM_SECTION0";
#else
uint8_t framebuffer[FRAMEBUFFER_LAYER_NUM][FRAMEBUFFER_STRIDE * FRAMEBUFFER_HEIGHT] __attribute__ ((section(".VRAM_SECTION0")));
#endif

// graphic buffer cursor_buffer is already defined in BSP/src/renesas/application/app_touchscreen/r_drawrectangle.c
volatile uint8_t *RZAFrameBuffers[2] =
{
    framebuffer[1],
    framebuffer[0],
};

static void IntCallbackFunc_LoVsync(vdc_int_type_t int_type)
{
    if (vsync_count > 0)
    {
        vsync_count--;
    }
}

static void Wait_Vsync(const int32_t wait_count)
{
    vsync_count = wait_count;
    while (vsync_count > 0)
    {
        vTaskDelay( 2 / portTICK_PERIOD_MS);
    }
}

/* Set / switch framebuffer */
void GrpDrv_SetFrameBuffer(void * ptr)
{
    if (draw_buffer_index == 1) {
        draw_buffer_index = 0;
    } else {
        draw_buffer_index = 1;
    }

    vdc_error_t error = R_RVAPI_GraphChangeSurfaceVDC(VDC_CHANNEL_0, VDC_LAYER_ID_0_RD, (void*)framebuffer[draw_buffer_index]);
    if (VDC_OK == error)
    {
         Wait_Vsync(1);
    }
}

void GRAPHIC_Clear(void)
{
    memset(framebuffer[draw_buffer_index], 0x0, FRAMEBUFFER_STRIDE * FRAMEBUFFER_HEIGHT);
}

void GRAPHIC_PutPixel(uint32_t x, uint32_t y, uint8_t color)
{
    for (int pixel = 0; pixel < DATA_SIZE_PER_PIC; ++pixel)
    {
        framebuffer[draw_buffer_index][(y * FRAMEBUFFER_STRIDE) + x * DATA_SIZE_PER_PIC + pixel] = color;
    }
}

void GRAPHIC_Rectangle( uint32_t begin_x, uint32_t begin_y, uint32_t width, uint32_t height, uint8_t color )
{
    uint32_t h = 0;
    uint32_t w = 0;

    /* new rectangle */
    h = begin_y;
    for (w = begin_x; w < (begin_x + width) ; w++)
    {
        GRAPHIC_PutPixel(w, h, color);
    }
    for (h = begin_y + 1; h < ((begin_y + height) - 1); h++)
    {
        GRAPHIC_PutPixel(begin_x, h, color);
        GRAPHIC_PutPixel(w, h, color);
    }
    for (w = begin_x; w < (begin_x + width); w++)
    {
        GRAPHIC_PutPixel(w, h, color);
    }
}

extern "C" void GRAPHIC_init_screen(void)
{
    vdc_error_t error;
    vdc_channel_t vdc_ch = VDC_CHANNEL_0;

    /***********************************************************************/
    /* display init (VDC output setting) */
    /***********************************************************************/
    {
        error = r_display_init (vdc_ch);
    }
    if (error == VDC_OK)
    {
        error = R_RVAPI_InterruptEnableVDC(vdc_ch, VDC_INT_TYPE_S0_LO_VSYNC, 0, IntCallbackFunc_LoVsync);
    }
    /***********************************************************************/
    /* Graphic Layer 2 CLUT8 */
    /***********************************************************************/
    if (error == VDC_OK)
    {
        gr_surface_disp_config_t gr_disp_cnf;
        uint32_t  clut_table[4] = {
                0x00000000, /* No.0 transparent color  */
                0xFF000000, /* No.1 black */
                0xFF00FF00, /* No.2 green */
                0xFFFF0000  /* No.3 red */
        };

        /* buffer clear */
        // Set frame buffer to black
        memset((void*)framebuffer[0], 0x00, FRAMEBUFFER_STRIDE * FRAMEBUFFER_HEIGHT);
        memset((void*)framebuffer[1], 0x00, FRAMEBUFFER_STRIDE * FRAMEBUFFER_HEIGHT);

#if (1) /* not use camera captured layer */
        gr_disp_cnf.layer_id         = VDC_LAYER_ID_0_RD;
#else   /* blend over camera captured image */
        gr_disp_cnf.layer_id         = VDC_LAYER_ID_2_RD;
#endif
        gr_disp_cnf.disp_area.hs_rel = 0;
        gr_disp_cnf.disp_area.hw_rel = FRAMEBUFFER_WIDTH;
        gr_disp_cnf.disp_area.vs_rel = 0;
        gr_disp_cnf.disp_area.vw_rel = FRAMEBUFFER_HEIGHT;
        gr_disp_cnf.fb_buff          = &framebuffer[0];
        gr_disp_cnf.fb_stride        = FRAMEBUFFER_STRIDE;
        gr_disp_cnf.read_format      = VDC_GR_FORMAT_CLUT8;
#if defined FRAME_BUFFER_BITS_PER_PIXEL_16
        gr_disp_cnf.read_format      = VDC_GR_FORMAT_RGB565;
#elif defined FRAME_BUFFER_BITS_PER_PIXEL_32
        gr_disp_cnf.read_format      = VDC_GR_FORMAT_RGB888;
#endif
        gr_disp_cnf.clut_table       = clut_table;
        gr_disp_cnf.read_ycc_swap    = VDC_GR_YCCSWAP_CBY0CRY1;
        gr_disp_cnf.read_swap        = VDC_WR_RD_WRSWA_32_16BIT;
#if (1) /* not use camera captured data */
        gr_disp_cnf.disp_mode        = VDC_DISPSEL_CURRENT;
#else   /* blend over camera captured image */
        gr_disp_cnf.disp_mode        = VDC_DISPSEL_BLEND;
#endif
        error = R_RVAPI_GraphCreateSurfaceVDC(vdc_ch, &gr_disp_cnf);

        GRAPHIC_Clear();
    }

    /* Image Quality Adjustment */
    if (VDC_OK == error)
    {
        error = r_image_quality_adjustment(vdc_ch);
    }

    /* Enable signal output */
    if (VDC_OK == error)
    {
        /* Wait for register update */
        R_OS_TaskSleep(5);

        R_RVAPI_DispPortSettingVDC(vdc_ch, &VDC_LcdPortSetting);
    }
}

void GRAPHIC_test(unsigned char color)
{
    GRAPHIC_Clear();
    for (int y = 0; y < FRAMEBUFFER_HEIGHT; ++y)
    {
        for (int x = 0; x < FRAMEBUFFER_WIDTH; ++x)
        {
            GRAPHIC_PutPixel(x, y, ((x * y) + color) & 0xff);
        }
    }
}

#ifdef   __cplusplus
}
#endif
