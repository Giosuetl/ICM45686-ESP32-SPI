#include "icm45685_spiV2.h"

#include <string.h>
#include "esp_log.h"
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
spi_device_handle_t icm45686_spi;
//Struct_ICM45686 ICM45686;
/* ---------- Helper: allocate DMA-capable buffer ---------- */
static void *dma_calloc(size_t len) {
    void *p = heap_caps_malloc(len, MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
    if (p) memset(p, 0, len);
    return p;
}

/* ---------- Low-level SPI read/write helpers ---------- */
/* Nota: en SPI del ICM, bit de lectura suele ser MSB=1 (address | 0x80 para read). */
#define SPI_MAX_TRANSFER 64

esp_err_t ICM45686_ReadBytes(uint8_t reg_addr, uint8_t *out_buf, size_t len)
{
    if (!icm45686_spi || !out_buf || len == 0 || len > SPI_MAX_TRANSFER) 
        return ESP_ERR_INVALID_ARG;

    static uint8_t tx_buf[SPI_MAX_TRANSFER + 1];
    static uint8_t rx_buf[SPI_MAX_TRANSFER + 1];
    
    tx_buf[0] = reg_addr | 0x80;
    memset(&tx_buf[1], 0, len);

    spi_transaction_t t = {
        .length = 8 * (len + 1),
        .rxlength = 8 * (len + 1),
        .tx_buffer = tx_buf,
        .rx_buffer = rx_buf
    };

    esp_err_t ret = spi_device_transmit(icm45686_spi, &t);
    if (ret == ESP_OK) {
        memcpy(out_buf, &rx_buf[1], len);
    } else {
        ESP_LOGE("ICM", "SPI read failed: %s", esp_err_to_name(ret));
    }
    
    return ret;
}

esp_err_t ICM45686_WriteBytes(uint8_t reg_addr, const uint8_t *data, size_t len)
{
    if (!icm45686_spi || !data || len == 0 || len > SPI_MAX_TRANSFER) 
        return ESP_ERR_INVALID_ARG;

    static uint8_t tx_buf[SPI_MAX_TRANSFER + 1];
    
    tx_buf[0] = reg_addr & 0x7F;
    memcpy(&tx_buf[1], data, len);

    spi_transaction_t t = {
        .length = 8 * (len + 1),
        .tx_buffer = tx_buf
    };

    esp_err_t ret = spi_device_transmit(icm45686_spi, &t);
    if (ret != ESP_OK) {
        ESP_LOGE("ICM", "SPI write failed: %s", esp_err_to_name(ret));
    }
    
    return ret;
}
uint8_t ICM45686_ReadByte(uint8_t reg_addr)
{
    uint8_t val = 0;
    if (ICM45686_ReadBytes(reg_addr, &val, 1) != ESP_OK) {
        ESP_LOGW("ICM", "ReadByte failed for reg 0x%02X", reg_addr);
        return 0xFF;
    }
    return val;
}

esp_err_t ICM45686_WriteByte(uint8_t reg_addr, uint8_t value)
{
    return ICM45686_WriteBytes(reg_addr, &value, 1);
}

esp_err_t  ICM45686_SPI_GPIO_Initialization(void) {
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
    gpio_config_t int_conf = {
        .pin_bit_mask = (1ULL << PIN_NUM_INT),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_POSEDGE
    };
    gpio_config(&int_conf);
    
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
        .clock_speed_hz = 20 * 1000 * 1000,
        .mode = 0,
        .spics_io_num =PIN_NUM_CS,
        .queue_size = 3,
    };
    ret = spi_bus_add_device(ICM45686_SPI_HOST, &devcfg, &icm45686_spi);
    if (ret != ESP_OK) {
        ESP_LOGE("ICM", "spi_bus_add_device failed: %s", esp_err_to_name(ret));
      return ret;
    }
        ESP_LOGI("ICM", "SPI and GPIO initialized");
           return ESP_OK;
}


uint8_t ICM45686_Readbyte(uint8_t reg_addr) {
    uint8_t tx[2] = { reg_addr | 0x80, 0x00 };
    uint8_t rx[2];
    spi_transaction_t t = {
        .length = 16,
        .tx_buffer = tx,
        .rx_buffer = rx
    };
    ICM45686_CHIP_SELECT();
    spi_device_transmit(icm45686_spi, &t);
    ICM45686_CHIP_DESELECT();
    return rx[1];
}

void ICM45686_Read_Buffer(uint8_t reg_addr, uint8_t len, uint8_t* data) {
    uint8_t tx[len + 1];
    uint8_t rx[len + 1];
    tx[0] = reg_addr | 0x80;
    memset(&tx[1], 0x00, len);

    spi_transaction_t t = {
        .length = 8 * (len + 1),
        .tx_buffer = tx,
        .rx_buffer = rx
    };

    ICM45686_CHIP_SELECT();
    spi_device_transmit(icm45686_spi, &t);
    ICM45686_CHIP_DESELECT();

    memcpy(data, &rx[1], len);
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
    uint8_t tx[len + 1];
    tx[0] = reg_addr & 0x7F;   // Write
    memcpy(&tx[1], data, len);

    spi_transaction_t t = {
        .length = 8 * (len + 1),
        .tx_buffer = tx
    };

    ICM45686_CHIP_SELECT();
    spi_device_transmit(icm45686_spi, &t);
    ICM45686_CHIP_DESELECT();
}


void ICM45686_Get6AxisRawData(int16_t * accel, int16_t * gyro) {
    uint8_t data[12];
     if (ICM45686_ReadBytes(0x00, data, sizeof(data)) != ESP_OK) {
        ESP_LOGE("ICM", "Failed to read 6-axis raw");
        memset(data, 0, sizeof(data));
    }//(0x00, 12, data);
    
accel[0] = (int16_t)((data[1] << 8) | data[0]); // <-- CAMBIO
accel[1] = (int16_t)((data[3] << 8) | data[2]);
accel[2] = (int16_t)((data[5] << 8) | data[4]);
gyro[0]  = (int16_t)((data[7] << 8) | data[6]);
gyro[1]  = (int16_t)((data[9] << 8) | data[8]);
gyro[2]  = (int16_t)((data[11] << 8) | data[10]);


}


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

int ICM45686_DataReady_(void) {
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

int ICM45686_DataReady(void) {
    uint8_t status1 = ICM45686_Readbyte(ICM45686_REG_INT_STATUS1);
    // Bit 0 de INT_STATUS1 suele ser DATA_RDY_INT
    return (status1 & 0x01); 
}
void ICM45686_Enable_DRDY(void) {
    // 1. Configurar salida del pin INT1: push-pull, active high
    ICM45686_Writebyte(ICM45686_REG_INT1_CONFIG0, 0x05);
    vTaskDelay(pdMS_TO_TICKS(10));

    // 2. Habilitar fuente DATA READY
    ICM45686_Writebyte(ICM45686_REG_INT1_CONFIG1, 0x01);
    vTaskDelay(pdMS_TO_TICKS(10));

    // 3. Enrutar DATA READY a INT1
    ICM45686_Writebyte(ICM45686_REG_INT1_CONFIG2, 0x01);
    vTaskDelay(pdMS_TO_TICKS(10));


    ESP_LOGI("DRDY", "DRDY interrupts enabled");
}
// Dump corregido y con verificación
// Leer IREG (versión robusta): devuelve ESP_OK y escribe *out_val, o retorna error.
esp_err_t ICM45686_ReadIREG(uint16_t address, uint8_t *out_val)
{
    if (!out_val) return ESP_ERR_INVALID_ARG;

    // IREG_ADDR_15_8 = 0x7C, IREG_ADDR_7_0 = 0x7D, IREG_DATA = 0x7E, REG_MISC2 = 0x7F
    const uint8_t IREG_ADDR_15_8 = 0x7C;
    const uint8_t IREG_ADDR_7_0  = 0x7D;
    const uint8_t IREG_DATA      = 0x7E;
    const uint8_t REG_MISC2      = 0x7F;
    const uint8_t IREG_DONE_BIT  = 0x01; // bit0

    // Escribe dirección (MSB primero)
    esp_err_t ret;
    ret = ICM45686_WriteByte(IREG_ADDR_15_8, (uint8_t)((address >> 8) & 0xFF));
    if (ret != ESP_OK) return ret;
    ret = ICM45686_WriteByte(IREG_ADDR_7_0, (uint8_t)(address & 0xFF));
    if (ret != ESP_OK) return ret;

    // Tras programar las direcciones, el dispositivo inicia un read-pre-fetch al finalizar la transacción.
    // Espera el tiempo mínimo (datasheet: mínimo 4 µs). Usamos 10 µs para margen.
    esp_rom_delay_us(10);

    // Opcionalmente, comprobar IREG_DONE en REG_MISC2 (bit0 == 1 indica completado).
    // Hacemos un pequeño polling con timeout.
    const int max_poll = 100; // intenta hasta ~100 * 10us = 1 ms máx
    int i;
    for (i = 0; i < max_poll; ++i) {
        uint8_t misc = ICM45686_ReadByte(REG_MISC2); // si falla, devuelve 0xFF pero seguimos
        if ((misc & IREG_DONE_BIT) == IREG_DONE_BIT)
            break;
        esp_rom_delay_us(10);
    }
    // si i==max_poll no es necesariamente fatal; aún así intentamos leer IREG_DATA

    // Leer IREG_DATA
    uint8_t data = ICM45686_ReadByte(IREG_DATA);

    // Si el read devolvió 0xFF puede ser un error de bus; aquí lo tratamos como lectura pero caller puede verificar.
    *out_val = data;
    return ESP_OK;
}

// Wrapper de conveniencia que devuelve valor o 0xFF en error
uint8_t ICM45686_ReadIREG8(uint16_t address)
{
    uint8_t v = 0xFF;
    if (ICM45686_ReadIREG(address, &v) != ESP_OK)
        return 0xFF;
    return v;
}

// Register dump corregido (usa direcciones 16-bit correctas)
void ICM45686_RegisterDump_Corrected(void)
{
    ESP_LOGI("ICM-DUMP", "=================== ICM45686 REGISTER DUMP ====================");
    // WHO_AM_I directo (addr 0x72)
    uint8_t who = ICM45686_ReadByte(0x72);
    ESP_LOGI("ICM-DUMP", "WHO_AM_I = 0x%02X", who);

    // PWR_MGMT0 (directo addr 0x10)
    uint8_t v = ICM45686_ReadByte(0x10);
    ESP_LOGI("ICM-DUMP", "PWR_MGMT0 = 0x%02X  (GyroMode=%d, AccelMode=%d)",
             v, (v >> 2) & 0x3, v & 0x3);

    // GYRO/ACCEL config (directos)
    v = ICM45686_ReadByte(0x1C); // GYRO_CONFIG0 (ejemplo addr, ajusta si tu defines otra cosa)
    ESP_LOGI("ICM-DUMP", "GYRO_CONFIG0 = 0x%02X", v);

    v = ICM45686_ReadByte(0x1D); // GYRO_CONFIG1
    ESP_LOGI("ICM-DUMP", "GYRO_CONFIG1 = 0x%02X", v);

    v = ICM45686_ReadByte(0x13); // ACCEL_CONFIG0
    ESP_LOGI("ICM-DUMP", "ACCEL_CONFIG0 = 0x%02X", v);

    v = ICM45686_ReadByte(0x14); // ACCEL_CONFIG1
    ESP_LOGI("ICM-DUMP", "ACCEL_CONFIG1 = 0x%02X", v);

    v = ICM45686_ReadByte(0x7A); // AAF_CONFIG1 (según tu defines)
    ESP_LOGI("ICM-DUMP", "AAF_CONFIG1 = 0x%02X", v);

    v = ICM45686_ReadByte(0x7B); // SRC_CONFIG (ejemplo)
    ESP_LOGI("ICM-DUMP", "SRC_CONFIG = 0x%02X", v);

    ESP_LOGI("ICM-DUMP", "---------------- INDIRECT REGISTERS ----------------");

    // RAW IREG addresses: sumar base A400/A500 según el banco (IPREG_SYS1 -> base 0xA400; IPREG_SYS2 -> 0xA500)
    // Direcciones finales (16-bit) que suele usar el driver/datasheet:
    // SYS1_REG_166 => 0xA4A6
    // SYS2_REG_123 => 0xA57B
    // SYS1_REG_170 => 0xA4AA
    // SYS2_REG_129 => 0xA581
    // SYS1_REG_172 => 0xA4AC
    // SYS2_REG_131 => 0xA583

    uint8_t sys1_166 = ICM45686_ReadIREG8(0xA4A6);
    ESP_LOGI("ICM-DUMP", "SYS1_REG_166 = 0x%02X", sys1_166);

    uint8_t sys2_123 = ICM45686_ReadIREG8(0xA57B);
    ESP_LOGI("ICM-DUMP", "SYS2_REG_123 = 0x%02X", sys2_123);

    uint8_t sys1_170 = ICM45686_ReadIREG8(0xA4AA);
    ESP_LOGI("ICM-DUMP", "SYS1_REG_170 = 0x%02X", sys1_170);

    uint8_t sys2_129 = ICM45686_ReadIREG8(0xA581);
    ESP_LOGI("ICM-DUMP", "SYS2_REG_129 = 0x%02X", sys2_129);

    uint8_t sys1_172 = ICM45686_ReadIREG8(0xA4AC);
    ESP_LOGI("ICM-DUMP", "SYS1_REG_172 = 0x%02X", sys1_172);

    uint8_t sys2_131 = ICM45686_ReadIREG8(0xA583);
    ESP_LOGI("ICM-DUMP", "SYS2_REG_131 = 0x%02X", sys2_131);

    // Verificación final: WHO_AM_I otra vez para asegurarnos que UI no quedó dañado
    uint8_t who_after = ICM45686_ReadByte(0x72);
    ESP_LOGI("ICM-DUMP", "WHO_AM_I (post-IREG recovery) = 0x%02X", who_after);
    if (who_after != 0xE9) {
        ESP_LOGW("ICM-DUMP", "ALERTA: IREG afectó el bus UI -> Recovery falló (WHO_AM_I mismatch)");
        // Medida sencilla de recuperación: reescribir INTF_CONFIG0 (0x00) y reintentar small delay
        ICM45686_WriteByte(0x11, 0x00); // INTF_CONFIG0 addr = 0x11 (ajusta si en tu header es otra)
        vTaskDelay(pdMS_TO_TICKS(5));
    }

    ESP_LOGI("ICM-DUMP", "==========================================================");
}
// -----------------------------------------------------------------------------
// Helpers IREG (indirect registers) robustos para ICM45686 (ESP-IDF)
// Usa tus wrappers existentes: ICM45686_ReadByte, ICM45686_WriteByte
// -----------------------------------------------------------------------------

// Escribe un registro indirecto: address = 16-bit IPREG address (ej: 0x00A6 = 166)
static esp_err_t ICM45686_WriteIREGz0(uint16_t address, uint8_t value)
{
    esp_err_t ret;
    // 1. Escribir high byte
    ICM45686_WriteByte(0x7C, (address >> 8) & 0xFF);

    // 2. Escribir low byte
    ICM45686_WriteByte(0x7D, address & 0xFF);

    // 3. Escribir el dato
    ICM45686_WriteByte(0x7E, value);

    // 4. Esperar confirmación
    for (int i = 0; i < 50; i++)
    {
        uint8_t resp = ICM45686_ReadByte(0x47); // IREG_RESP
        if ((resp & 0x80) == 0)  // bit7 == 0 → finished
            return ESP_OK;

        esp_rom_delay_us(10);
    }

    ESP_LOGW("ICM", "WARNING: IREG write timeout addr=0x%04X", address);
    return ESP_OK;
}
#define IPREG_SYS1_REG_172                                                      0xa4ac
typedef struct {
	uint8_t gyro_ui_lpfbw_sel                                                      : 3;
	uint8_t resv_1                                                                 : 4;
	uint8_t gyro_ois_hpf1_byp                                                      : 1;
} ipreg_sys1_reg_172_t;
#define IREG_ADDR_15_8                                                          0x7c
typedef struct {
	uint8_t ireg_addr_15_8                                                         : 8;
} ireg_addr_15_8_t;
#define IREG_DATA                                                               0x7e
typedef struct {
	uint8_t ireg_data                                                              : 8;
} ireg_data_t;

static esp_err_t ICM45686_WriteIREGz(uint16_t address, uint8_t value)
{
    esp_err_t ret;
    uint8_t data[3];


    data[0] = (address & 0xFF00) >> 8;
	data[1] = address & 0xFF;
	/* 3rd byte is the first data to write*/
	data[2] = value;

	/* Burst write address and first byte */
	 esp_rom_delay_us(4);
	//write_dreg(IREG_ADDR_15_8, 3, data);

 ICM45686_WriteBytes(IREG_ADDR_15_8, data, 3);

        esp_rom_delay_us(10);
   
	//for (uint32_t i = 1; i < 1; i++) {
	//	write_dreg(IREG_DATA, 1, data[1]);
	//	  esp_rom_delay_us(4);
	//}


    return ESP_OK;
}
// Leer un registro indirecto: address = 16-bit IPREG address (ej: 0x00A6 = 166)
static uint8_t ICM45686_ReadIREGz(uint16_t address)
{
    esp_err_t ret;
    uint8_t val;

    // Escribir dirección en IREG_ADDR_15_8 y IREG_ADDR_7_0
    ret = ICM45686_WriteByte(ICM45686_REG_IREG_ADDR_15_8, (uint8_t)((address >> 8) & 0xFF));
    if (ret != ESP_OK) {
        ESP_LOGW("ICM", "Write IREG_ADDR_15_8 failed: %s", esp_err_to_name(ret));
        return 0xFF;
    }
    ret = ICM45686_WriteByte(ICM45686_REG_IREG_ADDR_7_0, (uint8_t)(address & 0xFF));
    if (ret != ESP_OK) {
        ESP_LOGW("ICM", "Write IREG_ADDR_7_0 failed: %s", esp_err_to_name(ret));
        return 0xFF;
    }

    // Esperar lo requerido por datasheet (mínimo ~4µs). Usamos 10µs.
    esp_rom_delay_us(10);

    // Leer IREG_DATA
    val = ICM45686_ReadByte(ICM45686_REG_IREG_DATA);
    return val;
}

// Función de recuperación simple tras uso de IREG: vuelve a reconfigurar la interfaz UI si WHO_AM_I no responde.
static void ICM45686_EnsureUIBusRecovered(void)
{
    uint8_t who = ICM45686_ReadByte(ICM45686_REG_WHO_AM_I);
    if (who != ICM45686_ID) {
        ESP_LOGW("ICM", "WHO_AM_I mismatch after IREG ops (0x%02X). Trying UI recovery...", who);
        // Reescribir INTF_CONFIG0 (por ejemplo 0x00) y small delay
        ICM45686_WriteByte(ICM45686_REG_INTF_CONFIG0, 0x00);
        vTaskDelay(pdMS_TO_TICKS(1));
        // Re-lectura
        who = ICM45686_ReadByte(ICM45686_REG_WHO_AM_I);
        if (who == ICM45686_ID) {
            ESP_LOGI("ICM", "UI bus recovered (WHO_AM_I ok).");
        } else {
            ESP_LOGW("ICM", "Recovery failed, WHO_AM_I = 0x%02X", who);
        }
    }
}

// -----------------------------------------------------------------------------
// REGISTER DUMP COMPLETO y CORREGIDO
// -----------------------------------------------------------------------------
void ICM45686_RegisterDump_Full(void)
{
    ESP_LOGI("ICM-DUMP", "=================== ICM45686 REGISTER DUMP ===================");

    // Registros directos (usa tus defines, si son distintos cámbialos)
    uint8_t v;
    v = ICM45686_ReadByte(ICM45686_REG_WHO_AM_I);
    ESP_LOGI("ICM-DUMP", "WHO_AM_I = 0x%02X", v);

    v = ICM45686_ReadByte(ICM45686_REG_PWR_MGMT0);
    ESP_LOGI("ICM-DUMP", "PWR_MGMT0 = 0x%02X  (GyroMode=%d, AccelMode=%d)", v, (v >> 2) & 0x3, v & 0x3);

    v = ICM45686_ReadByte(ICM45686_REG_GYRO_CONFIG0);
    ESP_LOGI("ICM-DUMP", "GYRO_CONFIG0 = 0x%02X", v);

    v = ICM45686_ReadByte(ICM45686_REG_GYRO_CONFIG1);
    ESP_LOGI("ICM-DUMP", "GYRO_CONFIG1 = 0x%02X", v);

    v = ICM45686_ReadByte(ICM45686_REG_ACCEL_CONFIG0);
    ESP_LOGI("ICM-DUMP", "ACCEL_CONFIG0 = 0x%02X", v);

    v = ICM45686_ReadByte(ICM45686_REG_ACCEL_CONFIG1);
    ESP_LOGI("ICM-DUMP", "ACCEL_CONFIG1 = 0x%02X", v);


    // Indirect registers (usar direcciones IPREG como 16-bit: 0x00AA = 170 dec, etc.)
    ESP_LOGI("ICM-DUMP", "---------------- INDIRECT REGISTERS ----------------");

    // Direcciones usadas (según datasheet y tus pruebas):
    // SYS1_REG_166  -> 166 decimal -> 0x00A6
    // SYS2_REG_123  -> 123 decimal -> 0x007B
    // SYS1_REG_170  -> 170 decimal -> 0x00AA
    // SYS2_REG_129  -> 129 decimal -> 0x0081
    // SYS1_REG_172  -> 172 decimal -> 0x00AC
    // SYS2_REG_131  -> 131 decimal -> 0x0083

    uint8_t sys1_166 = ICM45686_ReadIREGz(0xA4A6);
    ESP_LOGI("ICM-DUMP", "SYS1_REG_166 = 0x%02X", sys1_166);

    uint8_t sys2_123 = ICM45686_ReadIREGz(0xA57B);
    ESP_LOGI("ICM-DUMP", "SYS2_REG_123 = 0x%02X", sys2_123);

    uint8_t sys1_170 = ICM45686_ReadIREGz(0xA4AA);
    ESP_LOGI("ICM-DUMP", "SYS1_REG_170 = 0x%02X", sys1_170);

    uint8_t sys2_129 = ICM45686_ReadIREGz(0xA581);
    ESP_LOGI("ICM-DUMP", "SYS2_REG_129 = 0x%02X", sys2_129);

    uint8_t sys1_172 = ICM45686_ReadIREGz(0xA4AC);
    ESP_LOGI("ICM-DUMP", "SYS1_REG_172 = 0x%02X", sys1_172);

    uint8_t sys2_131 = ICM45686_ReadIREGz(0xA583);
    ESP_LOGI("ICM-DUMP", "SYS2_REG_131 = 0x%02X", sys2_131);
    

    // Verificación final: asegurar que UI bus no quedó "des-configurado"
    uint8_t who_after = ICM45686_ReadByte(ICM45686_REG_WHO_AM_I);
    ESP_LOGI("ICM-DUMP", "WHO_AM_I (post-IREG recovery) = 0x%02X", who_after);
    if (who_after != ICM45686_ID) {
        ESP_LOGW("ICM-DUMP", "ALERTA: IREG afectó el bus UI -> intentando recovery...");
        // Intento de recuperación simple
        ICM45686_WriteByte(ICM45686_REG_INTF_CONFIG0, 0x00);
        vTaskDelay(pdMS_TO_TICKS(1));
        who_after = ICM45686_ReadByte(ICM45686_REG_WHO_AM_I);
        ESP_LOGI("ICM-DUMP", "WHO_AM_I (after recovery) = 0x%02X", who_after);
    }

    ESP_LOGI("ICM-DUMP", "==========================================================");
}

// -----------------------------------------------------------------------------
// Initialization UF (la tuya: versión legible y con uso de WriteIREG)
// -----------------------------------------------------------------------------
int ICM45686_Initialization_UF1(void)
{
    ESP_LOGI("ICM-", "============yeyeyeyey============================");

    // --- Inicializar GPIO del CS ---
    ICM45686_SPI_GPIO_Initialization();
    vTaskDelay(pdMS_TO_TICKS(20));

    // --- Reset completo ---
    if (ICM45686_WriteByte(ICM45686_REG_PWR_MGMT0, 0x80) != ESP_OK)
        return 0;
    vTaskDelay(pdMS_TO_TICKS(200));

    // --- Verificar WHO_AM_I ---
    uint8_t who = ICM45686_ReadByte(ICM45686_REG_WHO_AM_I);
    if (who != ICM45686_ID)
        return 0;

    // --- Config interfaz SPI (MSB-first, sin delays, sin pullups) ---
    if (ICM45686_WriteByte(ICM45686_REG_INTF_CONFIG0, 0x00) != ESP_OK)
        return 0;
    vTaskDelay(2);

    // STATIC5 (ACEL) -> BYPASS (indirect)
    ICM45686_WriteIREGz0(0x0028, 0x00); // IPREG_SYS2_REG_40 (0x28) STATIC5 accel -> bypass
    vTaskDelay(2);

    // STATIC5 (GYRO) -> BYPASS (indirect)
    ICM45686_WriteIREGz0(0x0021, 0x00); // IPREG_SYS1_REG_33? (0x21) -> bypass
    vTaskDelay(2);

    // Configurar GYRO: FS = 4000 dps, ODR = 6.4 kHz, BW = máxima (UI BW bits in gyro_config0 are [7:4])
    uint8_t gyro_cfg = ICM45686_GYRO_4000_DPS | ICM45686_GYRO_ODR_6_4KHz | (0x00 << 4);
    if (ICM45686_WriteByte(ICM45686_REG_GYRO_CONFIG0, gyro_cfg) != ESP_OK)
        return 0;
    vTaskDelay(2);

    // Configurar ACCEL: FS = ±16g, ODR = 6.4 kHz, BW = máxima
    uint8_t accel_cfg = ICM45686_ACCEL_16G | ICM45686_ACCEL_ODR_6_4KHz | (0x00 << 4);
    if (ICM45686_WriteByte(ICM45686_REG_ACCEL_CONFIG0, accel_cfg) != ESP_OK)
        return 0;
    vTaskDelay(2);

    // DESACTIVAR filtros digitales UI (bypass)
    /*
    apaga los filtros “normales” del UI, no los avanzados.
    los filtros UI del “UI domain”:
    UI LPF simplificado (no el UI_LPFBW configurable por IREG),UI averaging,UI HPF
    */
    ICM45686_WriteByte(ICM45686_REG_ACCEL_CONFIG1, 0x00);
    ICM45686_WriteByte(ICM45686_REG_GYRO_CONFIG1, 0x00);
    vTaskDelay(2);

    // Seleccionar INTERPOLATOR__FIR mediante registros indirectos IPREG_SYS2_REG_123/IPREG_SYS1_REG_166
   // ICM45686_WriteIREGz(IPREG_SYS2_REG_123, IPREG_SYS2_REG_123_ACCEL_SRC_CTRL_INTERPOLATOR_ON_FIR_ON); // 
   // vTaskDelay(2);
   // ICM45686_WriteIREGz(IPREG_SYS1_REG_166, IPREG_SYS1_REG_166_GYRO_SRC_CTRL_INTERPOLATOR_ON_FIR_ON);  //
   // vTaskDelay(2);
    ICM45686_WriteIREGz(IPREG_SYS2_REG_129, IPREG_SYS2_REG_129_ACCEL_LP_AVG_64); // addr 0x0083 == 131 dec
    vTaskDelay(2);
    ICM45686_WriteIREGz(IPREG_SYS1_REG_170, IPREG_SYS1_REG_170_GYRO_LP_AVG_64); // addr 0x0083 == 131 dec
    vTaskDelay(2);
    // Seleccionar UI LPF BW mediante registros indirectos SYS1/ SYS2 reg_172/131
    ICM45686_WriteIREGz(0xA583, IPREG_SYS2_REG_131_ACCEL_UI_LPFBW_DIV_128); // addr 0x0083 == 131 dec
    vTaskDelay(2);
    ICM45686_WriteIREGz(0xA4AC, IPREG_SYS1_REG_172_GYRO_UI_LPFBW_DIV_128);  // addr 0x00AC == 172 dec
    vTaskDelay(2);
// ICM45686_WriteIREGz0 no funciona, siempre da valor de 0 ICM45686_WriteIREGz si funciona 
    // ENCENDER ADCs DE 32 kHz (LOW NOISE): [3:2]=Gyro mode 0b11, [1:0]=Accel mode 0b11 => 0x0F
    if (ICM45686_WriteByte(ICM45686_REG_PWR_MGMT0, 0x0F) != ESP_OK)
        return 0;
    vTaskDelay(pdMS_TO_TICKS(10));

    ESP_LOGI("ICM", "ICM45686 inicializado en modo ULTRA-FAST (6.4kHz + BW Máx)");

    // Dump completo para verificar
    ICM45686_RegisterDump_Full();
// Llamar en init y guardar:

// En ICM45686_Initialization_UF1, al final:
g_is_big_endian = ICM45686_GetEndianness();
ESP_LOGI("ICM", "Endianness: %s", g_is_big_endian ? "BIG" : "LITTLE");
    ICM45686_FIFO_Init();
    return 1;
}

// ============================================================
// 1. Leer endianness al arrancar (SREG_CTRL = 0xA267)
// ============================================================
uint8_t ICM45686_GetEndianness(void)
{
    // SREG_CTRL es un registro indirecto (addr > 0xFF)
    uint8_t val = ICM45686_ReadIREGz(0xA267);
    return (val >> 1) & 0x01;  // bit1 = sreg_data_endian_sel
    // 0 = little endian, 1 = big endian
}



// ============================================================
// 2. Init FIFO corregido
// ============================================================
void ICM45686_FIFO_Init(void)
{
    // PASO 1: deshabilitar todo
    ICM45686_WriteByte(ICM45686_REG_FIFO_CONFIG3, 0x00);
    vTaskDelay(2);

    // PASO 2: bypass
    ICM45686_WriteByte(ICM45686_REG_FIFO_CONFIG0, 0x00);
    vTaskDelay(2);

    // PASO 3: watermark = 96 bytes (6 paquetes x 16 bytes)
    ICM45686_WriteByte(ICM45686_REG_FIFO_CONFIG1_0, 0x60); // LSB
    ICM45686_WriteByte(ICM45686_REG_FIFO_CONFIG1_1, 0x00); // MSB
    vTaskDelay(2);

    // PASO 4: stream mode
    ICM45686_WriteByte(ICM45686_REG_FIFO_CONFIG0, 0x40);
    vTaskDelay(2);

    // PASO 5: habilitar IF + accel + gyro — todo en CONFIG3
    ICM45686_WriteByte(ICM45686_REG_FIFO_CONFIG3, 0x07);
    vTaskDelay(pdMS_TO_TICKS(10));
}

// ============================================================
// 3. Parser corregido con endianness y frame de 16 bytes
// ============================================================
static inline int16_t parse_i16(uint8_t is_big_endian,
                                 const uint8_t *p)
{
    if (is_big_endian)
        return (int16_t)((p[0] << 8) | p[1]);
    else
        return (int16_t)((p[1] << 8) | p[0]);
}

esp_err_t ICM45686_ReadFIFO_Averaged(ICM_Averaged_t *out)
{
    if (!out) return ESP_ERR_INVALID_ARG;
    out->ax = out->ay = out->az = 0.0f;
    out->gx = out->gy = out->gz = 0.0f;
    out->count = 0;

    // --- 1) Count ---
    uint8_t cnt[2];
    ICM45686_ReadBytes(ICM45686_REG_FIFO_COUNT_L, cnt, 2);
    uint16_t fifo_bytes = ((uint16_t)cnt[1] << 8) | cnt[0];

    uint8_t n_pkts = fifo_bytes / 16;
    if (n_pkts == 0) return ESP_ERR_NOT_FOUND;
    if (n_pkts > FIFO_MAX_PKTS) n_pkts = FIFO_MAX_PKTS;

    // --- 2) Burst read ---
    uint8_t buf[FIFO_MAX_PKTS * 16];
    esp_err_t err = ICM45686_ReadBytes(ICM45686_REG_FIFO_DATA,
                                        buf, n_pkts * 16);
    if (err != ESP_OK) return err;

    // --- 3) Parsear con endianness correcto ---
    float sum_ax=0, sum_ay=0, sum_az=0;
    float sum_gx=0, sum_gy=0, sum_gz=0;
    uint8_t valid = 0;

    for (uint8_t i = 0; i < n_pkts; i++) {
        const uint8_t *p = &buf[i * 16];

        // Header: bit7=1 → inválido (empty marker)
        if (p[0] & 0x80) continue;

        // Parsear con endianness del chip
        int16_t raw_ax = parse_i16(g_is_big_endian, &p[1]);
        int16_t raw_ay = parse_i16(g_is_big_endian, &p[3]);
        int16_t raw_az = parse_i16(g_is_big_endian, &p[5]);
        int16_t raw_gx = parse_i16(g_is_big_endian, &p[7]);
        int16_t raw_gy = parse_i16(g_is_big_endian, &p[9]);
        int16_t raw_gz = parse_i16(g_is_big_endian, &p[11]);
        // p[13] = temp (int8, ignorar)
        // p[14..15] = timestamp (ignorar)

        if (raw_ax == (int16_t)0x8000 || raw_gx == (int16_t)0x8000) continue;

        sum_ax += raw_ax * ICM45686_FIFO_ACCEL_SCALE_16G;
        sum_ay += raw_ay * ICM45686_FIFO_ACCEL_SCALE_16G;
        sum_az += raw_az * ICM45686_FIFO_ACCEL_SCALE_16G;
        sum_gx += raw_gx * ICM45686_FIFO_GYRO_SCALE_4000DPS;
        sum_gy += raw_gy * ICM45686_FIFO_GYRO_SCALE_4000DPS;
        sum_gz += raw_gz * ICM45686_FIFO_GYRO_SCALE_4000DPS;
        valid++;
    }

    if (valid == 0) return ESP_ERR_INVALID_RESPONSE;

    float inv  = 1.0f / valid;
    out->ax    = sum_ax * inv;
    out->ay    = sum_ay * inv;
    out->az    = sum_az * inv;
    out->gx    = sum_gx * inv;
    out->gy    = sum_gy * inv;
    out->gz    = sum_gz * inv;
    out->count = valid;
    return ESP_OK;
}

////////////////////////////////////////////////////





/*

I (29186) ICM-DUMP: =================== ICM45686 REGISTER DUMP ===================
I (29186) ICM-DUMP: WHO_AM_I = 0xE9
I (29186) ICM-DUMP: PWR_MGMT0 = 0x0F  (GyroMode=3, AccelMode=3)
I (29196) ICM-DUMP: GYRO_CONFIG0 = 0x03
I (29206) ICM-DUMP: GYRO_CONFIG1 = 0x00
I (29206) ICM-DUMP: ACCEL_CONFIG0 = 0x13
I (29206) ICM-DUMP: ACCEL_CONFIG1 = 0x00
I (29216) ICM-DUMP: ---------------- INDIRECT REGISTERS ----------------
I (29226) ICM-DUMP: SYS1_REG_166 = 0x1B
I (29226) ICM-DUMP: SYS2_REG_123 = 0x34
I (29236) ICM-DUMP: SYS1_REG_170 = 0x0A
I (29236) ICM-DUMP: SYS2_REG_129 = 0x02
I (29236) ICM-DUMP: SYS1_REG_172 = 0x02
I (29246) ICM-DUMP: SYS2_REG_131 = 0x02
I (29246) ICM-DUMP: WHO_AM_I (post-IREG recovery) = 0xE9
I (29256) ICM-DUMP: ==========================================================
*/


/*
La configuracion actual div 128 con filtro promedio movil de 7 muestras solo en acelerometro tuvo un error absoluto de 0.7 grados de 0 a 39
I (29186) ICM-DUMP: WHO_AM_I = 0xE9
I (29186) ICM-DUMP: PWR_MGMT0 = 0x0F  (GyroMode=3, AccelMode=3)
I (29196) ICM-DUMP: GYRO_CONFIG0 = 0x03
I (29206) ICM-DUMP: GYRO_CONFIG1 = 0x00
I (29206) ICM-DUMP: ACCEL_CONFIG0 = 0x13
I (29206) ICM-DUMP: ACCEL_CONFIG1 = 0x00
I (29216) ICM-DUMP: ---------------- INDIRECT REGISTERS ----------------
I (29226) ICM-DUMP: SYS1_REG_166 = 0x1B
I (29226) ICM-DUMP: SYS2_REG_123 = 0x34
I (29236) ICM-DUMP: SYS1_REG_170 = 0x0A
I (29236) ICM-DUMP: SYS2_REG_129 = 0x02
I (29236) ICM-DUMP: SYS1_REG_172 = 0x06
I (29246) ICM-DUMP: SYS2_REG_131 = 0x06
I (29246) ICM-DUMP: WHO_AM_I (post-IREG recovery) = 0xE9
I (29256) ICM-DUMP: ========================================================== */
