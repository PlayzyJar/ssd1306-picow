#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "ssd1306.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
// SPI defines - CORRIGIDO para sua placa
#define SPI_PORT spi0
#define PIN_MISO 16 // GP16
#define PIN_CS 17   // GP17
#define PIN_SCK 18  // GP18
#define PIN_MOSI 19 // GP19

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5

// Use pins 4 and 5 for UART1
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
// UART defines - MUDEI PARA OUTROS PINOS
#define UART_ID uart1
#define BAUD_RATE 115200
#define UART_TX_PIN 8 // GP8 (era GP4)
#define UART_RX_PIN 9 // GP9 (era GP5)

ssd1306_t display;

/**
 * @brief Desenha um botão estilizado no display
 * @param x Posição horizontal do canto superior esquerdo
 * @param y Posição vertical do canto superior esquerdo
 * @param width Largura do botão
 * @param height Altura do botão
 * @param text Texto a ser exibido no botão
 */
void draw_button(ssd1306_t *disp, uint8_t x, uint8_t y, uint8_t width, uint8_t height, const char *text)
{
    // Desenha a borda do botão
    ssd1306_fill_rect(disp, x, y, width, height, false);        // Limpa a área
    ssd1306_fill_rect(disp, x, y, width, 1, true);              // Borda superior
    ssd1306_fill_rect(disp, x, y + height - 1, width, 1, true); // Borda inferior
    ssd1306_fill_rect(disp, x, y, 1, height, true);             // Borda esquerda
    ssd1306_fill_rect(disp, x + width - 1, y, 1, height, true); // Borda direita

    // Centraliza o texto no botão
    uint8_t text_x = x + (width - strlen(text) * 6) / 2;
    uint8_t text_y = y + (height - 8) / 2;
    ssd1306_draw_string(disp, text_x, text_y, text);
}

/**
 * @brief Desenha a tela de demonstração com dados dos sensores
 */
void draw_demo_screen(ssd1306_t *disp)
{
    // Limpa o display
    ssd1306_clear(disp);

    // Cabeçalho
    ssd1306_draw_string(disp, 40, 0, "DASHBOARD");
    ssd1306_fill_rect(disp, 0, 9, 128, 1, true); // Linha divisória

    // Dados dos sensores (valores estáticos para demonstração)
    ssd1306_draw_string(disp, 10, 16, "TEMPERATURA:");
    ssd1306_draw_string(disp, 90, 16, "23.5 C");

    ssd1306_draw_string(disp, 10, 28, "UMIDADE:");
    ssd1306_draw_string(disp, 90, 28, "65.2 %");

    ssd1306_draw_string(disp, 10, 40, "PRESSAO:");
    ssd1306_draw_string(disp, 90, 40, "1013 hPa");

    // Linha divisória antes dos botões
    ssd1306_fill_rect(disp, 0, 50, 128, 1, true);

    // Rodapé com 3 botões
    draw_button(disp, 5, 55, 38, 8, "UP");     // Botão esquerdo
    draw_button(disp, 45, 55, 38, 8, "SEL"); // Botão central
    draw_button(disp, 85, 55, 38, 8, "DOWN");      // Botão direito

    // Atualiza o display físico
    ssd1306_display(disp);
}

int main()
{
    stdio_init_all();

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000 * 1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS, GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    // Set up our UART
    uart_init(UART_ID, BAUD_RATE);
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Use some the various UART functions to send out data
    // In a default system, printf will also output via the default UART

    // Send out a string, with CR/LF conversions
    uart_puts(UART_ID, " Hello, UART!\n");

    // For more examples of UART use see https://github.com/raspberrypi/pico-examples/tree/master/uart

    // Inicializar display
    ssd1306_init(&display, I2C_PORT, SSD1306_I2C_ADDR);

    // Desenha a tela de demonstração
    draw_demo_screen(&display);

    while (true)
    {
        printf("Display updated!\n");
        sleep_ms(1000);
    }
}
