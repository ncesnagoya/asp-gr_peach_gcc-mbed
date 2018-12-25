#include <kernel.h>
#include "kernel_cfg.h"
#include "camtank.h"

#include "mbed.h"
#include "GR_PEACH_Camera.h"

void task_timelapse(intptr_t exinf)
{
	int n;
	int size;
	const char *p_data;
	char filename[128];
	FILE * fp;

	printf("task_timelapse start\r\n");

	while(1)
	{
		slp_tsk();

		for(n = 0; n < 20; n++)
		{
			size = snapshot_req((const char**)&p_data);

			sprintf(filename, "/sdroot/DCIM/100PEACH/LAP%05d.jpg", n);
			fp = fopen(filename, "w");
			if(fp){
				fwrite(p_data, size, 1, fp);
				fclose(fp);
				printf("fwrite %s\r\n", filename);

				dly_tsk(1000);
			}
			else
			{
				printf("time_lapse fopen error\r\n");
				break;
			}
		}
	}
}
