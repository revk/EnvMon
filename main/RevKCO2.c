// CO2 + other sensors all
const char TAG[] = "CO2";

#include "revk.h"
#include <driver/i2c.h>

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

   while (1)
   {
      sleep (1);
      i2c_cmd_handle_t i = i2c_cmd_link_create ();
      i2c_master_start (i);
      i2c_master_write_byte (i, (address << 1) + 1, 1);
      i2c_master_write_byte (i, 0xD1, 1);
      i2c_master_write_byte (i, 0x00, 1);
      i2c_master_stop (i);
      esp_err_t err = i2c_master_cmd_begin (port, i, 1000);
      i2c_cmd_link_delete (i);
      ESP_LOGI (TAG, "Res %d", err);

   }
}
