
#ifndef __ICM45686_H__
#define __ICM45686_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/spi_master.h"
#include "esp_err.h"
#include <string.h>
#include "driver/gpio.h"
#include "esp_log.h"
#define ICM45686_CHIP_SELECT()   gpio_set_level(PIN_NUM_CS, 0)
#define ICM45686_CHIP_DESELECT() gpio_set_level(PIN_NUM_CS, 1)
#define ICM45686_INT1_GPIO    GPIO_NUM_4
#define ICM45686_REG_ACCEL_DATA_X1_UI 		    UINT8_C(0x00)
#define ICM45686_REG_ACCEL_DATA_X0_UI 			UINT8_C(0x01)
#define ICM45686_REG_ACCEL_DATA_Y1_UI 			UINT8_C(0x02)
#define ICM45686_REG_ACCEL_DATA_Y0_UI 			UINT8_C(0x03)
#define ICM45686_REG_ACCEL_DATA_Z1_UI 			UINT8_C(0x04)
#define ICM45686_REG_ACCEL_DATA_Z0_UI 			UINT8_C(0x05)
#define ICM45686_REG_GYRO_DATA_X1_UI            UINT8_C(0x06)
#define ICM45686_REG_GYRO_DATA_X0_UI 		    UINT8_C(0x07)
#define ICM45686_REG_GYRO_DATA_Y1_UI 		    UINT8_C(0x08)
#define ICM45686_REG_GYRO_DATA_Y0_UI 		    UINT8_C(0x09)
#define ICM45686_REG_GYRO_DATA_Z1_UI 		    UINT8_C(0x0A)
#define ICM45686_REG_GYRO_DATA_Z0_UI 		    UINT8_C(0x0B)
#define ICM45686_REG_TEMP_DATA1_UI 			    UINT8_C(0x0C)
#define ICM45686_REG_TEMP_DATA0_UI 			    UINT8_C(0x0D)
#define ICM45686_REG_TMST_FSYNCH 			    UINT8_C(0x0E)
#define ICM45686_REG_TMST_FSYNCL 		    	UINT8_C(0x0F)
#define ICM45686_REG_PWR_MGMT0 			        UINT8_C(0x10)
#define ICM45686_REG_FIFO_COUNT_0 			    UINT8_C(0x12)
#define ICM45686_REG_FIFO_COUNT_1 		    	UINT8_C(0x13)
#define ICM45686_REG_FIFO_DATA 		        	UINT8_C(0x14)
#define ICM45686_REG_INT1_CONFIG0 	    		UINT8_C(0x16)
#define ICM45686_REG_INT1_CONFIG1 			    UINT8_C(0x17)
#define ICM45686_REG_INT1_CONFIG2 			    UINT8_C(0x18)
#define ICM45686_REG_INT1_STATUS0 			    UINT8_C(0x19)
#define ICM45686_REG_INT1_STATUS1 		    	UINT8_C(0x1A)
#define ICM45686_REG_ACCEL_CONFIG0 			    UINT8_C(0x1B)
#define ICM45686_REG_GYRO_CONFIG0 			    UINT8_C(0x1C)
#define ICM45686_REG_FIFO_CONFIG0 			    UINT8_C(0x1D)
#define ICM45686_REG_FIFO_CONFIG1_0 			UINT8_C(0x1E)
#define ICM45686_REG_FIFO_CONFIG1_1 			UINT8_C(0x1F)
#define ICM45686_REG_FIFO_CONFIG2 			    UINT8_C(0x20)
#define ICM45686_REG_FIFO_CONFIG3 			    UINT8_C(0x21)
#define ICM45686_REG_FIFO_CONFIG4	        	UINT8_C(0x22)
#define ICM45686_REG_TMST_WOM_CONFIG 			UINT8_C(0x23)
#define ICM45686_REG_FSYNC_CONFIG0 			    UINT8_C(0x24)
#define ICM45686_REG_FSYNC_CONFIG1 			    UINT8_C(0x25)
#define ICM45686_REG_RTC_CONFIG 			    UINT8_C(0x26)
#define ICM45686_REG_DMP_EXT_SEN_ODR_CFG 		UINT8_C(0x27)
#define ICM45686_REG_ODR_DECIMATE_CONFIG 		UINT8_C(0x28)
#define ICM45686_REG_EDMP_APEX_EN0 	            UINT8_C(0x29)
#define ICM45686_REG_EDMP_APEX_EN1 		        UINT8_C(0x2A)
#define ICM45686_REG_APEX_BUFFER_MGMT 			UINT8_C(0x2B)
#define ICM45686_REG_INTF_CONFIG0 			    UINT8_C(0x2C)
#define ICM45686_REG_INTF_CONFIG1_OVRD 			UINT8_C(0x2D)
#define ICM45686_REG_INTF_AUX_CONFIG 			UINT8_C(0x2E)
#define ICM45686_REG_IOC_PAD_SCENARIO 			UINT8_C(0x2F)
#define ICM45686_REG_IOC_PAD_SCENARIO_AUX_OVRD 	UINT8_C(0x30)
#define ICM45686_REG_DRIVE_CONFIG0 			    UINT8_C(0x32)
#define ICM45686_REG_DRIVE_CONFIG1 			    UINT8_C(0x33)
#define ICM45686_REG_DRIVE_CONFIG2 			    UINT8_C(0x34)
#define ICM45686_REG_INT_APEX_CONFIG0 			UINT8_C(0x39)
#define ICM45686_REG_INT_APEX_CONFIG1 			UINT8_C(0x3A)
#define ICM45686_REG_INT_APEX_STATUS0 			UINT8_C(0x3B)
#define ICM45686_REG_INT_APEX_STATUS1 			UINT8_C(0x3C)
#define ICM45686_REG_ACCEL_DATA_X1_AUX1 		UINT8_C(0x44)
#define ICM45686_REG_ACCEL_DATA_X0_AUX1 		UINT8_C(0x45)
#define ICM45686_REG_ACCEL_DATA_Y1_AUX1 		UINT8_C(0x46)
#define ICM45686_REG_ACCEL_DATA_Y0_AUX1 		UINT8_C(0x47)
#define ICM45686_REG_ACCEL_DATA_Z1_AUX1 		UINT8_C(0x48)
#define ICM45686_REG_ACCEL_DATA_Z0_AUX1 		UINT8_C(0x49)
#define ICM45686_REG_GYRO_DATA_X1_AUX1 			UINT8_C(0x4A)
#define ICM45686_REG_GYRO_DATA_X0_AUX1	 		UINT8_C(0x4B)
#define ICM45686_REG_GYRO_DATA_Y1_AUX1 			UINT8_C(0x4C)
#define ICM45686_REG_GYRO_DATA_Y0_AUX1 			UINT8_C(0x4D)
#define ICM45686_REG_GYRO_DATA_Z1_AUX1 			UINT8_C(0x4E)
#define ICM45686_REG_GYRO_DATA_Z0_AUX1 			UINT8_C(0x4F)
#define ICM45686_REG_TEMP_DATA1_AUX1 			UINT8_C(0x50)
#define ICM45686_REG_TEMP_DATA0_AUX1 			UINT8_C(0x51)
#define ICM45686_REG_TMST_FSYNCH_AUX1 			UINT8_C(0x52)
#define ICM45686_REG_TMST_FSYNCL_AUX1 			UINT8_C(0x53)
#define ICM45686_REG_PWR_MGMT_AUX1 			    UINT8_C(0x54)
#define ICM45686_REG_FS_SEL_AUX1 			    UINT8_C(0x55)
#define ICM45686_REG_INT2_CONFIG0 			    UINT8_C(0x56)
#define ICM45686_REG_INT2_CONFIG1 			    UINT8_C(0x57)
#define ICM45686_REG_INT2_CONFIG2 			    UINT8_C(0x58)
#define ICM45686_REG_INT2_STATUS0 			    UINT8_C(0x59)
#define ICM45686_REG_INT2_STATUS1 			    UINT8_C(0x5A)
#define ICM45686_REG_WHO_AM_I 			        UINT8_C(0x72)
#define ICM45686_REG_REG_HOST_MSG 			    UINT8_C(0x73)
#define ICM45686_REG_IREG_ADDR_15_8 			UINT8_C(0x7C)
#define ICM45686_REG_IREG_ADDR_7_0 			    UINT8_C(0x7D)
#define ICM45686_REG_IREG_DATA 			        UINT8_C(0x7E)
#define ICM45686_REG_REG_MISC2 			        UINT8_C(0x7F)
/* Register Map Ends ---------------------------------------------------------*/
#define ICM45686_BW_FILTER_MAX     (0x00 << 4)  // Filtro más rápido, menos atenuación de ruido
#define ICM45686_BW_FILTER_MID     (0x01 << 4)  // Filtro intermedio
#define ICM45686_BW_FILTER_MIN     (0x02 << 4)  // Filtro más lento, más atenuación de ruido

#define ICM45686_FILTER_BW_473HZ   0x00
#define ICM45686_FILTER_BW_236HZ   0x01
#define ICM45686_FILTER_BW_111HZ   0x02
#define ICM45686_FILTER_BW_54HZ    0x03
#define ICM45686_FILTER_BW_26HZ    0x04
#define ICM45686_FILTER_BW_12HZ    0x05
#define ICM45686_FILTER_BW_6HZ     0x06
#define ICM45686_FILTER_BW_3HZ     0x07


#define PIN_NUM_MISO  29//19
#define PIN_NUM_MOSI  28//23
#define PIN_NUM_CLK   30//18
#define PIN_NUM_CS    31//5
#define PIN_NUM_INT   21//26 4

#define ICM45686_SPI_HOST           SPI2_HOST
/* Define Accelerometer Macros -----------------------------------------------*/

#define ICM45686_ID 			        	UINT8_C(0xE9)
#define ICM45686_REG_ACCEL_CONFIG1       0x21 
#define ICM45686_REG_GYRO_CONFIG1   0x20

// 53 Hz
#define ICM45686_GYRO_FILTER_53HZ   0x40  


#define ICM45686_ACCEL_POWER_OFF			UINT8_C(0x00)
#define ICM45686_ACCEL_STANDBY				UINT8_C(0x01)
#define ICM45686_ACCEL_LOW_POWER 			UINT8_C(0x02)
#define ICM45686_ACCEL_LOW_NOISE			UINT8_C(0x03)

#define ICM45686_ACCEL_32G 					UINT8_C(0x00)
#define ICM45686_ACCEL_16G 					UINT8_C(0x10)
#define ICM45686_ACCEL_8G 					UINT8_C(0x20)
#define ICM45686_ACCEL_4G 					UINT8_C(0x30)
#define ICM45686_ACCEL_2G 					UINT8_C(0x40)

#define ICM45686_ACCEL_ODR_6_4KHz			UINT8_C(0x03)
#define ICM45686_ACCEL_ODR_3_2KHz			UINT8_C(0x04)
#define ICM45686_ACCEL_ODR_1_6KHz			UINT8_C(0x05)
#define ICM45686_ACCEL_ODR_800Hz			UINT8_C(0x06)
#define ICM45686_ACCEL_ODR_400Hz			UINT8_C(0x07)
#define ICM45686_ACCEL_ODR_200Hz			UINT8_C(0x08)
#define ICM45686_ACCEL_ODR_100Hz			UINT8_C(0x09)
#define ICM45686_ACCEL_ODR_50Hz				UINT8_C(0x0A)
#define ICM45686_ACCEL_ODR_25Hz				UINT8_C(0x0B)
#define ICM45686_ACCEL_ODR_12_5Hz			UINT8_C(0x0C)
#define ICM45686_ACCEL_ODR_6_25Hz			UINT8_C(0x0D)
#define ICM45686_ACCEL_ODR_3_125Hz			UINT8_C(0x0E)
#define ICM45686_ACCEL_ODR_1_5625Hz			UINT8_C(0x0F)

/* Define Gyroscope Macros ---------------------------------------------------*/
#define ICM45686_GYRO_POWER_OFF			    UINT8_C(0x00)
#define ICM45686_GYRO_STANDBY			    UINT8_C(0x04)
#define ICM45686_GYRO_LOW_POWER 			UINT8_C(0x08)
#define ICM45686_GYRO_LOW_NOISE 			UINT8_C(0x0C)

#define ICM45686_GYRO_4000_DPS 				UINT8_C(0x00)
#define ICM45686_GYRO_2000_DPS 				UINT8_C(0x10)
#define ICM45686_GYRO_1000_DPS 				UINT8_C(0x20)
#define ICM45686_GYRO_500_DPS 				UINT8_C(0x30)
#define ICM45686_GYRO_250_DPS 				UINT8_C(0x40)
#define ICM45686_GYRO_125_DPS 				UINT8_C(0x50)
#define ICM45686_GYRO_62_5_DPS 				UINT8_C(0x60)
#define ICM45686_GYRO_31_25_DPS 			UINT8_C(0x70)
#define ICM45686_GYRO_15_625_DPS 			UINT8_C(0x80)

#define ICM45686_GYRO_ODR_6_4KHz 			UINT8_C(0x03)
#define ICM45686_GYRO_ODR_3_2KHz			UINT8_C(0x04)
#define ICM45686_GYRO_ODR_1_6KHz			UINT8_C(0x05)
#define ICM45686_GYRO_ODR_800Hz				UINT8_C(0x06)
#define ICM45686_GYRO_ODR_400Hz				UINT8_C(0x07)
#define ICM45686_GYRO_ODR_200Hz 			UINT8_C(0x08)
#define ICM45686_GYRO_ODR_100Hz 			UINT8_C(0x09)
#define ICM45686_GYRO_ODR_50Hz 				UINT8_C(0x0A)
#define ICM45686_GYRO_ODR_25Hz 				UINT8_C(0x0B)
#define ICM45686_GYRO_ODR_12_5Hz 			UINT8_C(0x0C)
#define ICM45686_GYRO_ODR_6_25Hz			UINT8_C(0x0D)
#define ICM45686_GYRO_ODR_3_125Hz			UINT8_C(0x0E)
#define ICM45686_GYRO_ODR_1_5625Hz			UINT8_C(0x0F)

#define ICM45686_FILTER_BW_473HZ   0x00
#define ICM45686_FILTER_BW_236HZ   0x01
#define ICM45686_FILTER_BW_111HZ   0x02
#define ICM45686_FILTER_BW_54HZ    0x03
#define ICM45686_FILTER_BW_26HZ    0x04
#define ICM45686_FILTER_BW_12HZ    0x05
#define ICM45686_FILTER_BW_6HZ     0x06
#define ICM45686_FILTER_BW_3HZ     0x07

// Registros de estado e interrupciones
#define ICM45686_REG_INT_STATUS      0x1D
#define ICM45686_REG_INT_STATUS1     0x1E
#define ICM45686_REG_INT_STATUS2     0x1F
#define ICM45686_REG_INT_STATUS3     0x20


/*
 * IPREG_SYS1_REG_172
 */

/* gyro_ui_lpfbw_sel */
typedef enum {
	IPREG_SYS1_REG_172_GYRO_UI_LPFBW_DIV_128   = 0x06,
	IPREG_SYS1_REG_172_GYRO_UI_LPFBW_DIV_64    = 0x05,
	IPREG_SYS1_REG_172_GYRO_UI_LPFBW_DIV_32    = 0x04,
	IPREG_SYS1_REG_172_GYRO_UI_LPFBW_DIV_16    = 0x03,
	IPREG_SYS1_REG_172_GYRO_UI_LPFBW_DIV_8     = 0x02,
	IPREG_SYS1_REG_172_GYRO_UI_LPFBW_DIV_4     = 0x01,
	IPREG_SYS1_REG_172_GYRO_UI_LPFBW_NO_FILTER = 0x00,
} ipreg_sys1_reg_172_gyro_ui_lpfbw_sel_t;

/*
 * IPREG_SYS2_REG_131
 */

/* accel_ui_lpfbw_sel */
typedef enum {
	IPREG_SYS2_REG_131_ACCEL_UI_LPFBW_DIV_128   = 0x06,
	IPREG_SYS2_REG_131_ACCEL_UI_LPFBW_DIV_64    = 0x05,
	IPREG_SYS2_REG_131_ACCEL_UI_LPFBW_DIV_32    = 0x04,
	IPREG_SYS2_REG_131_ACCEL_UI_LPFBW_DIV_16    = 0x03,
	IPREG_SYS2_REG_131_ACCEL_UI_LPFBW_DIV_8     = 0x02,
	IPREG_SYS2_REG_131_ACCEL_UI_LPFBW_DIV_4     = 0x01,
	IPREG_SYS2_REG_131_ACCEL_UI_LPFBW_NO_FILTER = 0x00,
} ipreg_sys2_reg_131_accel_ui_lpfbw_t;
#define IPREG_SYS1_REG_170                                                      0xa4aa
/* gyro_lp_avg_sel */
typedef enum {
	IPREG_SYS1_REG_170_GYRO_LP_AVG_64 = 0xC,
	IPREG_SYS1_REG_170_GYRO_LP_AVG_32 = 0xB,
	IPREG_SYS1_REG_170_GYRO_LP_AVG_20 = 0xA,
	IPREG_SYS1_REG_170_GYRO_LP_AVG_18 = 0x9,
	IPREG_SYS1_REG_170_GYRO_LP_AVG_16 = 0x8,
	IPREG_SYS1_REG_170_GYRO_LP_AVG_11 = 0x7,
	IPREG_SYS1_REG_170_GYRO_LP_AVG_10 = 0x6,
	IPREG_SYS1_REG_170_GYRO_LP_AVG_8  = 0x5,
	IPREG_SYS1_REG_170_GYRO_LP_AVG_7  = 0x4,
	IPREG_SYS1_REG_170_GYRO_LP_AVG_5  = 0x3,
	IPREG_SYS1_REG_170_GYRO_LP_AVG_4  = 0x2,
	IPREG_SYS1_REG_170_GYRO_LP_AVG_2  = 0x1,
	IPREG_SYS1_REG_170_GYRO_LP_AVG_1  = 0x0,
} ipreg_sys1_reg_170_gyro_lp_avg_sel_t;

#define IPREG_SYS2_REG_129                                                      0xa581
/* accel_lp_avg_sel */
typedef enum {
	IPREG_SYS2_REG_129_ACCEL_LP_AVG_64 = 0xC,
	IPREG_SYS2_REG_129_ACCEL_LP_AVG_32 = 0xB,
	IPREG_SYS2_REG_129_ACCEL_LP_AVG_20 = 0xA,
	IPREG_SYS2_REG_129_ACCEL_LP_AVG_18 = 0x9,
	IPREG_SYS2_REG_129_ACCEL_LP_AVG_16 = 0x8,
	IPREG_SYS2_REG_129_ACCEL_LP_AVG_11 = 0x7,
	IPREG_SYS2_REG_129_ACCEL_LP_AVG_10 = 0x6,
	IPREG_SYS2_REG_129_ACCEL_LP_AVG_8  = 0x5,
	IPREG_SYS2_REG_129_ACCEL_LP_AVG_7  = 0x4,
	IPREG_SYS2_REG_129_ACCEL_LP_AVG_5  = 0x3,
	IPREG_SYS2_REG_129_ACCEL_LP_AVG_4  = 0x2,
	IPREG_SYS2_REG_129_ACCEL_LP_AVG_2  = 0x1,
	IPREG_SYS2_REG_129_ACCEL_LP_AVG_1  = 0x0,
} ipreg_sys2_reg_129_accel_lp_avg_sel_t;

// Registros de configuración de interrupciones
#define ICM45686_REG_INT_CONFIG0     0x63
#define ICM45686_REG_INT_CONFIG1     0x64
#define ICM45686_REG_INT_SOURCE0     0x65

#define ACCE_32G     32.0f / 32768.0f // ±32G
#define ACCE_16G     16.0f / 32768.0f // ±16G
#define ACCE_8G      8.0f  / 32768.0f // ±8G
#define ACCE_4G      4.0f  / 32768.0f // ±4G
#define ACCE_2G      2.0f  / 32768.0f // ±2G

#define GYRO_4000DPS     4000.0f / 32768.0f // ±4000 dps
#define GYRO_2000DPS    (2000.0f / 32768.0f)
#define GYRO_1000DPS    1000.0f / 32768.0f
#define GYRO_500DPS    500.0f  / 32768.0f
#define GYRO_250DPS    250.0f  / 32768.0f
#define GYRO_125DPS    125.0f  / 32768.0f
#define GYRO_62_5DPS    62.5f   / 32768.0f
#define GYRO_31_25PS    31.25f  / 32768.0f
 #define GYRO_15_625DPS   15.625f / 32768.0f
/* Structure Definition ------------------------------------------------------*/
typedef struct {
    float ax, ay, az;
    float gx, gy, gz;
    float dt;
} SensorData;

typedef struct _ICM45686{
	int16_t  accel_x_raw;
	int16_t  accel_y_raw;
	int16_t  accel_z_raw;
	int16_t  gyro_x_raw;
	int16_t  gyro_y_raw;
	int16_t  gyro_z_raw;
	int16_t  temperature_raw;

	//	short timestamp;

	double accel_x;
	double accel_y;
	double accel_z;
	double gyro_x;
	double gyro_y;
	double gyro_z;
	float temperature;

	double roll;
	double pitch;
	double yaw;

}Struct_ICM45686;
typedef struct {
    int16_t accel_raw[3];  // accel_raw[0]=x, [1]=y, [2]=z
    int16_t gyro_raw[3];
    
} Struct_RAW_ICM45686;

//extern Struct_ICM45686 ICM45686;
/*
 * IPREG_SYS1_REG_170
 */


#define IPREG_SYS1_REG_171                                                      0xa4ab
typedef struct __attribute__((packed)){
	uint8_t resv_1                                                                 : 1;
	uint8_t gyro_lp_avg_sel                                                        : 4;
	uint8_t gyro_ois_hpfbw_sel                                                     : 3;
} ipreg_sys1_reg_170_t;


/* ---------------------------------------------------------------------------
 * Bank IPREG_SYS2
 * ---------------------------------------------------------------------------*/

/*
 * IPREG_SYS2_REG_123
 */
#define IPREG_SYS2_REG_123                                                      0xa57b
/* accel_src_ctrl */
typedef enum {
	IPREG_SYS2_REG_123_ACCEL_SRC_CTRL_INTERPOLATOR_ON_FIR_ON   = 0x2,
	IPREG_SYS2_REG_123_ACCEL_SRC_CTRL_INTERPOLATOR_OFF_FIR_ON  = 0x1,
	IPREG_SYS2_REG_123_ACCEL_SRC_CTRL_INTERPOLATOR_OFF_FIR_OFF = 0x0,
} ipreg_sys2_reg_123_accel_src_ctrl_sel_t;

/* ---------------------------------------------------------------------------
 * Bank IPREG_SYS1
 * ---------------------------------------------------------------------------*/

/*
 * IPREG_SYS1_REG_166
 */
#define IPREG_SYS1_REG_166                                                      0xa4a6
/* gyro_src_ctrl */
typedef enum {
	IPREG_SYS1_REG_166_GYRO_SRC_CTRL_INTERPOLATOR_ON_FIR_ON   = 0x2,
	IPREG_SYS1_REG_166_GYRO_SRC_CTRL_INTERPOLATOR_OFF_FIR_ON  = 0x1,
	IPREG_SYS1_REG_166_GYRO_SRC_CTRL_INTERPOLATOR_OFF_FIR_OFF = 0x0,
} ipreg_sys1_reg_166_gyro_src_ctrl_sel_t;



typedef struct  __attribute__((packed)){
	uint8_t accel_lp_avg_sel                                                       : 4;
	uint8_t accel_ois_hpfbw_sel                                                    : 3;
	uint8_t resv_1                                                                 : 1;
} ipreg_sys2_reg_129_t;
// ============================================================
// ICM-45686 — FIFO helpers
// ============================================================
#define FIFO_MAX_PKTS                       16
#define ICM45686_FIFO_PKT_SIZE_ACCEL_GYRO   16
#define ICM45686_FIFO_ACCEL_SCALE_16G       (16.0f  / 32768.0f)
#define ICM45686_FIFO_GYRO_SCALE_4000DPS    (4000.0f / 32768.0f)
// ============================================================
// ICM-45686 — Registros FIFO (Bank 0)
// Fuente: DS-000577 + AN-000478
// ============================================================

// FIFO_CONFIG0 — 0x1D
// bits [7:6]: FIFO_MODE
//   00 = bypass (FIFO desactivado)
//   01 = stream (sobrescribe cuando lleno)  ← usar este
//   10 = stop-on-full
#define ICM45686_REG_FIFO_CONFIG0       0x1D
#define ICM45686_FIFO_MODE_BYPASS       0x00
#define ICM45686_FIFO_MODE_STREAM       0x40   // bits[7:6] = 01
#define ICM45686_FIFO_MODE_STOP_FULL    0x80   // bits[7:6] = 10

// FIFO_CONFIG1_0 — 0x1E  (nota: el datasheet lo llama FIFO_CONFIG1_0, NO FIFO_CONFIG1)
// bits individuales de habilitación de canales al FIFO
#define ICM45686_REG_FIFO_CONFIG1_0     0x1E
#define ICM45686_FIFO_ACCEL_EN          (1 << 0)   // bit 0
#define ICM45686_FIFO_GYRO_EN           (1 << 1)   // bit 1
#define ICM45686_FIFO_HIRES_EN          (1 << 2)   // bit 2 — formato 20-bit
#define ICM45686_FIFO_TMST_FSYNC_EN     (1 << 3)   // bit 3 — timestamp
#define ICM45686_FIFO_TEMP_EN           (1 << 4)   // bit 4 — temperatura

// FIFO_CONFIG1_1 — 0x1F
// bits [7:0]: FIFO watermark MSB (parte alta del umbral)
#define ICM45686_REG_FIFO_WM_TH_H      0x1F   // watermark MSB

// FIFO_CONFIG2 — 0x20
// bits [7:0]: FIFO watermark LSB (parte baja del umbral)
#define ICM45686_REG_FIFO_WM_TH_L      0x1E   // ← OJO: es FIFO_CONFIG1_0
// CORRECCIÓN: el watermark LSB está en FIFO_CONFIG2
#define ICM45686_REG_FIFO_CONFIG2       0x20
#define ICM45686_FIFO_WR_WM_GT_TH      (1 << 3)   // bit 3 — interrupción al superar watermark

// FIFO_CONFIG3 — 0x21
// bit 0: FIFO_IF_EN — habilita la interfaz FIFO (obligatorio para que funcione)
// bit 4: FIFO_ES0_EN — habilita sensor externo 0
// bit 5: FIFO_ES1_EN — habilita sensor externo 1
#define ICM45686_REG_FIFO_CONFIG3       0x21
#define ICM45686_FIFO_IF_EN             (1 << 0)   // bit 0 — DEBE estar en 1
#define ICM45686_FIFO_ES0_EN            (1 << 4)   // bit 4
#define ICM45686_FIFO_ES1_EN            (1 << 5)   // bit 5

// FIFO_COUNT — lectura del número de bytes en FIFO
#define ICM45686_REG_FIFO_COUNT_L      0x12   // FIFO_COUNT_0 (LSB)
#define ICM45686_REG_FIFO_COUNT_H      0x13   // FIFO_COUNT_1 (MSB)

// FIFO_DATA — registro de lectura burst del FIFO
#define ICM45686_REG_FIFO_DATA         0x14
static uint8_t g_is_big_endian = 0;
typedef struct {
    float ax, ay, az;   // g
    float gx, gy, gz;   // deg/s
    uint8_t count;      // muestras promediadas
} ICM_Averaged_t;

/* Read and Write Function ---------------------------------------------------*/
esp_err_t ICM45686_SPI_GPIO_Initialization(void);
uint8_t ICM45686_Readbyte(uint8_t reg_addr);
void ICM45686_Writebyte(uint8_t reg_addr, uint8_t val);
void ICM45686_Read_Buffer(uint8_t reg_addr, uint8_t len, uint8_t* data);
void ICM45686_Write_Buffer(uint8_t reg_addr, uint8_t len, uint8_t* data);

void ICM45686_Get6AxisRawData(int16_t * accel, int16_t * gyro);
void ICM45686_Get3AxisGyroRawData(int16_t * gyro);
void ICM45686_Get3AxisAccRawData(int16_t * accel);
void ICM45686_TempRawData(int16_t * temperature);
void ICM45686_Calculate_Accel_Gyro_Temp(Struct_ICM45686* imu, float range, float dps);
int ICM45686_DataReady_(void);
void ICM45686_Configure_Filter(uint8_t accel_bw, uint8_t gyro_bw);
int ICM45686_DataReady(void);
void ICM45686_Enable_DRDY(void);

int ICM45686_ReadFIFO(Struct_ICM45686* imu, float range, float dps);
esp_err_t icm_set_gyro_lp_avg(ipreg_sys1_reg_170_gyro_lp_avg_sel_t gyr_avg);
esp_err_t icm_set_accel_lp_avg(ipreg_sys2_reg_129_accel_lp_avg_sel_t acc_avg);
void ICM45686_DumpRegisters(void);
esp_err_t icm_set_accel_ln_bw(ipreg_sys2_reg_131_accel_ui_lpfbw_t acc_bw);
esp_err_t icm_set_gyro_ln_bw(ipreg_sys1_reg_172_gyro_ui_lpfbw_sel_t gyr_bw);
esp_err_t ICM45686_ReadIREG(uint16_t address, uint8_t *out_val);
void ICM45686_RegisterDump_Corrected(void);
esp_err_t ICM45686_WriteIREG(uint16_t address, uint8_t value);
esp_err_t ICM45686_SetGyroLPAvg(uint8_t avg_samples);
esp_err_t ICM45686_SetAccelLPAvg(uint8_t avg_samples);
esp_err_t ICM45686_SetGyroUILPFBW(uint8_t bw_setting);
esp_err_t ICM45686_SetAccelUILPFBW(uint8_t bw_setting);
void ICM45686_ConfigureForUltraFast(void);
void ICM45686_ConfigureForLowPower(void);
esp_err_t ICM45686_FixSourceControl(void);
int ICM45686_Initialization_Robust(void);
uint8_t ICM45686_ReadByteI(uint8_t reg);
void ICM45686_RegisterDump_Robust(void);
void ICM45686_RegisterDump_Corrected(void);
uint8_t ICM45686_ReadIREG8(uint16_t address);
static uint8_t ICM45686_ReadIREGz(uint16_t address);
static esp_err_t ICM45686_WriteIREGz(uint16_t address, uint8_t value);
void ICM45686_RegisterDump_Correctedz(void);
int ICM45686_Initialization_UF1(void);
static void ICM45686_EnsureUIBusRecovered(void);
void ICM45686_RegisterDump_Full(void);
static esp_err_t ICM45686_WriteIREGz0(uint16_t address, uint8_t value);
uint8_t ICM45686_GetEndianness(void);
esp_err_t ICM45686_ReadFIFO_Averaged(ICM_Averaged_t *out);
static inline int16_t parse_i16(uint8_t is_big_endian,
                                 const uint8_t *p);
								 void ICM45686_FIFO_Init(void);
								 uint8_t ICM45686_GetEndianness(void);
#ifdef __cplusplus
}
#endif

#endif /* __ICM45686_H__ */
