# <picture><img src = "https://github.com/Giosuetl/ICM45686-ESP32-SPI/blob/main/icm.png" width = 400px></picture> **ICM45686**
## Esto es un borrador funcional
#### ejemplo de uso:
#### .
#### .
#### .
####   Struct_ICM45686 ICM45686;
####   while (!ICM45686_Initialization_UF1()) {
####           printf("Error inicializando ICM4568. Abortando task.\n");
####           gpio_set_level(led1, 0);
####           vTaskDelay(pdMS_TO_TICKS(100));
####     }
#### .
#### .
#### .
####   ICM45686_Calculate_Accel_Gyro_Temp(&ICM45686, 16.0f,4000.0f);
####   ICM_Averaged_t icmfifo;
####   ICM45686_ReadFIFO_Averaged(&icmfifo);
####   printf("\nax:%f ay:%f az:%f   gx:%f gy:%f gz:%f\n",icmfifo.ax,icmfifo.ay,icmfifo.az,icmfifo.gx,icmfifo.gy,icmfifo.gz);
