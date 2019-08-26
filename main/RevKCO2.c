// CO2 + other sensors all
const char TAG[] = "CO2";

#include "revk.h"
#include <driver/i2c.h>
#define ACK_CHECK_EN 0x1        /*!< I2C master will check ack from slave */
#define ACK_CHECK_DIS 0x0       /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0             /*!< I2C ack value */
#define NACK_VAL 0x1            /*!< I2C nack value */

#define settings	\
	u8(sda,0)	\
	u8(scl,4)	\
	u8(port,0)	\
	u8(address,0x61)	\


#define u8(n,d)	uint8_t n;
settings
#undef u8
const char *
app_command (const char *tag, unsigned int len, const unsigned char *value)
{
   return "";
}

void
app_main ()
{
   revk_init (&app_command);
#define u8(n,d) revk_register(#n,0,sizeof(n),&n,#d,0);
   settings
#undef u8
      if (i2c_driver_install (port, I2C_MODE_MASTER, 0, 0, 0))
      return NULL;
   i2c_config_t config = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = sda,
      .scl_io_num = scl,
      .sda_pullup_en = true,
      .scl_pullup_en = true,
      .master.clk_speed = 100000,
   };
   if (i2c_param_config (port, &config))
   {
      i2c_driver_delete (port);
      return NULL;
   }
   i2c_set_timeout (port, 80000);       // 1ms? allow for clock stretching
   i2c_cmd_handle_t i;

   while (1)
   {
      i = i2c_cmd_link_create ();
      i2c_master_start (i);
      i2c_master_write_byte (i, (address << 1), ACK_CHECK_EN);
      i2c_master_write_byte (i, 0x00, ACK_CHECK_EN);    // 0010=start measurements
      i2c_master_write_byte (i, 0x10, ACK_CHECK_EN);
      i2c_master_write_byte (i, 0x00, ACK_CHECK_EN);    // Pressure (0=unknown)
      i2c_master_write_byte (i, 0x00, ACK_CHECK_EN);
      i2c_master_write_byte (i, 0x81, ACK_CHECK_EN);    // CRC
      i2c_master_stop (i);
      esp_err_t err = i2c_master_cmd_begin (port, i, 10 / portTICK_PERIOD_MS);
      i2c_cmd_link_delete (i);
      if (err)
         ESP_LOGI (TAG, "Tx StartMeasure %s", esp_err_to_name (err));
      else
         break;
      sleep (1);                // try again
   }

   while (1)
   {
      usleep (100000);
      i = i2c_cmd_link_create ();
      i2c_master_start (i);
      i2c_master_write_byte (i, (address << 1), ACK_CHECK_EN);
      i2c_master_write_byte (i, 0x02, ACK_CHECK_EN);    // 0202 get reading state
      i2c_master_write_byte (i, 0x02, ACK_CHECK_EN);
      i2c_master_stop (i);
      esp_err_t err = i2c_master_cmd_begin (port, i, 10 / portTICK_PERIOD_MS);
      i2c_cmd_link_delete (i);
      if (err)
         ESP_LOGI (TAG, "Tx GetReady %s", esp_err_to_name (err));
      else
      {
         uint8_t buf[3];
         i = i2c_cmd_link_create ();
         i2c_master_start (i);
         i2c_master_write_byte (i, (address << 1) + 1, ACK_CHECK_EN);
         i2c_master_read (i, buf, 2, ACK_VAL);
         i2c_master_read_byte (i, buf + 2, NACK_VAL);
         i2c_master_stop (i);
         esp_err_t err = i2c_master_cmd_begin (port, i, 10 / portTICK_PERIOD_MS);
         i2c_cmd_link_delete (i);
         if (err)
            ESP_LOGI (TAG, "Rx GetReady %s", esp_err_to_name (err));
         else if ((buf[0] << 8) + buf[1] == 1)
         {
            i = i2c_cmd_link_create ();
            i2c_master_start (i);
            i2c_master_write_byte (i, (address << 1), ACK_CHECK_EN);
            i2c_master_write_byte (i, 0x03, ACK_CHECK_EN);      // 0300 Read data
            i2c_master_write_byte (i, 0x00, ACK_CHECK_EN);
            i2c_master_stop (i);
            esp_err_t err = i2c_master_cmd_begin (port, i, 10 / portTICK_PERIOD_MS);
            i2c_cmd_link_delete (i);
            if (err)
               ESP_LOGI (TAG, "Tx GetData %s", esp_err_to_name (err));
            else
            {
               uint8_t buf[18];
               i = i2c_cmd_link_create ();
               i2c_master_start (i);
               i2c_master_write_byte (i, (address << 1) + 1, ACK_CHECK_EN);
               i2c_master_read (i, buf, 17, ACK_VAL);
               i2c_master_read_byte (i, buf + 17, NACK_VAL);
               i2c_master_stop (i);
               esp_err_t err = i2c_master_cmd_begin (port, i, 10 / portTICK_PERIOD_MS);
               i2c_cmd_link_delete (i);
               if (err)
                  ESP_LOGI (TAG, "Rx Data %s", esp_err_to_name (err));
               else
               {
                  //ESP_LOG_BUFFER_HEX_LEVEL (TAG, buf, 18, ESP_LOG_INFO);
                  uint8_t d[4];
                  d[3] = buf[0];
                  d[2] = buf[1];
                  d[1] = buf[3];
                  d[0] = buf[4];
                  float co2 = *(float *) d;
                  d[3] = buf[6];
                  d[2] = buf[7];
                  d[1] = buf[9];
                  d[0] = buf[10];
                  float t = *(float *) d;
                  d[3] = buf[12];
                  d[2] = buf[13];
                  d[1] = buf[15];
                  d[0] = buf[16];
                  float rh = *(float *) d;
                  revk_info (TAG, "CO2=%.2f T=%.2f RH=%.2f", co2, t, rh);
               }
            }
         }
      }
   }
}
