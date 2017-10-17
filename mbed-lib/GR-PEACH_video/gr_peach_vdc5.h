/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
* Copyright (C) 2012 - 2015 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**************************************************************************//**
* @file         gr_peach_vdc5.h
* @version      1.00
* $Rev: 199 $
* $Date:: 2014-05-23 16:33:52 +0900#$
* @brief        Graphics driver wrapper function definitions in C
******************************************************************************/

#ifndef GR_PEACH_VDC5_H
#define GR_PEACH_VDC5_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    <stdlib.h>

#include    "r_typedefs.h"
#include    "r_vdc5.h"
#include    "pinmap.h"

#ifdef  __cplusplus
extern  "C"
{
#endif  /* __cplusplus */

/******************************************************************************
Macro definitions
******************************************************************************/

#define VSYNC_1_2_FH_TIMING    (858u)   /* Vsync signal 1/2fH phase timing */
#define VSYNC_1_4_FH_TIMING    (429u)   /* Vsync signal 1/4fH phase timing */
#define DEFAULT_INPUT_CLOCK    (66.67)  /* P1 clock VDC5 */
#define DEFAULT_OUTPUT_CLOCK   (66.67)  /* LVDS output clock */

#define     IMGCAP_SIZE_NTSC_HS         (122u)
#define     IMGCAP_SIZE_NTSC_HW         (720u)
#define     IMGCAP_SIZE_NTSC_VS         (16u)
#define     IMGCAP_SIZE_NTSC_VW         (240u)

#define     IMGCAP_SIZE_PAL_HS          (132u)
#define     IMGCAP_SIZE_PAL_HW          (720u)
#define     IMGCAP_SIZE_PAL_VS          (19u)
#define     IMGCAP_SIZE_PAL_VW          (280u)

typedef void (*vdc5_irq_handler)(uint32_t int_sense);

/* video channel select */
typedef enum {
    DRV_VIDEO_INPUT_CHANNEL_0 = 0,      /* video input channel 0    */
    DRV_VIDEO_INPUT_CHANNEL_1           /* video input channel 1    */
} drv_video_input_channel_t;

/* input pin control */
typedef enum {
    DRV_VIDEO_ADC_VINSEL_VIN1 = 0,      /* VIN1 input               */
    DRV_VIDEO_ADC_VINSEL_VIN2           /* VIN2 input               */
} drv_video_adc_vinsel_t;

/* graphics layer select */
typedef enum {
    DRV_GRAPHICS_LAYER_0 = 0,           /* graphics layer 0         */
    DRV_GRAPHICS_LAYER_1,               /* graphics layer 1         */
    DRV_GRAPHICS_LAYER_2,               /* graphics layer 2         */
    DRV_GRAPHICS_LAYER_3                /* graphics layer 3         */
} drv_graphics_layer_t;

/* error codes */
typedef enum {
    DRV_GRAPHICS_OK = 0,                /* Normal termination       */
    DRV_GRAPHICS_VDC5_ERR = -1,         /* VDC5 error               */
    DRV_GRAPHICS_FORMAT_ERR = -2,       /* Not support format       */
    DRV_GRAPHICS_LAYER_ERR = -3,        /* Invalid layer ID error   */
    DRV_GRAPHICS_CHANNEL_ERR = -4,      /* Invalid channel error    */
    DRV_GRAPHICS_VIDEO_NTSC_SIZE_ERR = -5, /* Video Write           */
    DRV_GRAPHICS_VIDEO_PAL_SIZE_ERR = -6,  /* Video Write           */
    DRV_GRAPHICS_PARAM_RANGE_ERR = -7   /* Parameter range error    */
} drv_graphics_error_t;

/* graphics layer format select */
typedef enum {
    DRV_GRAPHICS_FORMAT_YCBCR422 = 0,   /* YCbCr422                 */
    DRV_GRAPHICS_FORMAT_RGB565,         /* RGB565                   */
    DRV_GRAPHICS_FORMAT_RGB888,         /* RGB888                   */
    DRV_GRAPHICS_FORMAT_ARGB8888,       /* ARGB8888                 */
    DRV_GRAPHICS_FORMAT_ARGB4444        /* ARGB4444                 */
} drv_graphics_format_t;

/* video writing format select */
typedef enum {
    DRV_VIDEO_FORMAT_YCBCR422 = 0,      /* YCbCr422                 */
    DRV_VIDEO_FORMAT_RGB565,            /* RGB565                   */
    DRV_VIDEO_FORMAT_RGB888             /* RGB888                   */
} drv_video_format_t;


/* lcd tcon output pin select */
typedef enum {
    DRV_LCD_TCON_PIN_NON = -1,          /* Not using output         */
    DRV_LCD_TCON_PIN_0,                 /* LCD_TCON0                */
    DRV_LCD_TCON_PIN_1,                 /* LCD_TCON1                */
    DRV_LCD_TCON_PIN_2                  /* LCD_TCON2                */
} drv_lcd_tcon_pin_t;

/* lcd output format select */
typedef enum {
    DRV_LCD_OUTFORMAT_RGB888 = 0,       /* RGB888 or LVDS           */
    DRV_LCD_OUTFORMAT_RGB666,           /* RGB666                   */
    DRV_LCD_OUTFORMAT_RGB565            /* RGB565                   */
} drv_lcd_outformat_t;

/* frame buffer swap setting */
typedef enum {
    DRV_WR_RD_WRSWA_NON = 0,           /* Not swapped: 1-2-3-4-5-6-7-8 */
    DRV_WR_RD_WRSWA_8BIT,              /* Swapped in 8-bit units: 2-1-4-3-6-5-8-7 */
    DRV_WR_RD_WRSWA_16BIT,             /* Swapped in 16-bit units: 3-4-1-2-7-8-5-6 */
    DRV_WR_RD_WRSWA_16_8BIT,           /* Swapped in 16-bit units + 8-bit units: 4-3-2-1-8-7-6-5 */
    DRV_WR_RD_WRSWA_32BIT,             /* Swapped in 32-bit units: 5-6-7-8-1-2-3-4 */
    DRV_WR_RD_WRSWA_32_8BIT,           /* Swapped in 32-bit units + 8-bit units: 6-5-8-7-2-1-4-3 */
    DRV_WR_RD_WRSWA_32_16BIT,          /* Swapped in 32-bit units + 16-bit units: 7-8-5-6-3-4-1-2 */
    DRV_WR_RD_WRSWA_32_16_8BIT,        /* Swapped in 32-bit units + 16-bit units + 8-bit units: 8-7-6-5-4-3-2-1 */
} drv_wr_rd_swa_t;

/* edge of a signal */
typedef enum {
    DRV_EDGE_RISING    = 0,             /* Rising edge              */
    DRV_EDGE_FALLING   = 1              /* Falling edge             */
} drv_edge_t;

/* lcd type */
typedef enum {
    DRV_LCD_TYPE_LVDS = 0,              /* LVDS signal control      */
    DRV_LCD_TYPE_PARALLEL_RGB           /* RGB parallel signal control */
} drv_lcd_type_t;

/* Polarity of a signal */
typedef enum {
    DRV_SIG_POL_NOT_INVERTED = 0,       /* Not inverted             */
    DRV_SIG_POL_INVERTED                /* Inverted                 */
} drv_sig_pol_t;

/* Video color system */
typedef enum {
    DRV_COL_SYS_NTSC_358       = 0,        /* NTSC-3.58 */
    DVV_COL_SYS_NTSC_443       = 1,        /* NTSC-4.43 */
    DRV_COL_SYS_PAL_443        = 2,        /* PAL-4.43 */
    DRV_COL_SYS_PAL_M          = 3,        /* PAL-M */
    DRV_COL_SYS_PAL_N          = 4,        /* PAL-N */
    DRV_COL_SYS_SECAM          = 5,        /* SECAM */
    DRV_COL_SYS_NTSC_443_60    = 6,        /* NTSC-4.43 (60Hz) */
    DRV_COL_SYS_PAL_60         = 7,        /* PAL-60 */
} drv_graphics_video_col_sys_t;

/* External Input select */
typedef enum {
    DRV_INPUT_SEL_VDEC     = 0,            /*!< Video decoder output signals */
    DRV_INPUT_SEL_EXT      = 1             /*!< Signals supplied via the external input pins */
} drv_video_input_sel_t;

/* External input format select  */
typedef enum {
    DRV_VIDEO_EXTIN_FORMAT_RGB888 = 0,   /*!< RGB888 Not support */
    DRV_VIDEO_EXTIN_FORMAT_RGB666,       /*!< RGB666 */
    DRV_VIDEO_EXTIN_FORMAT_RGB565,       /*!< RGB565 */
    DRV_VIDEO_EXTIN_FORMAT_BT656,        /*!< BT6556 */
    DRV_VIDEO_EXTIN_FORMAT_BT601,        /*!< BT6501 */
    DRV_VIDEO_EXTIN_FORMAT_YCBCR422,     /*!< YCbCr422 */
    DRV_VIDEO_EXTIN_FORMAT_YCBCR444,     /*!< YCbCr444 Not support */
} drv_video_extin_format_t;

/* On/off */
typedef enum {
    DRV_OFF    = 0,                      /*!< Off */
    DRV_ON     = 1                       /*!< On */
} drv_onoff_t;

/* Number of lines for BT.656 external input  */
typedef enum {
    DRV_EXTIN_LINE_525     = 0,    /*!< 525 lines */
    DRV_EXTIN_LINE_625     = 1     /*!< 625 lines */
} drv_extin_input_line_t;

/* Y/Cb/Y/Cr data string start timing */
typedef enum {
    DRV_EXTIN_H_POS_CBYCRY = 0,    /*!< Cb/Y/Cr/Y (BT656/601), Cb/Cr (YCbCr422) */
    DRV_EXTIN_H_POS_YCRYCB,        /*!< Y/Cr/Y/Cb (BT656/601), setting prohibited (YCbCr422) */
    DRV_EXTIN_H_POS_CRYCBY,        /*!< Cr/Y/Cb/Y (BT656/601), setting prohibited (YCbCr422) */
    DRV_EXTIN_H_POS_YCBYCR,        /*!< Y/Cb/Y/Cr (BT656/601), Cr/Cb (YCbCr422) */
} drv_extin_h_pos_t;

/* The relative position within the graphics display area */
typedef struct {
    uint16_t   vs;                  /* Vertical start pos       */
    uint16_t   vw;                  /* Vertical width (height)  */
    uint16_t   hs;                  /* Horizontal start pos     */
    uint16_t   hw;                  /* Horizontal width         */
} drv_rect_t;

/* lcd configuration  */
typedef struct {
    drv_lcd_type_t      lcd_type;               /* LVDS or Pararel RGB                      */
    double              intputClock;            /* P1  clk [MHz] ex. 66.67                  */
    double              outputClock;            /* LCD clk [MHz] ex. 33.33                  */

    drv_lcd_outformat_t lcd_outformat;          /* Output format select */
    drv_edge_t          lcd_edge;               /* Output phase control of LCD_DATA23 to LCD_DATA0 pin */

    uint16_t            h_toatal_period;        /* Free-running Hsync period                */
    uint16_t            v_toatal_period;        /* Free-running Vsync period                */
    uint16_t            h_disp_widht;           /* LCD display area size, horizontal width  */
    uint16_t            v_disp_widht;           /* LCD display area size, vertical width    */
    uint16_t            h_back_porch;           /* LCD display horizontal back porch period */
    uint16_t            v_back_porch;           /* LCD display vertical back porch period   */

    drv_lcd_tcon_pin_t  h_sync_port;            /* TCONn or Not use(-1)                     */
    drv_sig_pol_t       h_sync_port_polarity;   /* Polarity inversion control of signal     */
    uint16_t            h_sync_width;           /* Hsync width                              */

    drv_lcd_tcon_pin_t  v_sync_port;            /* TCONn or Not use(-1)                     */
    drv_sig_pol_t       v_sync_port_polarity;   /* Polarity inversion control of signal     */
    uint16_t            v_sync_width;           /* Vsync width  */

    drv_lcd_tcon_pin_t  de_port;                /* TCONn or Not use(-1)                     */
    drv_sig_pol_t       de_port_polarity;       /* Polarity inversion control of signal     */

} drv_lcd_config_t;

/* Digital video input configuration  */
typedef struct {
    drv_video_extin_format_t     inp_format;    /*!< External Input Format Select                                */
    drv_edge_t                   inp_pxd_edge;  /*!< Clock Edge Select for Capturing External Input Video Image  */
    drv_edge_t                   inp_vs_edge;   /*!< Clock Edge Select for Capturing External Input Vsync Signal */
    drv_edge_t                   inp_hs_edge;   /*!< Clock Edge Select for Capturing External Input Hsync Signal */
    drv_onoff_t                  inp_endian_on; /*!< External Input B/R Signal Swap On/Off Control               */
    drv_onoff_t                  inp_swap_on;   /*!< External Input Bit Endian Change On/Off Control             */
    drv_sig_pol_t                inp_vs_inv;    /*!< External Input Vsync Signal DV_VSYNC Inversion Control      */
    drv_sig_pol_t                inp_hs_inv;    /*!< External Input Hsync Signal DV_HSYNC Inversion Control      */
    drv_extin_input_line_t       inp_f525_625;  /*!< Number of lines for BT.656 external input */
    drv_extin_h_pos_t            inp_h_pos;     /*!< Y/Cb/Y/Cr data string start timing to Hsync reference */
} drv_video_ext_in_config_t;

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/
drv_graphics_error_t DRV_Graphics_Init( drv_lcd_config_t * drv_lcd_config );
drv_graphics_error_t DRV_Graphics_Video_init( drv_video_input_sel_t drv_video_input_sel, drv_video_ext_in_config_t * drv_video_ext_in_config );

drv_graphics_error_t DRV_Graphics_Lcd_Port_Init( PinName *pin, uint32_t pin_count );
drv_graphics_error_t DRV_Graphics_Lvds_Port_Init( PinName *pin, uint32_t pin_count );
drv_graphics_error_t DRV_Graphics_Dvinput_Port_Init( PinName *pin, uint32_t pin_count );

drv_graphics_error_t DRV_Graphics_Irq_Handler_Set( vdc5_int_type_t irq, uint16_t num, void (* callback)(vdc5_int_type_t)  );

drv_graphics_error_t DRV_Graphics_Start ( drv_graphics_layer_t layer_id );
drv_graphics_error_t DRV_Graphics_Stop ( drv_graphics_layer_t layer_id );
drv_graphics_error_t DRV_Video_Start ( drv_video_input_channel_t video_input_ch );
drv_graphics_error_t DRV_Video_Stop ( drv_video_input_channel_t video_input_ch );

drv_graphics_error_t DRV_Graphics_Read_Setting (
    drv_graphics_layer_t    layer_id,
    void                  * framebuff,
    uint32_t                fb_stride,
    drv_graphics_format_t   gr_format,
    drv_wr_rd_swa_t         wr_rd_swa,
    drv_rect_t            * gr_rect );

drv_graphics_error_t DRV_Graphics_Read_Change (
    drv_graphics_layer_t    layer_id,
    void                 *  framebuff);

drv_graphics_error_t DRV_Video_Write_Setting (
    drv_video_input_channel_t       video_input_ch,
    drv_graphics_video_col_sys_t    col_sys,
    void                          * framebuff,
    uint32_t                        fb_stride,
    drv_video_format_t              video_format,
    drv_wr_rd_swa_t                 wr_rd_swa,
    uint16_t                        video_write_buff_vw,
    uint16_t                        video_write_buff_hw,
    drv_video_adc_vinsel_t          video_adc_vinsel );

drv_graphics_error_t DRV_Video_Write_Setting_Digital (
    void                          * framebuff,
    uint32_t                        fb_stride,
    drv_video_format_t              video_format,
    drv_wr_rd_swa_t                 wr_rd_swa,
    uint16_t                        video_write_buff_vw,
    uint16_t                        video_write_buff_hw,
    drv_rect_t                    * cap_area );

drv_graphics_error_t DRV_Video_Write_Change (
    drv_video_input_channel_t    video_input_ch,
    void                       * framebuff,
    uint32_t                     fb_stride );

#ifdef  __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GR_PEACH_VDC5_H */
