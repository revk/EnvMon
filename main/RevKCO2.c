// CO2 + other sensors all

#include "revk.h"

const char     *
app_command(const char *tag, unsigned int len, const unsigned char *value)
{
	return "";
}

void
app_main()
{
	revk_init(&app_command);
}
