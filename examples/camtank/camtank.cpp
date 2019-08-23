#include <kernel.h>
#include "kernel_cfg.h"
#include "camtank.h"

#include "mbed.h"
#include "HTTPServer.h"
#include "mbed_rpc.h"
#include "SDFileSystem.h"
#include "i2c_setting.h"

#include "GR_PEACH_Camera.h"
GR_PEACH_Camera camera;

/**** User Selection *********/

/** Network setting **/

#define USE_DHCP				(1)					/* Select  0(static configuration) or 1(use DHCP) */
#if (USE_DHCP == 0)
	#define IP_ADDRESS			("192.168.0.2")		/* IP address */
	#define SUBNET_MASK			("255.255.255.0")	/* Subnet mask */
	#define DEFAULT_GATEWAY		("192.168.0.3")		/* Default gateway */
#endif

#define NETWORK_TYPE			(0)					/* Select  0(EthernetInterface) or 1(GR_PEACH_WlanBP3595) */
#if (NETWORK_TYPE == 1)
//	#define WLAN_SSID			("SSIDofYourAP")	/* SSID */
//	#define WLAN_PSK			("PSKofYourAP")		/* PSK(Pre-Shared Key) */

	#define WLAN_SSID			("aterm-f6f5c2-g")	/* SSID */
	#define WLAN_PSK			("2647054ff1f45")	/* PSK(Pre-Shared Key) */
	#define WLAN_SECURITY		NSAPI_SECURITY_WPA2	/* NSAPI_SECURITY_NONE, NSAPI_SECURITY_WEP, NSAPI_SECURITY_WPA or NSAPI_SECURITY_WPA2 */

//	#define WLAN_SSID			("aterm-bbc67e-g")	/* SSID */
//	#define WLAN_PSK			("yutaka__chie")		/* PSK(Pre-Shared Key) */
//	#define WLAN_SECURITY		NSAPI_SECURITY_WPA	/* NSAPI_SECURITY_NONvE, NSAPI_SECURITY_WEP, NSAPI_SECURITY_WPA or NSAPI_SECURITY_WPA2 */

#endif

/** Camera setting **/

#if 0
#define VIDEO_INPUT_METHOD		(VIDEO_CMOS_CAMERA)	/* Select  VIDEO_CVBS or VIDEO_CMOS_CAMERA */
#else
#define VIDEO_INPUT_METHOD		(VIDEO_CVBS)		/* Select  VIDEO_CVBS or VIDEO_CMOS_CAMERA */
#endif

#define VIDEO_INPUT_FORMAT		(VIDEO_YCBCR422)	/* Select  VIDEO_YCBCR422 or VIDEO_RGB888 or VIDEO_RGB565 */
#define USE_VIDEO_CH			(0)					/* Select  0 or 1 If selecting VIDEO_CMOS_CAMERA, should be 0.) */
#define VIDEO_PAL				(1)					/* Select  0(NTSC) or 1(PAL) If selecting VIDEO_CVBS, this parameter is not referenced.) */

/** SD card spi channel setting **/

#define SD_SPICH			(2)
/*****************************/

#if (NETWORK_TYPE == 0)
  #include "EthernetInterface.h"
  EthernetInterface network;
#elif (NETWORK_TYPE == 1)
  #include "GR_PEACH_WlanBP3595.h"
  GR_PEACH_WlanBP3595 network;
  DigitalOut usb1en(P3_8);
#else
  #error NETWORK_TYPE error
#endif /* NETWORK_TYPE */
#if (SD_SPICH == 2)
SDFileSystem sdfs(P8_5, P8_6, P8_3, P8_4, "sdroot"); // mosi miso sclk cs name
#elif (SD_SPICH == 1)
SDFileSystem sdfs(P4_6, P4_7, P4_4, P4_5, "sdroot"); // mosi miso sclk cs name
#endif

static char i2c_setting_str_buf[I2C_SETTING_STR_BUF_SIZE];

static void TerminalWrite(Arguments* arg, Reply* r) {
	if ((arg != NULL) && (r != NULL)) {
		for (int i = 0; i < arg->argc; i++) {
			if (arg->argv[i] != NULL) {
				printf("%s", arg->argv[i]);
			}
		}
		printf("\n");
		r->putData<const char*>("ok");
	}
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

DigitalOut dir_left(D8);
DigitalOut dir_right(D7);
PwmOut pwm_left(P5_0);		//TIOC0
PwmOut pwm_right(P8_14);	//TIOC2
PwmOut pwm_pan(P5_3);		//TIOC3
PwmOut pwm_tilt(P3_8);		//TIOC4
static double steer = 0, speed = 0;

void TankSpeed(Arguments* arg, Reply* r) {
	if((arg != NULL)
	&& (arg->argc == 1)) {

		sscanf(arg->argv[0], "%lf", &speed);

		if(speed < 0) {
			speed *= -1;
			dir_left = 1;
			dir_right = 1;
		}else{
			dir_left = 0;
			dir_right = 0;
		}
		pwm_left.write(speed * (1 + steer));
		pwm_right.write(speed * (1 - steer));
	}
	return;
}

void TankSteer(Arguments* arg, Reply* r) {
	if((arg != NULL)
	&& (arg->argc == 1)) {
		sscanf(arg->argv[0], "%lf", &steer);

		pwm_left.write(speed * (1 + steer));
		pwm_right.write(speed * (1 - steer));
	}
	return;
}

void CamPan(Arguments* arg, Reply* r) {
	if((arg != NULL)
	&& (arg->argc == 1)) {
		double val;

		sscanf(arg->argv[0], "%lf", &val);
		pwm_pan.write(0.08 - val * 0.04);
	}
	return;
}

void CamTilt(Arguments* arg, Reply* r) {
	if((arg != NULL)
	&& (arg->argc == 1)) {
		double val;

		sscanf(arg->argv[0], "%lf", &val);
		pwm_tilt.write(0.08 + val * 0.04);
	}
	return;
}

void SaveJpeg(Arguments* arg, Reply* r) {
	static int count = 0;
	int size;
	const char *p_data;
	char filename[128];
	FILE * fp;

	sprintf(filename, "/sdroot/DCIM/100PEACH/CAM%05d.jpg", count);

	size = snapshot_req(&p_data);
	fp = fopen(filename, "w");
	if(fp){
		fwrite(p_data, size, 1, fp);
		fclose(fp);
		printf("save_jpeg[%s}\r\n", filename);
	}
	else
	{
		printf("save_jpeg fopen error\r\n");
	}
	count++;
	return;
}

void TimeLapse(Arguments* arg, Reply* r) {
	wup_tsk(TASKID_TIMELAPSE);
	return;
}

void task_main(intptr_t exinf) {

	DigitalOut led_yellow(D13);
	led_yellow = 1;

	dly_tsk(200);
	syslog(LOG_NOTICE, "********* PROGRAM START ***********");
	//	printf("********* PROGRAM START ***********\r\n");

	pwm_left.period_us(50);
	pwm_left.write(0.0f);
	pwm_right.period_us(50);
	pwm_right.write(0.0f);

	pwm_pan.period_ms(20);
	pwm_pan.write(0.075f);
	dly_tsk(200);
	pwm_tilt.period_ms(20);
	pwm_tilt.write(0.075f);
	dly_tsk(200);

	/* Please enable this line when performing the setting from the Terminal side. */
//	Thread thread(SetI2CfromTerm, NULL, osPriorityBelowNormal, DEFAULT_STACK_SIZE);

	camera.start();	//Camera Start

	RPC::add_rpc_class<RpcDigitalOut>();
	RPC::construct<RpcDigitalOut, PinName, const char*>(LED1, "led1");
	RPC::construct<RpcDigitalOut, PinName, const char*>(LED2, "led2");
	RPC::construct<RpcDigitalOut, PinName, const char*>(LED3, "led3");
	RPCFunction rpcFunc(TerminalWrite, "TerminalWrite");
	RPCFunction rpcSetI2C(SetI2CfromWeb, "SetI2CfromWeb");

	RPCFunction rpcTankSteer(TankSteer, "TankSteer");
	RPCFunction rpcTankSpeed(TankSpeed, "TankSpeed");
	RPCFunction rpcCamPan(CamPan, "CamPan");
	RPCFunction rpcCamTilt(CamTilt, "CamTilt");
	RPCFunction rpcSaveJpeg(SaveJpeg, "SaveJpeg");
	RPCFunction rpcTimeLapse(TimeLapse, "TimeLapse");

#if (NETWORK_TYPE == 1)
	//Audio Camera Shield USB1 enable for WlanBP3595
	usb1en = 1;		//Outputs high level
	dly_tsk(5);
	usb1en = 0;		//Outputs low level
	dly_tsk(5);
#endif

	syslog(LOG_NOTICE, "Network Setting up...");
	//	printf("Network Setting up...\r\n");
#if (USE_DHCP == 1)
	while (network.init() != 0) {							//for DHCP Server
#else
	while (network.init(IP_ADDRESS, SUBNET_MASK, DEFAULT_GATEWAY) != 0) { //for Static IP Address (IPAddress, NetMasks, Gateway)
#endif
		syslog(LOG_NOTICE, "Network Initialize Error");
		//		printf("Network Initialize Error \r\n");
		dly_tsk(1000);		
	}

	dly_tsk(1000);
	
#if (NETWORK_TYPE == 0)
	while (network.connect(5000) != 0) {
#else
	while (network.connect(WLAN_SSID, WLAN_PSK, WLAN_SECURITY) != 0) {
#endif
		syslog(LOG_NOTICE, "Network Connect Error");
		//		printf("Network Connect Error \r\n");
	}

	dly_tsk(1000);

    syslog(LOG_NOTICE, "MAC Address is %s", network.getMACAddress());
    syslog(LOG_NOTICE, "IP Address is %s", network.getIPAddress());
    syslog(LOG_NOTICE, "NetMask is %s", network.getNetworkMask());
    syslog(LOG_NOTICE, "Gateway Address is %s", network.getGateway());
    syslog(LOG_NOTICE, "Network Setup OK...");
	
	//	printf("MAC Address is %s\r\n", network.getMACAddress());
	//	printf("IP Address is %s\r\n", network.getIPAddress());
	//	printf("NetMask is %s\r\n", network.getNetworkMask());
//	printf("Gateway Address is %s\r\n", network.getGateway());
//	printf("Network Setup OK\r\n");

	led_yellow = 0;

	SnapshotHandler::attach_req(&snapshot_req);
	HTTPServerAddHandler<SnapshotHandler>("/camera"); //Camera
	FSHandler::mount("/sdroot", "/");

	mkdir("/sdroot/DCIM", 0777);
	mkdir("/sdroot/DCIM/100PEACH", 0777);

	HTTPServerAddHandler<FSHandler>("/");
	HTTPServerAddHandler<RPCHandler>("/rpc");
	HTTPServerStart(80);
}
