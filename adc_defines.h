#define FOSC 12000000
#define CCLK (FOSC*5)
#define PCLK (CCLK/4)
#define ADCCLK 3000000
#define CLKDIV ((PCLK/ADCCLK)-1)

#define CLKDIV_BITS 8
#define PDN_BIT 21
#define ADC_START_BIT 24
                //defines for ADCR SFR
#define PIN_FUNC2 1
#define AN0_INPUT_PIN PIN_FUNC2
        //defines for ADDR SFR
#define RESULT_BITS 6
#define DONE_BIT 31
