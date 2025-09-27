/**
 * ______________________________________________________________________________________________________
 * @author		:		HITESH BHOYAR
 * @file    	:		icm45686.c
 * @brief   	:		This file includes the LL driver functions for ICM45686 IMU
 * ______________________________________________________________________________________________________
 */

#include "icm45685_spiV2.h"

#include <string.h>
#include "esp_log.h"

spi_device_handle_t icm45686_spi;
//Struct_ICM45686 ICM45686;

void ICM45686_SPI_GPIO_Initialization(void) {
    esp_err_t ret;

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_NUM_CS),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    gpio_set_level(PIN_NUM_CS, 1);



        // Configurar el GPIO para interrupciones
 
        io_conf.intr_type = GPIO_INTR_POSEDGE; // o GPIO_INTR_NEGEDGE según la polaridad del INT1
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pin_bit_mask = (1ULL << PIN_NUM_INT);
        io_conf.pull_down_en = 0;
        io_conf.pull_up_en = 1; // si el pin requiere pull-up
    
    gpio_config(&io_conf);
    
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 64
    };
    ret = spi_bus_initialize(ICM45686_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 2 * 1000 * 1000,
        .mode = 0,
        .spics_io_num = -1,
        .queue_size = 3
    };
    ret = spi_bus_add_device(ICM45686_SPI_HOST, &devcfg, &icm45686_spi);
    ESP_ERROR_CHECK(ret);
}

static uint8_t ICM45686_Transmit_Receive(uint8_t data) {
    uint8_t rx;
    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &data,
        .rx_buffer = &rx
    };
    ICM45686_CHIP_SELECT();
    spi_device_transmit(icm45686_spi, &t);
    ICM45686_CHIP_DESELECT();
    return rx;
}

uint8_t ICM45686_Readbyte(uint8_t reg_addr) {
    uint8_t tx = reg_addr | 0x80;
    uint8_t rx;
    ICM45686_CHIP_SELECT();
    spi_transaction_t t[2] = {0};
    t[0].length = 8;
    t[0].tx_buffer = &tx;
    spi_device_transmit(icm45686_spi, &t[0]);

    t[1].length = 8;
    t[1].rx_buffer = &rx;
    uint8_t dummy = 0x00;
    t[1].tx_buffer = &dummy;
    spi_device_transmit(icm45686_spi, &t[1]);
    ICM45686_CHIP_DESELECT();
    return rx;
}

void ICM45686_Read_Buffer(uint8_t reg_addr, uint8_t len, uint8_t* data) {
    ICM45686_CHIP_SELECT();
    uint8_t tx = reg_addr | 0x80;
    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &tx
    };
    spi_device_transmit(icm45686_spi, &t);

    memset(&t, 0, sizeof(t));
    t.length = len * 8;
    t.rx_buffer = data;
    uint8_t dummy[len];
    memset(dummy, 0x00, len);
    t.tx_buffer = dummy;
    spi_device_transmit(icm45686_spi, &t);
    ICM45686_CHIP_DESELECT();
}

void ICM45686_Writebyte(uint8_t reg_addr, uint8_t val) {
    uint8_t data[2] = {reg_addr & 0x7F, val};
    spi_transaction_t t = {
        .length = 2 * 8,
        .tx_buffer = data
    };
    ICM45686_CHIP_SELECT();
    spi_device_transmit(icm45686_spi, &t);
    ICM45686_CHIP_DESELECT();
}

void ICM45686_Write_Buffer(uint8_t reg_addr, uint8_t len, uint8_t* data) {
    ICM45686_CHIP_SELECT();
    uint8_t header = reg_addr & 0x7F;
    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &header
    };
    spi_device_transmit(icm45686_spi, &t);

    t.length = len * 8;
    t.tx_buffer = data;
    spi_device_transmit(icm45686_spi, &t);
    ICM45686_CHIP_DESELECT();
}
int ICM45686_Initialization(void) {
    ICM45686_SPI_GPIO_Initialization();
    vTaskDelay(pdMS_TO_TICKS(10));

    uint8_t who = ICM45686_Readbyte(ICM45686_REG_WHO_AM_I);
    if (who != ICM45686_ID) {
        ESP_LOGE("ICM", "WHO_AM_I mismatch: got 0x%02X, expected 0x%02X", who, ICM45686_ID);
        return 1;
    }

    // Interfaz SPI: MSB-first, sin delay, sin pad drive extra
    ICM45686_Writebyte(ICM45686_REG_INTF_CONFIG0, 0x00);
    vTaskDelay(pdMS_TO_TICKS(1));

    // Activar giroscopio y acelerómetro en modo LOW_NOISE
    ICM45686_Writebyte(ICM45686_REG_PWR_MGMT0, ICM45686_GYRO_LOW_NOISE | ICM45686_ACCEL_LOW_NOISE);
    vTaskDelay(pdMS_TO_TICKS(1));

   // ACCEL: STATIC5 (0x28)
    ICM45686_Writebyte(ICM45686_REG_IREG_ADDR_15_8, 0x00);
    ICM45686_Writebyte(ICM45686_REG_IREG_ADDR_7_0, 0x28);
    ICM45686_Writebyte(ICM45686_REG_IREG_DATA, 0x00);  // Bypass

    // GYRO: STATIC5 (0x21)
    ICM45686_Writebyte(ICM45686_REG_IREG_ADDR_15_8, 0x00);
    ICM45686_Writebyte(ICM45686_REG_IREG_ADDR_7_0, 0x21);
    ICM45686_Writebyte(ICM45686_REG_IREG_DATA, 0x00);  // Bypass

    // GYRO: ±2000DPS, ODR = 6.4kHz (0x03), BW = máximo (bits[7:4] = 0x0)
    uint8_t gyro_config = ICM45686_GYRO_4000_DPS | ICM45686_GYRO_ODR_6_4KHz ;
    ICM45686_Writebyte(ICM45686_REG_GYRO_CONFIG0, gyro_config);
    vTaskDelay(pdMS_TO_TICKS(1));

    // ACCEL: ±16G, ODR = 6.4kHz (0x03), BW = máximo (bits[7:4] = 0x0)
    uint8_t accel_config = ICM45686_ACCEL_16G | ICM45686_ACCEL_ODR_6_4KHz ;
    ICM45686_Writebyte(ICM45686_REG_ACCEL_CONFIG0, accel_config);
    vTaskDelay(pdMS_TO_TICKS(1));



 
    ESP_LOGI("ICM", "Initialization completed at 6.4kHz ODR");

    return 0;
}

/*
int ICM45686_Initialization(void) {
    ICM45686_SPI_GPIO_Initialization();
    vTaskDelay(pdMS_TO_TICKS(10));

    uint8_t who = ICM45686_Readbyte(0x72);
    if (who != ICM45686_ID) {
        ESP_LOGE("ICM", "WHO_AM_I mismatch: got 0x%02X, expected 0x%02X", who, ICM45686_ID);
        return 1;
    }

    ICM45686_Writebyte(ICM45686_REG_INTF_CONFIG0, 0x00);  // INTFCFG0: MSB-first
    vTaskDelay(pdMS_TO_TICKS(1));

    // Activar acelerómetro y giroscopio en modo LOW_NOISE
    ICM45686_Writebyte(ICM45686_REG_PWR_MGMT0, ICM45686_GYRO_LOW_NOISE | ICM45686_ACCEL_LOW_NOISE);
    vTaskDelay(pdMS_TO_TICKS(1));

    // Configurar giroscopio: ±2000DPS, ODR = 1.6kHz, BW máximo (bits [7:4] = 0x0)
    ICM45686_Writebyte(ICM45686_REG_GYRO_CONFIG0, (ICM45686_GYRO_2000_DPS | ICM45686_GYRO_ODR_1_6KHz | (0x00 << 4)));
    vTaskDelay(pdMS_TO_TICKS(1));

    // Configurar acelerómetro: ±16G, ODR = 1.6kHz, BW máximo (bits [7:4] = 0x0)
    ICM45686_Writebyte(ICM45686_REG_ACCEL_CONFIG0, (ICM45686_ACCEL_16G | ICM45686_ACCEL_ODR_1_6KHz | (0x00 << 4)));
    vTaskDelay(pdMS_TO_TICKS(1));

    // Puedes descomentar estas líneas si usas interrupciones DRDY
    //ICM45686_Writebyte(ICM45686_REG_INT1_CONFIG0, 0x04); // INT1_CONFIG0: DRDY push-pull
    //vTaskDelay(pdMS_TO_TICKS(1));
    //ICM45686_Writebyte(ICM45686_REG_INT1_CONFIG2, 0x02); // INT1_CONFIG2: enable DRDY
    //vTaskDelay(pdMS_TO_TICKS(1));

    return 0;
}
*/

void ICM45686_Get6AxisRawData(int16_t * accel, int16_t * gyro) {
    uint8_t data[12];
    ICM45686_Read_Buffer(0x00, 12, data);
accel[0] = (int16_t)((data[1] << 8) | data[0]); // <-- CAMBIO
accel[1] = (int16_t)((data[3] << 8) | data[2]);
accel[2] = (int16_t)((data[5] << 8) | data[4]);
gyro[0]  = (int16_t)((data[7] << 8) | data[6]);
gyro[1]  = (int16_t)((data[9] << 8) | data[8]);
gyro[2]  = (int16_t)((data[11] << 8) | data[10]);
}
/**
void ICM45686_Get3AxisGyroRawData(int16_t * gyro) {
    uint8_t data[6];
    ICM45686_Read_Buffer(0x06, 6, data);
    gyro[0] = (data[1] << 8) | data[1];
    gyro[1] = (data[3] << 8) | data[2];
    gyro[2] = (data[5] << 8) | data[4];
}

void ICM45686_Get3AxisAccRawData(int16_t * accel) {
    uint8_t data[6];
    ICM45686_Read_Buffer(0x00, 6, data);
    accel[0] = (data[0] << 8) | data[1];
    accel[1] = (data[3] << 8) | data[2];
    accel[2] = (data[5] << 8) | data[4];
}*/

void ICM45686_TempRawData(int16_t * temperature) {
    uint8_t data[2];
    ICM45686_Read_Buffer(0x0C, 2, data);
    *temperature = (data[0] << 8) | data[1];
}

void ICM45686_Calculate_Accel_Gyro_Temp(Struct_ICM45686* imu, float range, float dps) {

    int16_t accel_raw[3]; 
    int16_t gyro_raw[3];
    ICM45686_Get6AxisRawData(accel_raw, gyro_raw);
   // ICM45686_TempRawData(&imu->temperature_raw);

    imu->accel_x = accel_raw[0] * (range / 32768.0f);
    imu->accel_y = accel_raw[1] * (range / 32768.0f);
    imu->accel_z = accel_raw[2] * (range / 32768.0f);

    imu->gyro_x = gyro_raw[0] *  (dps / 32768.0f);
    imu->gyro_y = gyro_raw[1] *  (dps / 32768.0f);
    imu->gyro_z = gyro_raw[2] *  (dps / 32768.0f);

  //  imu->temperature = (imu->temperature_raw / 132.48f) + 25.f;
}

int ICM45686_DataReady(void) {
    return gpio_get_level(PIN_NUM_INT);
}

void ICM45686_Configure_Filter(uint8_t accel_bw, uint8_t gyro_bw)
{
    uint8_t reg;

    // Leer ACCEL_CONFIG0, conservar bits de FSR y ODR, actualizar bits [7:4]
    reg = ICM45686_Readbyte(ICM45686_REG_ACCEL_CONFIG0);
    reg &= 0x0F;  // Limpiar bits [7:4]
    reg |= (accel_bw << 4);
    ICM45686_Writebyte(ICM45686_REG_ACCEL_CONFIG0, reg);

    // Leer GYRO_CONFIG0, conservar bits de FSR y ODR, actualizar bits [7:4]
    reg = ICM45686_Readbyte(ICM45686_REG_GYRO_CONFIG0);
    reg &= 0x0F;  // Limpiar bits [7:4]
    reg |= (gyro_bw << 4);
    ICM45686_Writebyte(ICM45686_REG_GYRO_CONFIG0, reg);
}
/*#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "icm45685_spiV2.h"
#include "esp_log.h"
#include <math.h>
#include "driver/timer.h"
#include "esp_timer.h"
#define ICM45686_INT1_GPIO    GPIO_NUM_4

static const char *TAG = "ICM45686";

// Cola para manejar eventos de interrupción
static QueueHandle_t gpio_evt_queue = NULL;

// ISR del pin INT1
static void IRAM_ATTR icm45686_isr_handler(void* arg) {
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

// Tarea que maneja los eventos de DRDY
void icm45686_task(void* arg) {
    uint32_t io_num;
    short accel[3], gyro[3];
    float     state_ROLL=0,state_PITCH=0;
    uint64_t now_time=0,last_mpu_read_time=0;
    while (1) {

        now_time = esp_timer_get_time();//get_micro_timer_ticks_count();
            double time_delta_in_seconds = ((double)(now_time - last_mpu_read_time))* 0.000001f;
            last_mpu_read_time = now_time;
       // if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
           // if (io_num == ICM45686_INT1_GPIO) {

ICM45686_Calculate_Accel_Gyro_Temp(&ICM45686, 16.0f, 2000.0f);
ESP_LOGI("DEBUG", "Raw ax=%d ay=%d az=%d | gx=%d gy=%d gz=%d", 
         ICM45686.accel_x_raw, ICM45686.accel_y_raw, ICM45686.accel_z_raw,
         ICM45686.gyro_x_raw, ICM45686.gyro_y_raw, ICM45686.gyro_z_raw);
ESP_LOGI("DEBUG", "Scaled Ax=%.3f Ay=%.3f Az=%.3f | Gx=%.3f Gy=%.3f Gz=%.3f",
         ICM45686.accel_x, ICM45686.accel_y, ICM45686.accel_z,
         ICM45686.gyro_x, ICM45686.gyro_y, ICM45686.gyro_z);

   
                         
            double auxacc = atan2f(ICM45686.accel_y, sqrt(ICM45686.accel_x * ICM45686.accel_x +ICM45686.accel_z * ICM45686.accel_z)) * 180 / M_PI;
       
            state_ROLL
            = (state_ROLL  + ICM45686.gyro_x * time_delta_in_seconds) * 0.98
            + (auxacc ) * 0.02;


                    double auxaccy= atan2f(-ICM45686.accel_x, sqrt(ICM45686.accel_y * ICM45686.accel_y + ICM45686.accel_z* ICM45686.accel_z)) * 180 / M_PI;


            state_PITCH
            = (state_PITCH + ICM45686.gyro_y * time_delta_in_seconds) * 0.98
            + (auxaccy) * 0.02;

            printf("\n state_ROLL: %f ", state_ROLL);
            printf("\n state_PITCH: %f ",state_PITCH);
              //  ESP_LOGI(TAG, "Accel: X=%d Y=%d Z=%d | Gyro: X=%d Y=%d Z=%d",
                //         accel[0], accel[1], accel[2], gyro[0], gyro[1], gyro[2]);
           // }
      //  }
    }
}
void app_main(void) {
    // Inicializar el sensor (asegúrate que SPI y CS estén bien configurados)
    ICM45686_Initialization();


    // Crear cola
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    // Instalar servicio de ISR
    gpio_install_isr_service(0);
    gpio_isr_handler_add(ICM45686_INT1_GPIO, icm45686_isr_handler, (void*) ICM45686_INT1_GPIO);

    // Crear la tarea que manejará los datos
    xTaskCreate(icm45686_task, "icm45686_task", 4096, NULL, 10, NULL);
}
*/