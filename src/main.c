

#include <stdio.h>
#include <stdlib.h>
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/dac.h"
#include "esp_log.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "mqtt_client.h"

#include "app_temp.h"
#include "app_wifi.h"

//static const adc2_channel_t adc_channel = ADC_CHANNEL_0;  //select ADC2 Channel 0 to convert
static const adc1_channel_t adc_channel = ADC_CHANNEL_0;    //Select Channel 0 of ADC1 for the conversion
esp_timer_handle_t timer_handler;

int adc_val=0,LEDON;           // define a variable for the ADC reading
char sound_array[160000];
unsigned int i=0,j,start; 

static void IRAM_ATTR replay(void* args){
    
start = i;
            gpio_set_level(4, 1);
            gpio_set_level(2, 0);   
           for(j=start;j<160000;j++){
                dac_output_voltage(DAC_CHAN_0, sound_array[j]);
                for(i=0;i<662;i++);
            }
            for(j=0;j<start;j++){          
                dac_output_voltage(DAC_CHAN_0, sound_array[j]);
                for(i=0;i<662;i++);
                
            }


            dac_output_voltage(DAC_CHAN_0, 0);
            i=0; 
}
static void init_hw(void)
{

  //  adc2_config_channel_atten(adc_channel, ADC_ATTEN_DB_11);    // 3dB attenuation
    adc1_config_width(ADC_WIDTH_BIT_12);          // Select 12-bit resolution for ADC (i.e. 0 to 4095)

    dac_output_enable(DAC_CHAN_0); // define the audio output pin (Pin 25)
}

void app_main()
{
    init_hw();          // initialize all needed hardware
    esp_rom_gpio_pad_select_gpio(13);
    gpio_set_direction(2, GPIO_MODE_OUTPUT);
    gpio_set_direction(4, GPIO_MODE_OUTPUT);

    gpio_set_direction(13, GPIO_MODE_INPUT);
    gpio_pulldown_en(13);
    gpio_pullup_dis(13);
    gpio_set_intr_type(13, GPIO_INTR_POSEDGE);
 gpio_install_isr_service(0);
    gpio_isr_handler_add(13, replay, NULL);
    gpio_set_level(4, 0);   
    gpio_set_level(2, 1);  
    while (1)
    {
       
        //adc2_get_raw(adc_channel,12,&adc_val);   // Read ADC2 value with 12-bit resolution to match the 12-bit PWM resolution
        adc_val =  adc1_get_raw(adc_channel);
        sound_array[i++] =  adc_val/16;
        if(i==160000)
            i=0;
        for(j=0;j<200;j++);
        
    }

}