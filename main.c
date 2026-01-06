#include "stm32f429xx.h"

/* ---------- UART FUNCTIONS ---------- */
static void uart_tx(char c)
{
    while (!(USART1->SR & (1 << 7))); // TXE
    USART1->DR = c;
}

static void uart_print(const char *s)
{
    while (*s)
        uart_tx(*s++);
}

static void delay(void)
{
    for (volatile int i = 0; i < 800000; i++);
}

/* ---------- MAIN ---------- */
int main(void)
{
    /* Enable clocks */
    RCC->AHB1ENR |= (1 << 0);   // GPIOA (IR, UART)
    RCC->AHB1ENR |= (1 << 6);   // GPIOG (LED)
    RCC->APB2ENR |= (1 << 4);   // USART1

    /* PA0 = INPUT (IR sensor, active-low) */
    GPIOA->MODER &= ~(3 << (0 * 2));
    GPIOA->PUPDR &= ~(3 << (0 * 2));
    GPIOA->PUPDR |=  (1 << (0 * 2));   // pull-up

    /* PG13 = OUTPUT (Green LED) */
    GPIOG->MODER &= ~(3 << (13 * 2));
    GPIOG->MODER |=  (1 << (13 * 2));

    /* PA9 = USART1 TX (ST-LINK VCP) */
    GPIOA->MODER &= ~(3 << (9 * 2));
    GPIOA->MODER |=  (2 << (9 * 2));   // Alternate Function
    GPIOA->AFR[1] &= ~(0xF << (1 * 4));
    GPIOA->AFR[1] |=  (7 << (1 * 4));  // AF7 = USART1

    /* USART1 config: 9600 baud @16 MHz */
    USART1->BRR = 0x683;
    USART1->CR1 |= (1 << 13) | (1 << 3); 

    uart_print("SYSTEM STARTED\r\n");

    /* ---------- LOOP ---------- */
    while (1)
    {
        if ((GPIOA->IDR & (1 << 0)) == 0)   // IR detected
        {
            GPIOG->ODR |= (1 << 13);        // LED ON
            uart_print("DETECTED\r\n");
        }
        else
        {
            GPIOG->ODR &= ~(1 << 13);       // LED OFF
            uart_print("NONDETECTED\r\n");
        }

        delay(); // 
    }
}
