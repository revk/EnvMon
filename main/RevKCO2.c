// CO2 + other sensors all
const char TAG[] = "CO2";

#include "revk.h"
#include <driver/i2c.h>

#include "owb.h"
#include "owb_rmt.h"
#include "ds18b20.h"

#define ACK_CHECK_EN 0x1        /*!< I2C master will check ack from slave */
#define ACK_CHECK_DIS 0x0       /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0             /*!< I2C ack value */
#define NACK_VAL 0x1            /*!< I2C nack value */
#define	MAX_OWB	8
#define DS18B20_RESOLUTION   (DS18B20_RESOLUTION_12_BIT)

#define settings	\
	s8(co2sda,-1)	\
	s8(co2scl,-1)	\
	s8(co2address,0x61)	\
	s8(oledsda,-1)	\
	s8(oledscl,-1)	\
	s8(oledaddress,0x3C)	\
	s8(ds18b20,-1)	\

#define s8(n,d)	int8_t n;
settings
#undef s8
static int lastco2 = 0;
static int lastrh = 0;
static int lasttemp = 0;

const char *
app_command (const char *tag, unsigned int len, const unsigned char *value)
{
   if (!strcmp (tag, "connect"))
   {
      lastco2 = 0;
      lasttemp = 0;
      lastrh = 0;
   }
   return "";
}

void
app_main ()
{
   revk_init (&app_command);
#define s8(n,d) revk_register(#n,0,sizeof(n),&n,#d,SETTING_SIGNED);
   settings
#undef s8
      int8_t co2port = -1, oledport = -1;
   if (co2sda >= 0 && co2scl >= 0)
   {
      co2port = 0;
      if (i2c_driver_install (co2port, I2C_MODE_MASTER, 0, 0, 0))
         return;
      i2c_config_t config = {
         .mode = I2C_MODE_MASTER,
         .sda_io_num = co2sda,
         .scl_io_num = co2scl,
         .sda_pullup_en = true,
         .scl_pullup_en = true,
         .master.clk_speed = 100000,
      };
      if (i2c_param_config (co2port, &config))
      {
         i2c_driver_delete (co2port);
         return;
      }
      i2c_set_timeout (co2port, 160000);        // 2ms? allow for clock stretching
   }
   if (oledsda == co2sda && oledscl == co2scl)
      oledport = co2port;
   else if (oledsda >= 0 && oledscl >= 0)
   {                            // Separate OLED port
      oledport = 1;
      if (i2c_driver_install (oledport, I2C_MODE_MASTER, 0, 0, 0))
         return;
      i2c_config_t config = {
         .mode = I2C_MODE_MASTER,
         .sda_io_num = oledsda,
         .scl_io_num = oledscl,
         .sda_pullup_en = true,
         .scl_pullup_en = true,
         .master.clk_speed = 100000,
      };
      if (i2c_param_config (oledport, &config))
      {
         i2c_driver_delete (oledport);
         return;
      }
      i2c_set_timeout (oledport, 160000);       // 2ms? allow for clock stretching
   }

   i2c_cmd_handle_t i;

   if (co2port >= 0)
   {                            // CO2 init
      while (1)
      {
         i = i2c_cmd_link_create ();
         i2c_master_start (i);
         i2c_master_write_byte (i, (co2address << 1), ACK_CHECK_EN);
         i2c_master_write_byte (i, 0x00, ACK_CHECK_EN); // 0010=start measurements
         i2c_master_write_byte (i, 0x10, ACK_CHECK_EN);
         i2c_master_write_byte (i, 0x00, ACK_CHECK_EN); // Pressure (0=unknown)
         i2c_master_write_byte (i, 0x00, ACK_CHECK_EN);
         i2c_master_write_byte (i, 0x81, ACK_CHECK_EN); // CRC
         i2c_master_stop (i);
         esp_err_t err = i2c_master_cmd_begin (co2port, i, 10 / portTICK_PERIOD_MS);
         i2c_cmd_link_delete (i);
         if (err)
            ESP_LOGI (TAG, "Tx StartMeasure %s", esp_err_to_name (err));
         else
            break;
         sleep (1);             // try again
      }
   }
   if (oledport >= 0)
   {                            // OLED Init
      // TODO
   }
   OneWireBus *owb = NULL;
   owb_rmt_driver_info rmt_driver_info;
   DS18B20_Info *ds18b20s[MAX_OWB] = { 0 };
   int num_owb = 0;
   if (ds18b20 >= 0)
   {                            // DS18B20 init
      owb = owb_rmt_initialize (&rmt_driver_info, ds18b20, RMT_CHANNEL_1, RMT_CHANNEL_0);
      owb_use_crc (owb, true);  // enable CRC check for ROM code
      OneWireBus_ROMCode device_rom_codes[MAX_OWB] = { 0 };
      OneWireBus_SearchState search_state = { 0 };
      bool found = false;
      owb_search_first (owb, &search_state, &found);
      while (found && num_owb < MAX_OWB)
      {
         char rom_code_s[17];
         owb_string_from_rom_code (search_state.rom_code, rom_code_s, sizeof (rom_code_s));
         device_rom_codes[num_owb] = search_state.rom_code;
         ++num_owb;
         owb_search_next (owb, &search_state, &found);
      }
      if (!num_owb)
         revk_error ("temp", "No OWB devices");
#if 0
      if (num_owb == 1)
      {
         OneWireBus_ROMCode rom_code;
         owb_status status = owb_read_rom (owb, &rom_code);
         if (status == OWB_STATUS_OK)
         {
            char rom_code_s[OWB_ROM_CODE_STRING_LENGTH];
            owb_string_from_rom_code (rom_code, rom_code_s, sizeof (rom_code_s));
            printf ("Single device %s present\n", rom_code_s);
         } else
         {
            printf ("An error occurred reading ROM code: %d", status);
         }
      } else
      {
         // Search for a known ROM code (LSB first):
         // For example: 0x1502162ca5b2ee28
         OneWireBus_ROMCode known_device = {
            .fields.family = {0x28},
            .fields.serial_number = {0xee, 0xb2, 0xa5, 0x2c, 0x16, 0x02},
            .fields.crc = {0x15},
         };
         char rom_code_s[OWB_ROM_CODE_STRING_LENGTH];
         owb_string_from_rom_code (known_device, rom_code_s, sizeof (rom_code_s));
         bool is_present = false;

         owb_status search_status = owb_verify_rom (owb, known_device, &is_present);
         if (search_status == OWB_STATUS_OK)
         {
            printf ("Device %s is %s\n", rom_code_s, is_present ? "present" : "not present");
         } else
         {
            printf ("An error occurred searching for known device: %d", search_status);
         }
      }
#endif
      for (int i = 0; i < num_owb; i++)
      {
         DS18B20_Info *ds18b20_info = ds18b20_malloc ();        // heap allocation
         ds18b20s[i] = ds18b20_info;
         if (num_owb == 1)
            ds18b20_init_solo (ds18b20_info, owb);      // only one device on bus
         else
            ds18b20_init (ds18b20_info, owb, device_rom_codes[i]);      // associate with bus and device
         ds18b20_use_crc (ds18b20_info, true);  // enable CRC check for temperature readings
         ds18b20_set_resolution (ds18b20_info, DS18B20_RESOLUTION);
      }
   }

   while (1)
   {
      usleep (100000);
      if (co2port >= 0)
      {                         // CO2
         i = i2c_cmd_link_create ();
         i2c_master_start (i);
         i2c_master_write_byte (i, (co2address << 1), ACK_CHECK_EN);
         i2c_master_write_byte (i, 0x02, ACK_CHECK_EN); // 0202 get reading state
         i2c_master_write_byte (i, 0x02, ACK_CHECK_EN);
         i2c_master_stop (i);
         esp_err_t err = i2c_master_cmd_begin (co2port, i, 10 / portTICK_PERIOD_MS);
         i2c_cmd_link_delete (i);
         if (err)
            ESP_LOGI (TAG, "Tx GetReady %s", esp_err_to_name (err));
         else
         {
            uint8_t buf[3];
            i = i2c_cmd_link_create ();
            i2c_master_start (i);
            i2c_master_write_byte (i, (co2address << 1) + 1, ACK_CHECK_EN);
            i2c_master_read (i, buf, 2, ACK_VAL);
            i2c_master_read_byte (i, buf + 2, NACK_VAL);
            i2c_master_stop (i);
            esp_err_t err = i2c_master_cmd_begin (co2port, i, 10 / portTICK_PERIOD_MS);
            i2c_cmd_link_delete (i);
            if (err)
               ESP_LOGI (TAG, "Rx GetReady %s", esp_err_to_name (err));
            else if ((buf[0] << 8) + buf[1] == 1)
            {
               i = i2c_cmd_link_create ();
               i2c_master_start (i);
               i2c_master_write_byte (i, (co2address << 1), ACK_CHECK_EN);
               i2c_master_write_byte (i, 0x03, ACK_CHECK_EN);   // 0300 Read data
               i2c_master_write_byte (i, 0x00, ACK_CHECK_EN);
               i2c_master_stop (i);
               esp_err_t err = i2c_master_cmd_begin (co2port, i, 10 / portTICK_PERIOD_MS);
               i2c_cmd_link_delete (i);
               if (err)
                  ESP_LOGI (TAG, "Tx GetData %s", esp_err_to_name (err));
               else
               {
                  uint8_t buf[18];
                  i = i2c_cmd_link_create ();
                  i2c_master_start (i);
                  i2c_master_write_byte (i, (co2address << 1) + 1, ACK_CHECK_EN);
                  i2c_master_read (i, buf, 17, ACK_VAL);
                  i2c_master_read_byte (i, buf + 17, NACK_VAL);
                  i2c_master_stop (i);
                  esp_err_t err = i2c_master_cmd_begin (co2port, i, 10 / portTICK_PERIOD_MS);
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
                     if (lastco2 != (int) co2)
                     {
                        lastco2 = (int) co2;
                        revk_info ("co2", "%d", lastco2);
                     }
                     if (lastrh != (int) rh)
                     {
                        lastrh = (int) rh;
                        revk_info ("rh", "%d", lastrh);
                     }
                     if (!num_owb && lasttemp != (int) (t * 10))
                     {          // We use DS18B20 if we have one
                        lasttemp = (int) (t * 10);
                        revk_info ("temp", "%.1f", t);
                     }
                  }
               }
            }
         }
      }
      if (oledport >= 0)
      {                         // OLED
         // TODO
      }
      if (ds18b20 >= 0 && num_owb)
      {                         // DS18B20
         ds18b20_convert_all (owb);
         ds18b20_wait_for_conversion (ds18b20s[0]);
         float readings[MAX_OWB] = { 0 };
         DS18B20_ERROR errors[MAX_OWB] = { 0 };
         for (int i = 0; i < num_owb; ++i)
            errors[i] = ds18b20_read_temp (ds18b20s[i], &readings[i]);
         if (!errors[0] && (int) (readings[0] * 10) != lasttemp)
         {
            lasttemp = (int) (readings[0] * 10);
            revk_info ("temp", "%.1f", readings[0]);
         }
      }
   }
}
