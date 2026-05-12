/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "fonts.h"
#define LCD_WIDTH  80  // Landscape
#define LCD_HEIGHT 260  // Landscape
#define X_OFFSET   1
#define Y_OFFSET   26
#define ST7735_CS_GPIO_Port  GPIOB
#define ST7735_CS_Pin        GPIO_PIN_14
#define ST7735_DC_GPIO_Port  GPIOB
#define ST7735_DC_Pin        GPIO_PIN_13
#define ST7735_RES_GPIO_Port GPIOB
#define ST7735_RES_Pin       GPIO_PIN_15
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
struct Node {
    int x;
    int y;
    struct Node* next;
};
struct Node *starList = NULL;
int score = 0;
int Delay = 500;
uint8_t starsize = 5;
void LCD_Init(void);
void Lcdclose(void);
void LcdOpen(void);
void LCD_DataMode(uint8_t pin);
void LCD_CommandMode(uint8_t data);

void LCD_Fill(uint16_t color);
void LCD_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void DrawChar(uint16_t x, uint16_t y, char ch, sFONT font, uint16_t color ,uint16_t bgcolor);
void DrawString(uint16_t x, uint16_t y,  char* str, sFONT font, uint16_t color,uint16_t bgcolor);


void movingStars(void);
void EraseArea(uint16_t x, uint16_t y);
struct Node* createStar( uint32_t startrandomX, uint32_t endrandomX);
void PrintStars(struct Node *head);
void moveStar(struct Node *head);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();
  LCD_Init();
  MX_USART2_UART_Init();
  LCD_Fill(0x0010);
  uint16_t GAMEON =0;

  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  char scoreBuf[10];
	  sprintf(scoreBuf, "%d", score);

    /* USER CODE END WHILE */
	  if( GAMEON == 0){ DrawString(60, 5, "press C to start", Font12, 0xFFFF,0x0010);}else{



		  movingStars();
		  DrawString(90, 35, scoreBuf, Font8, 0xFFFF, 0x0011);
			score += 1;
			HAL_Delay(Delay);
			Delay -= 1;
	  }

	  if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10) == GPIO_PIN_RESET) {
		  GAMEON = 1;
		  LCD_Fill(0x0010);
		 DrawString( 90, 5, "score", Font8, 0xFFFF,0x0010);
	  	      }
	  /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB13 PB14 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


#ifdef __GNUC__
  /* With GCC, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART2 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

// Also add this for newer GCC versions to handle the full write stream
int _write(int file, char *ptr, int len)
{
  HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, 0xFFFF);
  return len;
}
void LcdOpen(void){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
}
void Lcdclose(void){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
}

void LCD_Init(void) {//GEMINI
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
    HAL_Delay(50);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
    HAL_Delay(150);

    LCD_CommandMode(0x01); // Reset
    HAL_Delay(150);
    LCD_CommandMode(0x11); // Wake
    HAL_Delay(200);
    LCD_CommandMode(0x3A); // Color mode
    LCD_DataMode(0x05);    // 16-bit
    LCD_CommandMode(0x21);

    LCD_CommandMode(0x29); // On
    HAL_Delay(100);
}


void LCD_CommandMode(uint8_t data) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);// command
    LcdOpen();
    HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
    Lcdclose();
}


void LCD_DataMode(uint8_t pin) {
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);//color
   LcdOpen();
   HAL_SPI_Transmit(&hspi1, &pin, 1, HAL_MAX_DELAY);
   Lcdclose();
}


void LCD_Fill(uint16_t color) {
    LCD_CommandMode(0x2A);
    LCD_DataMode(0x00); LCD_DataMode(0x00); LCD_DataMode(0x00); LCD_DataMode(0x7F);
    LCD_CommandMode(0x2B);
    LCD_DataMode(0x00); LCD_DataMode(0x00); LCD_DataMode(0x00); LCD_DataMode(0x9F);
    LCD_CommandMode(0x2C);

    uint8_t data[] = {color & 0xFF, color >> 8};
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
    LcdOpen();
    for (uint32_t i = 0; i < (LCD_WIDTH * LCD_HEIGHT); i++) {
        HAL_SPI_Transmit(&hspi1, data, 2, HAL_MAX_DELAY);
    }
    Lcdclose();
}


void LCD_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){
	 LCD_CommandMode(0x2A);
	 uint8_t dataX[] ={ 0x00, x0 + 1, 0x00, x1 + 1 };
	 for(int i=0; i<4; i++){

		 LCD_DataMode(dataX[i]);

	 }
	 LCD_CommandMode(0x2B);
	 uint8_t dataY[] ={ 0x00, y0 + 1, 0x00, y1 + 1 };
	 for(int i=0; i<4; i++){

		 LCD_DataMode(dataY[i]);

	 }
	 LCD_CommandMode(0x2C);
}


void DrawPixel(uint16_t x, uint16_t y, uint16_t color){
	   LCD_SetAddressWindow(x, y, x, y);
	   uint8_t data[] = {color & 0xFF, color >> 8};
	      LCD_DataMode(data[0]);
	      LCD_DataMode(data[1]);

}

void DrawChar(uint16_t x, uint16_t y, char ch, sFONT font, uint16_t color ,uint16_t bgcolor) {
    uint16_t bytesPerRow = (font.Width + 7) / 8;
    uint16_t JumpDistance = bytesPerRow * font.Height;
    uint32_t charLocation = (ch - 32) * JumpDistance;


    for(int i = 0; i < font.Height; i++) {
        for(int j = 0; j < font.Width; j++) {

            uint8_t byte = font.table[charLocation + (i * bytesPerRow) + (j / 8)];

            if (byte & (0x80 >> (j % 8))) {

                DrawPixel(x + (font.Height - 1 - i), y + j, color);
            } else{
            	DrawPixel(x + (font.Height - 1 - i), y + j, bgcolor);
            }
        }
    }
}

void DrawString(uint16_t x, uint16_t y, char* str, sFONT font, uint16_t color,uint16_t bgcolor) {


    while(*str) {
        DrawChar(x, y, *str, font, color,bgcolor);
        y += font.Width;
        str++;
    }
}




void movingStars(){
	if ((rand() % 10) == 0) {
	        struct Node *newStar = createStar(rand() % 85, 0);
	        if (newStar != NULL) {
	            newStar->next = starList;
	            starList = newStar;
	        }
	    }moveStar(starList);



	}

struct Node* createStar( uint32_t startrandomX, uint32_t endrandomX){

	 struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
	 if (newNode == NULL) return NULL;
			 newNode->y =0;
			 newNode->x =startrandomX;;
			 newNode->next = NULL;
			 return newNode;
}




void moveStar(struct Node *head){
	struct Node *temp = head;
	while (temp != NULL){
		temp->y += 5;
		temp = temp->next;
	}
	PrintStars(head);
}

void PrintStars(struct Node *head){


	uint8_t data[] = { (uint8_t)(0xFFFF >> 8), (uint8_t)(0xFF & 0xFF) };
	struct Node *temp = head;

	while(temp != NULL){
		EraseArea(temp->x, temp->y);
	    // Set Column Address
	    LCD_CommandMode(0x2A);
	    LCD_DataMode(0x00); LCD_DataMode(temp->x);
	    LCD_DataMode(0x00); LCD_DataMode((temp->x)+starsize-1);

	    // Set Row Address
	    LCD_CommandMode(0x2B);
	    LCD_DataMode(0x00); LCD_DataMode(temp->y);
	    LCD_DataMode(0x00); LCD_DataMode((temp->y)+starsize-1);

	    LCD_CommandMode(0x2C); // RAM Write

	    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
	    LcdOpen();
	    for(int i = 0; i < (starsize * starsize); i++) {
	        HAL_SPI_Transmit(&hspi1, data, 2, HAL_MAX_DELAY);
	    }
	    Lcdclose();

	    temp = temp->next;
	}

}

void EraseArea(uint16_t x, uint16_t y){


	uint8_t data[] = { 0x10, 0x00 };
	// Set Column Address
	    LCD_CommandMode(0x2A);
	    LCD_DataMode(0x00); LCD_DataMode(x );
	    LCD_DataMode(0x00); LCD_DataMode((x + starsize - 1));

	    // Set Row Address
	    LCD_CommandMode(0x2B);
	    LCD_DataMode(0x00); LCD_DataMode(y-6);
	    LCD_DataMode(0x00); LCD_DataMode((y + starsize - 6) );

	    LCD_CommandMode(0x2C); // RAM Write


	    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
	    LcdOpen();
	    for(int i = 0; i < (starsize * starsize); i++) {
	        HAL_SPI_Transmit(&hspi1, data, 2, HAL_MAX_DELAY);
	    }
	    Lcdclose();



}











/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
