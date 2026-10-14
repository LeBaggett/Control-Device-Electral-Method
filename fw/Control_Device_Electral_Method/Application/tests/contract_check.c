#include "app.h"
#include "board.h"
#include "device.h"
#include "encoder.h"
#include "job.h"
#include "oled.h"
#include "power.h"
#include "rtd.h"
#include "sdlog.h"
#include "ui.h"
#include "vf_adc.h"

int main(void)
{
    device_t device = {0};
    ui_cmd_t command = {0};
    input_event_t event = INPUT_EVENT_NONE;
    job_status_t status = JOB_STATUS_IDLE;

    (void)device;
    (void)command;
    (void)event;
    (void)status;
    return 0;
}
