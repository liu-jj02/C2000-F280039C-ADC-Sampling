////#############################################################################
////
//// FILE:   adc_ex1_soc_software.c
////
//// TITLE:  ADC Software Triggering
////
////! \addtogroup driver_example_list
////! <h1>ADC Software Triggering</h1>
////!
////! This example converts some voltages on ADCA and ADCC based on a software
////! trigger.
////!
////! The ADCC will not convert until ADCA is complete, so the ADCs will not run
////! asynchronously. However, this is much less efficient than allowing the ADCs
////! to convert synchronously in parallel (for example, by using an ePWM
////! trigger).
////!
////! \b External \b Connections \n
////!  - A0, A1, C2, and C3 should be connected to signals to convert
////!
////! \b Watch \b Variables \n
////! - \b myADC0Result0 - Digital representation of the voltage on pin A0
////! - \b myADC0Result1 - Digital representation of the voltage on pin A1
////! - \b myADC1Result0 - Digital representation of the voltage on pin C2
////! - \b myADC1Result1 - Digital representation of the voltage on pin C3
////!
////
////#############################################################################
////
////
//// $Copyright:
//// Copyright (C) 2024 Texas Instruments Incorporated - http://www.ti.com/
////
//// Redistribution and use in source and binary forms, with or without
//// modification, are permitted provided that the following conditions
//// are met:
////
////   Redistributions of source code must retain the above copyright
////   notice, this list of conditions and the following disclaimer.
////
////   Redistributions in binary form must reproduce the above copyright
////   notice, this list of conditions and the following disclaimer in the
////   documentation and/or other materials provided with the
////   distribution.
////
////   Neither the name of Texas Instruments Incorporated nor the names of
////   its contributors may be used to endorse or promote products derived
////   from this software without specific prior written permission.
////
//// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//// $
////#############################################################################
////
//
////
//// Included Files
////
//#include "driverlib.h"
//#include "device.h"
//#include "board.h"
//
////
//// Globals
////
//uint16_t myADC0Result0;
//uint16_t myADC0Result1;
//uint16_t myADC1Result0;
//uint16_t myADC1Result1;
//
////
//// Main
////
//void main(void)
//{
//    //
//    // Initialize device clock and peripherals
//    //
//    Device_init();
//
//    //
//    // Disable pin locks and enable internal pullups.
//    //
//    Device_initGPIO();
//
//    //
//    // Initialize PIE and clear PIE registers. Disables CPU interrupts.
//    //
//    Interrupt_initModule();
//
//    //
//    // Initialize the PIE vector table with pointers to the shell Interrupt
//    // Service Routines (ISR).
//    //
//    Interrupt_initVectorTable();
//
//    //
//    // Set up ADCs, initializing the SOCs to be triggered by software
//    //
//    Board_init();
//
//    //
//    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
//    //
//    EINT;
//    ERTM;
//
//    //
//    // Loop indefinitely
//    //
//    while(1)
//    {
//        //
//        // Convert, wait for completion, and store results
//        //
//        ADC_forceMultipleSOC(myADC0_BASE, (ADC_FORCE_SOC0 | ADC_FORCE_SOC1));
//
//        //
//        // Wait for ADCA to complete, then acknowledge flag
//        //
//        while(ADC_getInterruptStatus(myADC0_BASE, ADC_INT_NUMBER1) == false)
//        {
//        }
//        ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER1);
//
//        ADC_forceMultipleSOC(myADC1_BASE, (ADC_FORCE_SOC0 | ADC_FORCE_SOC1));
//        //
//        // Wait for ADCC to complete, then acknowledge flag
//        //
//        while(ADC_getInterruptStatus(myADC1_BASE, ADC_INT_NUMBER1) == false)
//        {
//        }
//        ADC_clearInterruptStatus(myADC1_BASE, ADC_INT_NUMBER1);
//
//        //
//        // Store results
//        //
//        myADC0Result0 = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);
//        myADC0Result1 = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER1);
//        myADC1Result0 = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER0);
//        myADC1Result1 = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER1);
//
//        //
//        // Software breakpoint. At this point, conversion results are stored in
//        // myADC0Result0, myADC0Result1, myADC1Result0, and myADC1Result1.
//        //
//        // Hit run again to get updated conversions.
//        //
//        ESTOP0;
//    }
//}


////用 CPU 主动触发一次 ADC，读取 ADCINA6 上的电压，并在 Expressions 中看到“原始码值”和“换算电压”。
//
//#include "driverlib.h"
//#include "device.h"
//
//volatile uint16_t adcCode    = 0;
//volatile float    adcVoltage = 0.0f;
//
//void initADC_A6(void)
//{
//    /* 1. 使用内部 3.3V 参考 */
//    ADC_setVREF(ADCA_BASE,ADC_REFERENCE_INTERNAL,ADC_REFERENCE_3_3V);
//
//    /* 2. ADC 时钟先分频 */
//    ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_4_0);
//
//    /* 3. 转换结束时产生完成信号 */
//    ADC_setInterruptPulseMode(ADCA_BASE, ADC_PULSE_END_OF_CONV);
//
//    /* 4. 给 ADC 上电，并等待它稳定 */
//    ADC_enableConverter(ADCA_BASE);
//    DEVICE_DELAY_US(1000);
//
//    /* 5. 配置 SOC0：
//       软件触发
//       ADCINA6 通道
//       采样窗口 10 个 ADCCLK 周期
//    */
//    ADC_setupSOC(ADCA_BASE,ADC_SOC_NUMBER0,ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN6,10);
//
//    /* 6. SOC0 转换完成后，设置 ADCINT1 标志 */
//    ADC_setInterruptSource(ADCA_BASE,ADC_INT_NUMBER1,ADC_SOC_NUMBER0);
//
//    ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER1);
//
//    /* 清除可能残留的旧完成标志 */
//    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
//}
//
//uint16_t readADC_A6(void)
//{
//    uint16_t result;
//
//    /* CPU 主动发起一次转换 */
//    ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER0);
//
//    /* 等待转换完成 */
//    while(ADC_getInterruptStatus(ADCA_BASE,ADC_INT_NUMBER1) == false)
//    {
//
//    }
//
//    /* 清完成标志，为下一次转换做准备 */
//    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
//
//    /* 读取 SOC0 对应的结果寄存器 */
//    result = ADC_readResult(ADCARESULT_BASE,ADC_SOC_NUMBER0);
//
//    return result;
//}
//
//void main(void)
//{
//    /* 时钟、外设时钟、关闭看门狗 */
//    Device_init();
//
//    /* 解除 GPIO 配置锁 */
//    Device_initGPIO();
//
//    /* PIE 中断相关初始化 */
//    Interrupt_initModule();
//    Interrupt_initVectorTable();
//
//    /* 配置 ADCA + SOC0 + ADCINA6 */
//    initADC_A6();
//
//    while(1)
//    {
//        adcCode = readADC_A6();
//
//        /* 12 位 ADC，3.3V 参考 */
//        adcVoltage = (float)adcCode / 4096.0f * 3.3f;
//
//        /* 每次转换后停下来，方便在 Expressions 观察 */
//        ESTOP0;
//    }
//}


////连续读取 128 个 ADC 数据，算出平均值，确认 0 V 和 3.3 V 两个端点，并用 CCS Graph 把数组画出来。
//
//#include "driverlib.h"
//#include "device.h"
//
//#define ADC_N  128
//
///* 这一组是 128 个 ADC 原始码值 */
//volatile uint16_t adcBuffer[ADC_N];
//
///* 和、平均值、最大最小值 */
//volatile uint32_t adcSum          = 0;
//volatile uint16_t adcAverageCode  = 0;
//volatile uint16_t adcMinCode      = 0;
//volatile uint16_t adcMaxCode      = 0;
//
///* 便于在 Expressions 中间接观察 */
//volatile uint16_t adcCode         = 0;
//volatile float    adcAverageVolt  = 0.0f;
//volatile float    adcLastVolt     = 0.0f;
//
//
//void initADC_A6(void)
//{
//    /* 使用内部 3.3 V 参考 */
//    ADC_setVREF(ADCA_BASE,ADC_REFERENCE_INTERNAL, ADC_REFERENCE_3_3V);
//
//    /* ADC 时钟分频，先沿用官方例程设置 */
//    ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_4_0);
//
//    /* 转换结束时产生完成信号 */
//    ADC_setInterruptPulseMode(ADCA_BASE,ADC_PULSE_END_OF_CONV);
//
//    /* ADC 上电，并等待约 1 ms */
//    ADC_enableConverter(ADCA_BASE);
//    DEVICE_DELAY_US(1000);
//
//    /*
//     * SOC0：
//     * 软件触发；
//     * 使用 ADCINA6；
//     * 采样窗口先设为 10 个 ADCCLK 周期。
//     */
//    ADC_setupSOC(ADCA_BASE,ADC_SOC_NUMBER0,ADC_TRIGGER_SW_ONLY,ADC_CH_ADCIN6,10);
//
//    /* SOC0 完成后设置 ADCINT1 标志 */
//    ADC_setInterruptSource(ADCA_BASE,ADC_INT_NUMBER1,ADC_SOC_NUMBER0);
//
//    ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER1);
//    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
//}
//
//
//uint16_t readADC_A6(void)
//{
//    uint16_t result;
//
//    /* 软件发起一次转换 */
//    ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER0);
//
//    /* 等待本次转换完成 */
//    while(ADC_getInterruptStatus(ADCA_BASE,ADC_INT_NUMBER1) == false)
//    {
//    }
//
//    /* 清完成标志，为下一次转换做准备 */
//    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
//
//    /* 读取 SOC0 的结果寄存器 */
//    result = ADC_readResult(ADCARESULT_BASE,ADC_SOC_NUMBER0);
//
//    return result;
//}
//
//
//void main(void)
//{
//    uint16_t i;
//
//    Device_init();
//    Device_initGPIO();
//
//    Interrupt_initModule();
//    Interrupt_initVectorTable();
//
//    initADC_A6();
//
//    while(1)
//    {
//        adcSum     = 0;
//        adcMinCode = 4095;
//        adcMaxCode = 0;
//
//        for(i = 0; i < ADC_N; i++)
//        {
//            /* 读取一个原始码值 */
//            adcCode = readADC_A6();
//
//            /* 保存到数组，CCS Graph 要使用它 */
//            adcBuffer[i] = adcCode;
//
//            /* 累加，用于求平均 */
//            adcSum += adcCode;
//
//            /* 记录最小值和最大值 */
//            if(adcCode < adcMinCode)
//            {
//                adcMinCode = adcCode;
//            }
//
//            if(adcCode > adcMaxCode)
//            {
//                adcMaxCode = adcCode;
//            }
//        }
//
//        /* 128 个码值求平均 */
//        adcAverageCode = (uint16_t)(adcSum / ADC_N);
//
//        /* 平均值换算成电压 */
//        adcAverageVolt = (float)adcAverageCode / 4096.0f * 3.3f;
//
//        /* 最后一个元素的电压，便于对照 */
//        adcLastVolt = (float)adcCode / 4096.0f * 3.3f;
//
//        /* 一批采样结束后暂停，方便看数组 */
//        ESTOP0;
//    }
//}



//比较 16、128、256 个点的平均值

#include "driverlib.h"
#include "device.h"

#define ADC_N  256

/* 保存 256 个 ADC 原始数据 */
volatile uint16_t adcBuffer[ADC_N];

/* 总和及整体最大、最小值 */
volatile uint32_t adcSum      = 0;
volatile uint16_t adcMinCode  = 0;
volatile uint16_t adcMaxCode  = 0;

/* 三种平均点数对应的平均码值 */
volatile float adcAvgCode16   = 0.0f;
volatile float adcAvgCode128  = 0.0f;
volatile float adcAvgCode256  = 0.0f;

/* 三种平均码值对应的电压 */
volatile float adcAvgVolt16   = 0.0f;
volatile float adcAvgVolt128  = 0.0f;
volatile float adcAvgVolt256  = 0.0f;

/* 最后一次采样的码值和电压 */
volatile uint16_t adcCode     = 0;
volatile float    adcLastVolt = 0.0f;


void initADC_A6(void)
{
    /* 内部 3.3 V 参考 */
    ADC_setVREF(ADCA_BASE,
                ADC_REFERENCE_INTERNAL,
                ADC_REFERENCE_3_3V);

    /* ADC 时钟分频 */
    ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_4_0);

    /* 转换结束时设置完成信号 */
    ADC_setInterruptPulseMode(ADCA_BASE,
                              ADC_PULSE_END_OF_CONV);

    /* ADC 上电并等待稳定 */
    ADC_enableConverter(ADCA_BASE);
    DEVICE_DELAY_US(1000);

    /*
     * SOC0：
     * 软件触发；
     * ADCINA6；
     * 采样窗口 10 个周期。
     */
    ADC_setupSOC(ADCA_BASE,
                 ADC_SOC_NUMBER0,
                 ADC_TRIGGER_SW_ONLY,
                 ADC_CH_ADCIN6,
                 10);

    /* SOC0 转换结束后设置 ADCINT1 */
    ADC_setInterruptSource(ADCA_BASE,
                           ADC_INT_NUMBER1,
                           ADC_SOC_NUMBER0);

    ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
}


uint16_t readADC_A6(void)
{
    uint16_t result;

    /* CPU 主动触发一次转换 */
    ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER0);

    /* 等转换结束 */
    while(ADC_getInterruptStatus(ADCA_BASE,
                                 ADC_INT_NUMBER1) == false)
    {
    }

    /* 清完成标志 */
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);

    /* 读取原始码值 */
    result = ADC_readResult(ADCARESULT_BASE,
                            ADC_SOC_NUMBER0);

    return result;
}


void main(void)
{
    uint16_t i;

    Device_init();
    Device_initGPIO();

    Interrupt_initModule();
    Interrupt_initVectorTable();

    initADC_A6();

    while(1)
    {
        /* ---------- 第一部分：采集 256 个点 ---------- */

        adcSum     = 0;
        adcMinCode = 4095;
        adcMaxCode = 0;

        for(i = 0; i < ADC_N; i++)
        {
            adcCode = readADC_A6();

            /* 保存到数组 */
            adcBuffer[i] = adcCode;

            /* 累加全部 256 个点 */
            adcSum += adcCode;

            /* 找最小值和最大值 */
            if(adcCode < adcMinCode)
            {
                adcMinCode = adcCode;
            }

            if(adcCode > adcMaxCode)
            {
                adcMaxCode = adcCode;
            }
        }

        adcLastVolt = (float)adcCode / 4096.0f * 3.3f;

        /* ---------- 第二部分：计算前 16 点平均 ---------- */

        adcSum = 0;

        for(i = 0; i < 16; i++)
        {
            adcSum += adcBuffer[i];
        }

        adcAvgCode16 = (float)adcSum / 16.0f;
        adcAvgVolt16 = adcAvgCode16 / 4096.0f * 3.3f;

        /* ---------- 第三部分：计算前 128 点平均 ---------- */

        adcSum = 0;

        for(i = 0; i < 128; i++)
        {
            adcSum += adcBuffer[i];
        }

        adcAvgCode128 = (float)adcSum / 128.0f;
        adcAvgVolt128 = adcAvgCode128 / 4096.0f * 3.3f;

        /* ---------- 第四部分：计算 256 点平均 ---------- */

        adcSum = 0;

        for(i = 0; i < 256; i++)
        {
            adcSum += adcBuffer[i];
        }

        adcAvgCode256 = (float)adcSum / 256.0f;
        adcAvgVolt256 = adcAvgCode256 / 4096.0f * 3.3f;

        /* 完成一批实验后暂停 */
        ESTOP0;
    }
}
