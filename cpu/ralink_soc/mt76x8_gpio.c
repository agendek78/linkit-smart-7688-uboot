#include <common.h>
#include <command.h>

#include "asm/string.h"
#include "asm/types.h"
#include "rt_mmap.h"
#include "configs/rt2880.h"

#define RALINK_GPIO_DIR_IN		0
#define RALINK_GPIO_DIR_OUT		1

static u8	pinmuxConf[] =
{
	6, //I2S_SDI
	6, //I2S_SDO
	6, //I2S_WS
	6, //I2S_CLK
	20, //I2C_SCLK
	20, //I2C_SD
	4, //SPI_CS1
	12, //SPI_CLK
	12, //SPI_MOSI
	12, //SPI_MISO
	12, //SPI_CS0
	0, //GPIO0
	8, //UART_TXD0
	8, //UART_RXD0
	2, //MDI_TP_P1
	2, //MDI_TN_P1
	2, //MDI_RP_P1
	2, //MDI_RN_P1
	28, //MDI_RP_P2
	30, //MDI_RN_P2
	26, //MDI_TP_P2
	26, //MDI_TN_P2
	10, //MDI_TP_P3
	10, //MDI_TN_P3
	10, //MDI_RP_P3
	10, //MDI_RN_P3
	10, //MDI_RP_P4
	10, //MDI_RN_P4
	10, //MDI_TP_P4
	10, //MDI_TN_P4
	26, //P4_LED_KN
	24, //P3_LED_KN
	22, //P2_LED_KN
	20, //P1_LED_KN
	18, //P0_LED_KN
	16, //WLED_KN
	16, //PERST_N
	18, //REF_CLKO
	14, //WDT_RST_N
	10, //P4_LED_AN
	8, //P3_LED_AN
	6, //P2_LED_AN
	4, //P1_LED_AN
	2, //P0_LED_AN
	0, //WLED_AN
	24, //UART_TXD1
	24, //UART_RXD1
};

void PINMUX_Set(int pin, int value)
{
	u32 tmp;
	
	if ((pin >= 30 && pin <= 35) ||
		(pin >= 39 && pin <= 44))
	{
		tmp = RALINK_REG(RT2880_GPIOMODE_REG + 4);
		tmp &= ~(0x03u << pinmuxConf[pin]);
		tmp |= (value & 0x03) << pinmuxConf[pin];
		RALINK_REG(RT2880_GPIOMODE_REG + 4) = tmp;
	}
	else
	{
		tmp = RALINK_REG(RT2880_GPIOMODE_REG);
		tmp &= ~(0x03u << pinmuxConf[pin]);
		tmp |= (value & 0x03) << pinmuxConf[pin];
		RALINK_REG(RT2880_GPIOMODE_REG) = tmp;		
	}
}

int GPIO_Get(int pin)
{
	u32 tmp = 0;

	if (pin <= 31)  
	{
		tmp = RALINK_REG(RT2880_REG_PIODATA);
		tmp = (tmp >> pin) & 1u;
	} 
	else if (pin <= 63) 
	{
		tmp = RALINK_REG(RT2880_REG_PIODATA + 4);
		tmp = (tmp >> (pin-32)) & 1u;
	} 
	else if (pin <= 95) 
	{
		tmp = RALINK_REG(RT2880_REG_PIODATA + 8);
		tmp = (tmp >> (pin-64)) & 1u;
	}

	return tmp;
}

void GPIO_Set(int pin, int value)
{
	u32 tmp;

	if (pin <= 31) 
	{
		tmp = (1u << pin);
		if (value)
		{
			RALINK_REG(RT2880_REG_PIOSET) = tmp;
		}
		else
		{
			RALINK_REG(RT2880_REG_PIORESET) = tmp;
		}
	} 
	else if (pin <= 63) 
	{
		tmp = (1u << (pin-32));
		if (value)
		{
			RALINK_REG(RT2880_REG_PIOSET + 4) = tmp;
		}
		else
		{
			RALINK_REG(RT2880_REG_PIORESET + 4) = tmp;
		}
	} 
	else if (pin <= 95) 
	{
		tmp = (1u << (pin-64));
		if (value)
		{
			RALINK_REG(RT2880_REG_PIOSET + 8) = tmp;
		}
		else
		{
			RALINK_REG(RT2880_REG_PIORESET + 8) = tmp;
		}
	}
}

void GPIO_Configure(int pin, int is_output)
{
	u32 tmp;

	if (pin <= 31) 
	{
		tmp = RALINK_REG(RT2880_REG_PIODIR);
		if (is_output)
			tmp |=  (1u << pin);
		else
			tmp &= ~(1u << pin);
		RALINK_REG(RT2880_REG_PIODIR) = tmp;
	} 
	else if (pin <= 63) 
	{
		tmp = RALINK_REG(RT2880_REG_PIODIR + 4);
		if (is_output)
			tmp |=  (1u << (pin-32));
		else
			tmp &= ~(1u << (pin-32));
		RALINK_REG(RT2880_REG_PIODIR + 4) = tmp;
	} 
	else if (pin <= 95) 
	{
		tmp = RALINK_REG(RT2880_REG_PIODIR + 8);
		if (is_output)
			tmp |=  (1u << (pin-64));
		else
			tmp &= ~(1u << (pin-64));
		RALINK_REG(RT2880_REG_PIODIR + 8) = tmp;
	}
}

int do_setgpio(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int rcode = 0;
	
	do
	{
		u32	p1, p2;
		char*	ep;
		
		if (argc < 1)
		{
			printf("Select option: set, get, dir, mux\n");
			rcode = -1;
			break;
		}
		
		if (argc >= 3)
		{
			p1 = simple_strtoul(argv[2], &ep, 10);
			if (argv[2] == ep || *ep != '\0')
			{
				printf("p2: convert error!\n");
				break;
			}
		}

		if (argc >= 4)
		{
			p2 = simple_strtoul(argv[3], &ep, 10);
			if (argv[3] == ep || *ep != '\0')
			{
				printf("p1: convert error!\n");
				break;
			}
		}
		
		if (strcmp("set", argv[1]) == 0)
		{
			printf("Setting GPIO%02d = %d\n", p1, p2);
			GPIO_Set(p1, p2);
		}
		else if (strcmp("get", argv[1]) == 0)
		{
		}
		else if (strcmp("dir", argv[1]) == 0)
		{
		}
		else if (strcmp("mux", argv[1]) == 0)
		{
	
		}
		else
		{
			printf("Unknown option: %s\n", argv[1]);
			rcode = -1;
			break;
		}
		
	} while (0);
	
	return rcode;
}

U_BOOT_CMD(
 	gpio,	4,	1,	do_setgpio,
 	"gpio - gpio operations\n",
	"[set] num val\n" \
	"[get] num\n" \
	"[dir] num [in/out]\n" \
	"[mux] num val\n"
);

