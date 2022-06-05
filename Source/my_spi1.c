#include "my_spi1.h"

void spiPinsInit(void){
    // init PA15 pins - NSS(CS)
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOAEN, ENABLE);
    GPIO_InitTypeDef GPIOA_ini = {0};
    
    //GPIOA_ini.GPIO_Pin = SPI1_NSS_PIN;
    GPIOA_ini.GPIO_Mode = GPIO_Mode_AF;
    GPIOA_ini.GPIO_Speed = GPIO_Speed_10MHz;   // 2, 10, 50 ÌÃö
    GPIOA_ini.GPIO_OType = GPIO_OType_PP;
    GPIOA_ini.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
    GPIO_Init(GPIOA, &GPIOA_ini);
    //GPIO_WriteBit(GPIOA, SPI1_NSS_PIN, Bit_RESET);
    
    // init PB3 - SCK, PB5 - MOSI and PB4 - MISO pins
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOBEN, ENABLE);
    GPIO_InitTypeDef GPIOB_ini = {0};
    
    //GPIOB_ini.GPIO_Pin = SPI1_SCK_PIN | SPI1_MISO_PIN | SPI1_MOSI_PIN;
    GPIOB_ini.GPIO_Mode = GPIO_Mode_AF;
    GPIOB_ini.GPIO_Speed = GPIO_Speed_10MHz;   // 2, 10, 50 ÌÃö
    GPIOB_ini.GPIO_OType = GPIO_OType_PP;
    GPIOB_ini.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
    GPIO_Init(GPIOB, &GPIOB_ini);
    
    // init rst pin
    //GPIOB_ini.GPIO_Pin = RST_PIN;
    GPIOB_ini.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOB, &GPIOB_ini);
    GPIO_WriteBit(GPIOB, RST_PIN, Bit_RESET);
    
    // spi init
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    SPI_InitTypeDef spi_ini;
    spi_ini.SPI_Mode = SPI_Mode_Master;
    spi_ini.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    spi_ini.SPI_DataSize = SPI_DataSize_8b;
    spi_ini.SPI_CPOL = SPI_CPOL_High;
    spi_ini.SPI_CPHA = SPI_CPHA_2Edge;
    spi_ini.SPI_NSS = SPI_NSS_Soft;
    spi_ini.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    spi_ini.SPI_FirstBit = SPI_FirstBit_MSB;
    spi_ini.SPI_CRCPolynomial = 10;
    
    SPI_Init(SPI1, &spi_ini);
}

