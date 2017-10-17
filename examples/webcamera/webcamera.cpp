#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"

#include "webcamera.h"
#include "mbed.h"
#include "DisplayBace.h"
//#include "rtos.h"
#include "JPEG_Converter.h"
#include "EthernetInterface.h"
#include "HTTPServer.h"
#include "mbed_rpc.h"
#include "RomRamFileSystem.h"
#include "file_table.h"        //Binary data of web pages
#include "i2c_setting.h"

#include "syssvc/logtask.h"

#define VIDEO_CVBS             (0)                 /* Analog  Video Signal */
#define VIDEO_CMOS_CAMERA      (1)                 /* Digital Video Signal */
#define VIDEO_YCBCR422         (0)
#define VIDEO_RGB888           (1)
#define VIDEO_RGB565           (2)

/*
 *  サービスコールのエラーのログ出力
 */
Inline void
svc_perror(const char *file, int_t line, const char *expr, ER ercd)
{
	if (ercd < 0) {
		t_perror(LOG_ERROR, file, line, expr, ercd);
	}
}

#define	SVC_PERROR(expr)	svc_perror(__FILE__, __LINE__, #expr, (expr))

/**** User Selection *********/
/** Network setting **/
#define USE_DHCP               (1)                 /* Select  0(static configuration) or 1(use DHCP) */
#if (USE_DHCP == 0)
  #define IP_ADDRESS           ("192.168.0.2")     /* IP address      */
  #define SUBNET_MASK          ("255.255.255.0")   /* Subnet mask     */
  #define DEFAULT_GATEWAY      ("192.168.0.3")     /* Default gateway */
#endif
/** Camera setting **/
#define VIDEO_INPUT_METHOD     (VIDEO_CMOS_CAMERA) /* Select  VIDEO_CVBS or VIDEO_CMOS_CAMERA                       */
#define VIDEO_INPUT_FORMAT     (VIDEO_YCBCR422)    /* Select  VIDEO_YCBCR422 or VIDEO_RGB888 or VIDEO_RGB565        */
#define USE_VIDEO_CH           (0)                 /* Select  0 or 1            If selecting VIDEO_CMOS_CAMERA, should be 0.)               */
#define VIDEO_PAL              (0)                 /* Select  0(NTSC) or 1(PAL) If selecting VIDEO_CVBS, this parameter is not referenced.) */
/*****************************/

#if USE_VIDEO_CH == (0)
#define VIDEO_INPUT_CH         (DisplayBase::VIDEO_INPUT_CHANNEL_0)
#define VIDEO_INT_TYPE         (DisplayBase::INT_TYPE_S0_VFIELD)
#else
#define VIDEO_INPUT_CH         (DisplayBase::VIDEO_INPUT_CHANNEL_1)
#define VIDEO_INT_TYPE         (DisplayBase::INT_TYPE_S1_VFIELD)
#endif

#if ( VIDEO_INPUT_FORMAT == VIDEO_YCBCR422 || VIDEO_INPUT_FORMAT == VIDEO_RGB565 )
#define DATA_SIZE_PER_PIC      (2u)
#else
#define DATA_SIZE_PER_PIC      (4u)
#endif

/*! Frame buffer stride: Frame buffer stride should be set to a multiple of 32 or 128
    in accordance with the frame buffer burst transfer mode. */
#define PIXEL_HW               (320u)  /* QVGA */
#define PIXEL_VW               (240u)  /* QVGA */

#define VIDEO_BUFFER_STRIDE    (((PIXEL_HW * DATA_SIZE_PER_PIC) + 31u) & ~31u)
#define VIDEO_BUFFER_HEIGHT    (PIXEL_VW)

EthernetInterface network;
RomRamFileSystem romramfs("romram");

#if defined(__ICCARM__)
#pragma data_alignment=16
static uint8_t FrameBuffer_Video[VIDEO_BUFFER_STRIDE * VIDEO_BUFFER_HEIGHT]@ ".mirrorram";  //16 bytes aligned!;
#pragma data_alignment=4
#else
static uint8_t FrameBuffer_Video[VIDEO_BUFFER_STRIDE * VIDEO_BUFFER_HEIGHT]__attribute((section("NC_BSS"),aligned(16)));  //16 bytes aligned!;
#endif
static volatile int32_t vsync_count = 0;
static volatile int32_t vfield_count = 1;

#if defined(__ICCARM__)
#pragma data_alignment=8
static uint8_t JpegBuffer[2][1024 * 50]@ ".mirrorram";  //8 bytes aligned!;
#pragma data_alignment=4
#else
static uint8_t JpegBuffer[2][1024 * 50]__attribute((section("NC_BSS"),aligned(8)));  //8 bytes aligned!;
#endif
static size_t jcu_encode_size[2];
static int image_change = 0;
JPEG_Converter Jcu;
static int jcu_buf_index_write = 0;
static int jcu_buf_index_write_done = 0;
static int jcu_buf_index_read = 0;
static int jcu_encoding = 0;
static char i2c_setting_str_buf[I2C_SETTING_STR_BUF_SIZE];

static void JcuEncodeCallBackFunc(JPEG_Converter::jpeg_conv_error_t err_code) {
    jcu_buf_index_write_done = jcu_buf_index_write;
    image_change = 1;
    jcu_encoding = 0;
}

static void IntCallbackFunc_Vfield(DisplayBase::int_type_t int_type) {
    //Interrupt callback function
    if (vfield_count != 0) {
        vfield_count = 0;
    } else {
        vfield_count = 1;

        JPEG_Converter::bitmap_buff_info_t bitmap_buff_info;
        JPEG_Converter::encode_options_t   encode_options;

        bitmap_buff_info.width          = PIXEL_HW;
        bitmap_buff_info.height         = PIXEL_VW;
        bitmap_buff_info.format         = JPEG_Converter::WR_RD_YCbCr422;
        bitmap_buff_info.buffer_address = (void *)FrameBuffer_Video;

        encode_options.encode_buff_size = sizeof(JpegBuffer[0]);
        encode_options.p_EncodeCallBackFunc = &JcuEncodeCallBackFunc;

        jcu_encoding = 1;
        if (jcu_buf_index_read == jcu_buf_index_write) {
            if (jcu_buf_index_write != 0) {
                jcu_buf_index_write = 0;
            } else {
                jcu_buf_index_write = 1;
            }
        }
        jcu_encode_size[jcu_buf_index_write] = 0;
        if (Jcu.encode(&bitmap_buff_info, JpegBuffer[jcu_buf_index_write], &jcu_encode_size[jcu_buf_index_write], &encode_options) != JPEG_Converter::JPEG_CONV_OK) {
            jcu_encode_size[jcu_buf_index_write] = 0;
            jcu_encoding = 0;
        }
    }
}

static void IntCallbackFunc_Vsync(DisplayBase::int_type_t int_type) {
    //Interrupt callback function for Vsync interruption
    if (vsync_count > 0) {
        vsync_count--;
    }
}

#if 1 /* WaitVsync(const int32_t wait_count) */
static void WaitVsync(const int32_t wait_count) {
    //Wait for the specified number of times Vsync occurs
    vsync_count = wait_count;
    while (vsync_count > 0) {
        /* Do nothing */
    }
}
#endif /* WaitVsync */

#if 1 /* camera_start(void) */
static void camera_start(void) {
    DisplayBase::graphics_error_t error;

#if VIDEO_INPUT_METHOD == VIDEO_CMOS_CAMERA
    DisplayBase::video_ext_in_config_t ext_in_config;
    PinName cmos_camera_pin[11] = {
        /* data pin */
        P2_7, P2_6, P2_5, P2_4, P2_3, P2_2, P2_1, P2_0,
        /* control pin */
        P10_0,      /* DV0_CLK   */
        P1_0,       /* DV0_Vsync */
        P1_1        /* DV0_Hsync */
    };
#endif 

    /* Create DisplayBase object */
    DisplayBase Display;

    /* Graphics initialization process */
    error = Display.Graphics_init(NULL);
    if (error != DisplayBase::GRAPHICS_OK) {
        printf("Line %d, error %d\n", __LINE__, error);
        while (1);
    }

#if VIDEO_INPUT_METHOD == VIDEO_CVBS
    error = Display.Graphics_Video_init( DisplayBase::INPUT_SEL_VDEC, NULL);
    if( error != DisplayBase::GRAPHICS_OK ) {
        printf("Line %d, error %d\n", __LINE__, error);
        while(1);
    }

#elif VIDEO_INPUT_METHOD == VIDEO_CMOS_CAMERA
    /* MT9V111 camera input config */
    ext_in_config.inp_format     = DisplayBase::VIDEO_EXTIN_FORMAT_BT601; /* BT601 8bit YCbCr format */
    ext_in_config.inp_pxd_edge   = DisplayBase::EDGE_RISING;              /* Clock edge select for capturing data          */
    ext_in_config.inp_vs_edge    = DisplayBase::EDGE_RISING;              /* Clock edge select for capturing Vsync signals */
    ext_in_config.inp_hs_edge    = DisplayBase::EDGE_RISING;              /* Clock edge select for capturing Hsync signals */
    ext_in_config.inp_endian_on  = DisplayBase::OFF;                      /* External input bit endian change on/off       */
    ext_in_config.inp_swap_on    = DisplayBase::OFF;                      /* External input B/R signal swap on/off         */
    ext_in_config.inp_vs_inv     = DisplayBase::SIG_POL_NOT_INVERTED;     /* External input DV_VSYNC inversion control     */
    ext_in_config.inp_hs_inv     = DisplayBase::SIG_POL_INVERTED;         /* External input DV_HSYNC inversion control     */
    ext_in_config.inp_f525_625   = DisplayBase::EXTIN_LINE_525;           /* Number of lines for BT.656 external input */
    ext_in_config.inp_h_pos      = DisplayBase::EXTIN_H_POS_CRYCBY;       /* Y/Cb/Y/Cr data string start timing to Hsync reference */
    ext_in_config.cap_vs_pos     = 6;                                     /* Capture start position from Vsync */
    ext_in_config.cap_hs_pos     = 150;                                   /* Capture start position form Hsync */
    ext_in_config.cap_width      = 640;                                   /* Capture width  */
    ext_in_config.cap_height     = 468u;                                  /* Capture height Max 468[line]
                                                                             Due to CMOS(MT9V111) output signal timing and VDC5 specification */
    error = Display.Graphics_Video_init( DisplayBase::INPUT_SEL_EXT, &ext_in_config);
    if( error != DisplayBase::GRAPHICS_OK ) {
        printf("Line %d, error %d\n", __LINE__, error);
        while(1);
    }

    /* MT9V111 camera input port setting */
    error = Display.Graphics_Dvinput_Port_Init(cmos_camera_pin, 11);
    if( error != DisplayBase::GRAPHICS_OK ) {
        printf("Line %d, error %d\n", __LINE__, error);
        while (1);
    }
#endif

    /* Interrupt callback function setting (Vsync signal input to scaler 0) */
    error = Display.Graphics_Irq_Handler_Set(DisplayBase::INT_TYPE_S0_VI_VSYNC, 0, IntCallbackFunc_Vsync);
    if (error != DisplayBase::GRAPHICS_OK) {
        printf("Line %d, error %d\n", __LINE__, error);
        while (1);
    }
    /* Video capture setting (progressive form fixed) */
    error = Display.Video_Write_Setting(
                VIDEO_INPUT_CH,
#if VIDEO_PAL == 0
                DisplayBase::COL_SYS_NTSC_358,
#else
                DisplayBase::COL_SYS_PAL_443,
#endif
                FrameBuffer_Video,
                VIDEO_BUFFER_STRIDE,
#if VIDEO_INPUT_FORMAT == VIDEO_YCBCR422
                DisplayBase::VIDEO_FORMAT_YCBCR422,
                DisplayBase::WR_RD_WRSWA_NON,
#elif VIDEO_INPUT_FORMAT == VIDEO_RGB565
                DisplayBase::VIDEO_FORMAT_RGB565,
                DisplayBase::WR_RD_WRSWA_32_16BIT,
#else
                DisplayBase::VIDEO_FORMAT_RGB888,
                DisplayBase::WR_RD_WRSWA_32BIT,
#endif
                PIXEL_VW,
                PIXEL_HW
            );
    if (error != DisplayBase::GRAPHICS_OK) {
        printf("Line %d, error %d\n", __LINE__, error);
        while (1);
    }

    /* Interrupt callback function setting (Field end signal for recording function in scaler 0) */
    error = Display.Graphics_Irq_Handler_Set(VIDEO_INT_TYPE, 0, IntCallbackFunc_Vfield);
    if (error != DisplayBase::GRAPHICS_OK) {
        printf("Line %d, error %d\n", __LINE__, error);
        while (1);
    }

    /* Video write process start */
    error = Display.Video_Start (VIDEO_INPUT_CH);
    if (error != DisplayBase::GRAPHICS_OK) {
        printf("Line %d, error %d\n", __LINE__, error);
        while (1);
    }

    /* Video write process stop */
    error = Display.Video_Stop (VIDEO_INPUT_CH);
    if (error != DisplayBase::GRAPHICS_OK) {
        printf("Line %d, error %d\n", __LINE__, error);
        while (1);
    }

    /* Video write process start */
    error = Display.Video_Start (VIDEO_INPUT_CH);
    if (error != DisplayBase::GRAPHICS_OK) {
        printf("Line %d, error %d\n", __LINE__, error);
        while (1);
    }

    /* Wait vsync to update resister */
    WaitVsync(1);
}
#endif /* camera_start(void) */

static int snapshot_req(const char ** pp_data) {
    int encode_size = 0;
#if 1
    while ((jcu_encoding == 1) || (image_change == 0)) {
        //Thread::wait(1);
    	dly_tsk(1);
    }
    jcu_buf_index_read = jcu_buf_index_write_done;
    image_change = 0;

    *pp_data = (const char *)JpegBuffer[jcu_buf_index_read];
    encode_size = (int)jcu_encode_size[jcu_buf_index_read];
#endif
    return encode_size;
}

#if 1 /* TerminalWrite(Arguments* arg, Reply* r) */
static void TerminalWrite(Arguments* arg, Reply* r) {
    printf("%s\n",arg->argv[0]);
}
#endif /* TerminalWrite(Arguments* arg, Reply* r) */

static void mount_romramfs(void) {
    FILE * fp;

    romramfs.format();

    //index.htm
    fp = fopen("/romram/index.htm", "w");
    fwrite(index_htm_tbl, sizeof(char), sizeof(index_htm_tbl), fp);
    fclose(fp);

    //camera.js
    fp = fopen("/romram/camera.js", "w");
    fwrite(camaera_js_tbl, sizeof(char), sizeof(camaera_js_tbl), fp);
    fclose(fp);

    //camera.htm
    fp = fopen("/romram/camera.htm", "w");
    fwrite(camera_htm_tbl, sizeof(char), sizeof(camera_htm_tbl), fp);
    fclose(fp);

    //mbedrpc.js
    fp = fopen("/romram/mbedrpc.js", "w");
    fwrite(mbedrpc_js_tbl, sizeof(char), sizeof(mbedrpc_js_tbl), fp);
    fclose(fp);

    //led.htm
    fp = fopen("/romram/led.htm", "w");
    fwrite(led_htm_tbl, sizeof(char), sizeof(led_htm_tbl), fp);
    fclose(fp);

    //i2c_set.htm
    fp = fopen("/romram/i2c_set.htm", "w");
    fwrite(i2c_set_htm_tbl, sizeof(char), sizeof(i2c_set_htm_tbl), fp);
    fclose(fp);

    //web_top.htm
    fp = fopen("/romram/web_top.htm", "w");
    fwrite(web_top_htm_tbl, sizeof(char), sizeof(web_top_htm_tbl), fp);
    fclose(fp);

    //menu.htm
    fp = fopen("/romram/menu.htm", "w");
    fwrite(menu_htm_tbl, sizeof(char), sizeof(menu_htm_tbl), fp);
    fclose(fp);

    //window.htm
    fp = fopen("/romram/window.htm", "w");
    fwrite(window_htm_tbl, sizeof(char), sizeof(window_htm_tbl), fp);
    fclose(fp);
}

static void SetI2CfromWeb(Arguments* arg, Reply* r) {
    int result = 0;

    if (arg != NULL) {
        if (arg->argc >= 2) {
            if ((arg->argv[0] != NULL) && (arg->argv[1] != NULL)) {
                sprintf(i2c_setting_str_buf, "%s,%s", arg->argv[0], arg->argv[1]);
                result = 1;
            }
        } else if (arg->argc == 1) {
            if (arg->argv[0] != NULL) {
                sprintf(i2c_setting_str_buf, "%s", arg->argv[0]);
                result = 1;
            }
        } else {
            /* Do nothing */
        }
        /* command analysis and execute */
        if (result != 0) {
            if (i2c_setting_exe(i2c_setting_str_buf) != false) {
                r->putData<const char*>(i2c_setting_str_buf);
            }
        }
    }
}

void
webcamera_main_task(intptr_t exinf) {
	/* syslogの設定 */
	SVC_PERROR(syslog_msk_log(LOG_UPTO(LOG_INFO), LOG_UPTO(LOG_EMERG)));	

	syslog(LOG_NOTICE, "Sample program starts (exinf = %d).", (int_t) exinf);
    printf("********* PROGRAM START ***********\r\n");
	
    /* Please enable this line when performing the setting from the
	   Terminal side. */
//    Thread thread(SetI2CfromTerm, NULL, osPriorityBelowNormal, DEFAULT_STACK_SIZE);

    mount_romramfs();   //RomRamFileSystem Mount
	syslog(LOG_NOTICE, "LOG_NOTICE: RomRamFile System Mounted.");	

	camera_start();     //Camera Start
	syslog(LOG_NOTICE, "LOG_NOTICE: Camera Started.");
	
    RPC::add_rpc_class<RpcDigitalOut>();
    RPC::construct<RpcDigitalOut, PinName, const char*>(LED1, "led1");
    RPC::construct<RpcDigitalOut, PinName, const char*>(LED2, "led2");
    RPC::construct<RpcDigitalOut, PinName, const char*>(LED3, "led3");
    RPCFunction rpcFunc(TerminalWrite, "TerminalWrite");
    RPCFunction rpcSetI2C(SetI2CfromWeb, "SetI2CfromWeb");

    printf("Network Setting up...\r\n");
	syslog(LOG_NOTICE, "LOG_NOTICE: Network Setting up...");
	
#if (USE_DHCP == 1)
    if (network.init() != 0) {                             //for DHCP Server
#else
    if (network.init(IP_ADDRESS, SUBNET_MASK, DEFAULT_GATEWAY) != 0) { //for Static IP Address (IPAddress, NetMasks, Gateway)
#endif
		//        printf("Network Initialize Error \r\n");
		syslog(LOG_NOTICE, "Network Initialize Error");
		//        return -1;
        return;		
    }
	syslog(LOG_NOTICE, "Network was initialized successfully");

	//if (network.connect(5000) != 0) {
	while (network.connect(5000) != 0) {
		//		printf("Network Connect Error \r\n");
		syslog(LOG_NOTICE, "LOG_NOTICE: Network Connect Error");
		//        return -1;
		//return;
    }
			
	//    syslog(LOG_NOTICE, "MAC Address is %s", network.getMACAddress());
	//    syslog(LOG_NOTICE, "IP Address is %s", network.getIPAddress());
	//    syslog(LOG_NOTICE, "NetMask is %s\r\n", network.getNetworkMask());
	//    syslog(LOG_NOTICE, "Gateway Address is %s", network.getGateway());
	
	printf("MAC Address is %s\r\n", network.getMACAddress());
	printf("IP Address is %s\r\n", network.getIPAddress());
	printf("NetMask is %s\r\n", network.getNetworkMask());
	printf("Gateway Address is %s\r\n", network.getGateway());
	
	//	syslog(LOG_INFO, "LOG_INFO: Network Setup OK");
	printf("Network Setup OK\r\n");
	
    SnapshotHandler::attach_req(&snapshot_req);
	HTTPServerAddHandler<SnapshotHandler>("/camera"); //Camera
	FSHandler::mount("/romram", "/");
	HTTPServerAddHandler<FSHandler>("/");
	HTTPServerAddHandler<RPCHandler>("/rpc");
	HTTPServerStart(80);	
}

// set mac address
void mbed_mac_address(char *mac) {
	// PEACH1
    mac[0] = 0x00;
    mac[1] = 0x02;
    mac[2] = 0xF7;
    mac[3] = 0xF0;
    mac[4] = 0x00;
    mac[5] = 0x00;
}

 /*
 *  周期ハンドラ
 *
 *  HIGH_PRIORITY，MID_PRIORITY，LOW_PRIORITY の各優先度のレディキュー
 *  を回転させる．
 */
bool_t led_state = true;
void cyclic_handler(intptr_t exinf)
{
	if (led_state == true) {
		led_state = false;
	} else {
		led_state = true;
	}
	set_led(BLUE_LED, led_state);
}
