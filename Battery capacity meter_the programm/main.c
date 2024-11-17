#include "./STE2007_nokia1202_display_driver/shared_file.h"

//Fatal Error[Pe1696]: cannot open source file "stm32f103xb.h" C:\Users\Pz\Desktop\Education STM_C-C++\0_Testing\CMSIS_learning_2_battery capacity\4.09.2024\Needed files\stm32f1xx.h 131 
//Fatal Error[Pe1696]: cannot open source file "core_cm3.h" C:\Users\Pz\Desktop\Education STM_C-C++\0_Testing\CMSIS_learning_2_battery capacity\4.09.2024\Needed files\stm32f103xb.h 132 
//Fatal Error[Pe1696]: cannot open source file "cmsis_version.h" C:\Users\Pz\Desktop\Education STM_C-C++\0_Testing\CMSIS_learning_2_battery capacity\4.09.2024\Needed files\core_cm3.h 63 
//Fatal Error[Pe1696]: cannot open source file "cmsis_compiler.h" C:\Users\Pz\Desktop\Education STM_C-C++\0_Testing\CMSIS_learning_2_battery capacity\4.09.2024\Needed files\core_cm3.h 115 
//Fatal Error[Pe1696]: cannot open source file "cmsis_iccarm.h" C:\Users\Pz\Desktop\Education STM_C-C++\0_Testing\CMSIS_learning_2_battery capacity\4.09.2024\Needed files\cmsis_compiler.h 55 

//Description
//PA0 for a current
//PA1 for a voltage
//PA9 - as switch

  struct different_varaibles varaibles;//this struct for display_driver function

//varaibles
  //voltages
  float sum_of_voltages;
  
  //current
  unsigned int sum_of_currents;
  
  //different
  unsigned int ADC2_VAL[2];
  
//flags
char ADC2_Flag;
char the_current_has_already_been_recalculated_flag;
char initial_voltage_flag=0;

//functions prototypes
  //System clock config
  void System_Clock_Config(void);

  //PA9
  void GPIO_PA9_Config(void);

  //ADC2
  void ADC2_Config(void);
  void ADC2_Enable(void);
  void ADC2_Start(char channel);

  //TIM1
  void TIM1_Config(void);
  void TIM1_Interrupt_Config(void);
  
  //void user_delay(unsigned long wait);
  
  //For Nokia display
  void GPIO_PB3_PB7_Config(void);
  
//Interrupts
void TIM1_UP_IRQHandler(void)
{
TIM1->SR&=~TIM_SR_UIF;
ADC2_Flag=0;//Current
ADC2_Start(0);
}

void ADC1_2_IRQHandler(void)
{
if(ADC2->SR&ADC_SR_EOC)
{
  if(ADC2_Flag==1)
  {
    ADC2_VAL[1]=ADC2->DR;
    the_current_has_already_been_recalculated_flag=0;
    varaibles.seconds_counter++;
  }else if(ADC2_Flag==0)
  {
    ADC2_VAL[0]=ADC2->DR;
    ADC2_Flag=1;
    ADC2_Start(1);
  }
}
}

void main(void)
{
/* Config*/
//System clock config
System_Clock_Config();

//PA9
GPIO_PA9_Config();

/* For Nokia display*/
GPIO_PB3_PB7_Config();
/* For Nokia display_END*/
  
//ADC2
ADC2_Config();
ADC2_Enable();

//TIM1
TIM1_Config();
TIM1_Interrupt_Config();

/* Switch on*/
GPIOA->BSRR|=GPIO_BSRR_BR9;

ste2007_display_driver(display_initialization, &varaibles);
ste2007_display_driver(clear_all, &varaibles);
ste2007_display_driver(permanent_words, &varaibles);
      
while(4)
{
  if((varaibles.seconds_counter!=0)&&(the_current_has_already_been_recalculated_flag==0))
  {
    the_current_has_already_been_recalculated_flag=1;
    
    //sum_of_voltages
    sum_of_voltages+=(2*ADC2_VAL[1]*8.0383)/10000;//V     2* becase the resistor divider is be
    
    //sum_of_currents
    varaibles.voltage_to_calculate_the_current=(unsigned int)((ADC2_VAL[0]*7.7695)/10);//V?
    sum_of_currents+=(varaibles.voltage_to_calculate_the_current/1);//mA
    varaibles.capacity=sum_of_currents/3600;//mAh
    
    ste2007_display_driver(seconds, &varaibles);
    ste2007_display_driver(current, &varaibles);
    ste2007_display_driver(capacity_word, &varaibles);
      if(varaibles.seconds_counter%8==0)
      {
      //voltage
      varaibles.voltage=sum_of_voltages/8;
      sum_of_voltages=0;
      ste2007_display_driver(voltage_displaying, &varaibles);
      
      if(initial_voltage_flag==0)
      {
        initial_voltage_flag=1;
        ste2007_display_driver(initial_voltage, &varaibles);
      }
      
      //Calculate the capacity
      varaibles.capacity=sum_of_currents/3600;//mAh
      
          if(varaibles.voltage<=2.9)
          {
            /* Switch off*/
            GPIOA->BSRR|=GPIO_BSRR_BS9;
          
            //TIM1 interrupts off
            TIM1->DIER&=~TIM_DIER_UIE;
            
            //Calculate the capacity
            varaibles.capacity=sum_of_currents/3600;//mAh
            
            ste2007_display_driver(final_voltage, &varaibles);
          }
      }
  }
}
}

/* Functions body*/
  //System clock config
  void System_Clock_Config(void)
  {
  //Steps
  //1 Flash latency
  //2 Enable HSE and wait for
  //3 PLL multiplication factor
  //4 PLL entry source clock
  //5 Enable PLL and wait for
  //6 Set the prescalers
    //HPRE
    //PPRE1 is divided by 2
    //PPRE2
  //7 SW and wait for
  
  //1 Flash latency
  FLASH->ACR|=FLASH_ACR_LATENCY_1;
  
  //2 Enable HSE and wait for
  RCC->CR|=RCC_CR_HSEON;
  while(!(RCC->CR&RCC_CR_HSERDY));
  
  //3 PLL multiplication factor
  RCC->CFGR|=RCC_CFGR_PLLMULL9;
  
  //4 PLL entry source clock
  RCC->CFGR|=RCC_CFGR_PLLSRC;
  
  //5 Enable PLL and wait for
  RCC->CR|=RCC_CR_PLLON;
  while(!(RCC->CR&RCC_CR_PLLRDY));
  
  //6 Set the prescalers
    //HPRE
    RCC->CFGR&=~RCC_CFGR_HPRE_DIV1;
    
    //PPRE1 is divided by 2
    RCC->CFGR|=RCC_CFGR_PPRE1_DIV2;
    
    //PPRE2
    RCC->CFGR&=RCC_CFGR_PPRE2_DIV1;
    
  //7 SW and wait for
  RCC->CFGR|=RCC_CFGR_SW_PLL;
  while(!(RCC->CFGR&RCC_CFGR_SW_PLL));
  }

  //PA9
  void GPIO_PA9_Config(void)
  {
  //Steps
  //1 Clock
  //2 Reset
  //3 Mode
  //4 Configurate
  
  //1 Clock
  RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;
  
  //2 Reset
  GPIOA->BSRR=0xffff;
  
  //3 Mode
  GPIOA->CRH|=GPIO_CRH_MODE9;//11 for output mode
  
  //4 Configurate
  GPIOA->CRH&=~GPIO_CRH_CNF9;//00 for general purpose push-pull
  
  }

  //ADC2
  void ADC2_Config(void)//PA0 and PA1
  {
  //Steps
  //1 ADC and GPIO clock
  //2 Set the prescaler
  //3 Data alignment
  //4 Sampling time
  //5 Sequence length
  //6 Analog mode for pins
  
  //1 ADC and GPIO clock
  RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;
  RCC->APB2ENR|=RCC_APB2ENR_ADC2EN;
  
  //2 Set the prescaler
  RCC->CFGR|=RCC_CFGR_ADCPRE_DIV6;
  
  //3 Data alignment
  ADC2->CR2&=~ADC_CR2_ALIGN;//0 for right alignment
  
  //4 Sampling time
  ADC2->SMPR2|=ADC_SMPR2_SMP0;
  ADC2->SMPR2|=ADC_SMPR2_SMP1;
  
  //5 Sequence length
  ADC2->SQR1&=~ADC_SQR1_L;//0000 for 1 conversion
  
  //6 Analog mode for pins
  //PA0
  GPIOA->CRL&=~GPIO_CRL_MODE0;//00 for input mode
  GPIOA->CRL&=~GPIO_CRL_CNF0;//00 for analog mode
  
  //PA1
  GPIOA->CRL&=~GPIO_CRL_MODE1;//00 for input mode
  GPIOA->CRL&=~GPIO_CRL_CNF1;//00 for analog mode
  }
  
  void ADC2_Enable(void)
  {
  //Steps
  //1 Calibration
  //2 Enable the ADC2
  //3 Enable the EOC interrupt
  //4 Set interrupts priority
  //5 Enable the interrupt
  
  //1 Calibration
  ADC2->CR2|=ADC_CR2_ADON;
  user_delay(500);
  ADC2->CR2|=ADC_CR2_CAL;
  while(ADC2->CR2&ADC_CR2_CAL);
  
  //2 Enable the ADC2
  //ADC2->CR2|=ADC_CR2_ADON;
  
  //3 Enable the EOC interrupt
  ADC2->CR1|=ADC_CR1_EOCIE;
  
  //4 Set interrupts priority
  NVIC_SetPriority(ADC1_2_IRQn, 1);
  
  //5 Enable the interrupt
  NVIC_EnableIRQ(ADC1_2_IRQn);
  }

  void ADC2_Start(char channel)
  {
  //Steps
  //1 Reset the status register
  //2 Select the channel
  //3 Start the conversion
  
  //1 Reset the status register
  ADC2->SR=0;
  
  //2 Select the channel
  ADC2->SQR3=0;
  ADC2->SQR3|=channel;
  
  //3 Start the conversion
  ADC2->CR2|=ADC_CR2_ADON;
  }

  //TIM1
  void TIM1_Config(void)
  {
  //Steps
  //1 Clock
  //2 Prescaler
  //3 ARR
  //4 Enable ARPE
  //5 Enable the timer counter
  
  //1 Clock
  RCC->APB2ENR|=RCC_APB2ENR_TIM1EN;
  
  //2 Prescaler
  TIM1->PSC=7200-1;
  
  //3 ARR
  TIM1->ARR=10000-1;
  
  //4 Enable ARPE
  TIM1->CR1|=TIM_CR1_ARPE;
  
  //5 Enable the timer counter
  TIM1->CR1|=TIM_CR1_CEN;
  }

  void TIM1_Interrupt_Config(void)
  {
  //Steps
  //1 Enable AFIOEN
  //2 Enable the UIE
  //3 Set the interrupt priority
  //4 Enable interrupts
  
  //1 Enable AFIOEN
  RCC->CR|=RCC_APB2ENR_AFIOEN;//Alternative function clock on
  
  //2 Enable the UIE
  TIM1->DIER|=TIM_DIER_UIE;
  
  //3 Set the interrupt priority
  NVIC_SetPriority(TIM1_UP_IRQn, 1);
  
  //4 Enable interrupts
  NVIC_EnableIRQ(TIM1_UP_IRQn);
  }

void GPIO_PB3_PB7_Config(void)
{
  //Steps
  //1 Clock
  //2 Reset
  //3 Mode
  //4 Configurate
  
  //for PB4 and PB3 enabling RCC->AFIOEN=1 AFIO->MAPR->SWJ_CFG = 0x2
  RCC->APB2ENR|=RCC_APB2ENR_AFIOEN;
  AFIO->MAPR|=AFIO_MAPR_SWJ_CFG_1;//0x2
  
  //1 Clock
  RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;
  
  //2 Reset
  GPIOB->BRR=0xffff;
  
  //3 Mode
  GPIOB->CRL|=GPIO_CRL_MODE3;//11 for output mode
  GPIOB->CRL|=GPIO_CRL_MODE4;//11 for output mode
  GPIOB->CRL|=GPIO_CRL_MODE5;//11 for output mode  
  GPIOB->CRL|=GPIO_CRL_MODE6;//11 for output mode
  GPIOB->CRL|=GPIO_CRL_MODE7;//11 for output mode
  
  //4 Configurate
  GPIOB->CRL&=~GPIO_CRL_CNF3;//00 for general purpose push-pull
  GPIOB->CRL&=~GPIO_CRL_CNF4;//00 for general purpose push-pull
  GPIOB->CRL&=~GPIO_CRL_CNF5;//00 for general purpose push-pull
  GPIOB->CRL&=~GPIO_CRL_CNF6;//00 for general purpose push-pull
  GPIOB->CRL&=~GPIO_CRL_CNF7;//00 for general purpose push-pull
}
/* For Nokia display_END*/