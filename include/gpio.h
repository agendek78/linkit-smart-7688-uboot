#ifndef GPIO_MT76x8_H
#define GPIO_MT76x8_H

#ifdef __cplusplus
extern "C"
{
#endif


int GPIO_Get(int pin);
void GPIO_Set(int pin, int value);
void GPIO_Configure(int pin, int is_output);
void PINMUX_Set(int pin, int value);

#ifdef __cplusplus
}
#endif

#endif
