#include "com.h"

int com_open_and_autoconfig (const char *port_name, uint32_t *speed)
{
    struct sp_port *port;
    if (sp_get_port_by_name (port_name, &port) != SP_OK)
    {
        printf ("Can't find port by name: %s\n", port_name);
        return -1;
    }

    if (sp_open (port, SP_MODE_READ_WRITE) != SP_OK)
    {
        printf ("Can't open port: %s\n", port_name);
        return -1;
    }

    sp_set_baudrate (port, *speed);
	sp_set_bits (port, 8);
	sp_set_parity (port, SP_PARITY_NONE);
	sp_set_stopbits (port, 1);
	sp_set_flowcontrol (port, SP_FLOWCONTROL_NONE);

    return 0;

}

void com_check (struct sp_port *port)
{

    while (sp_input_waiting (port) < 5)
    {
        sleep (1);
    }

}

int com_read (struct sp_port *port, char *buffer)
{

    if (sp_blocking_read (port, buffer, sizeof (buffer), 1000) != sizeof (buffer))
    {
        printf ("Error while reading COM");
        return -1;
    }

    return 0;

}