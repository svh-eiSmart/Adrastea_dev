/*  ---------------------------------------------------------------------------

    (c) copyright 2021 Altair Semiconductor, Ltd. All rights reserved.

    This software, in source or object form (the "Software"), is the
    property of Altair Semiconductor Ltd. (the "Company") and/or its
    licensors, which have all right, title and interest therein, You
    may use the Software only in  accordance with the terms of written
    license agreement between you and the Company (the "License").
    Except as expressly stated in the License, the Company grants no
    licenses by implication, estoppel, or otherwise. If you are not
    aware of or do not agree to the License terms, you may not use,
    copy or modify the Software. You may use the source code of the
    Software only for your internal purposes and may not distribute the
    source code of the Software, any part thereof, or any derivative work
    thereof, to any third party, except pursuant to the Company's prior
    written consent.
    The Software is the confidential information of the Company.

   ------------------------------------------------------------------------- */
/**
 * @file DRV_LED.h
 */

#ifndef __LED_H__
#define __LED_H__

#include <stdint.h>

/**
 * @defgroup led LED Low-Level Driver
 * @{
 */
/**
 *  @defgroup LED_const LED Constants
 *  Bit mask of LED controllers from 0 to 5
 *  @{
 */

#define LED_CTRL_0 (1 << 0) /*!< LED controller 0 offset */
#define LED_CTRL_1 (1 << 1) /*!< LED controller 1 offset */
#define LED_CTRL_2 (1 << 2) /*!< LED controller 2 offset */

#ifdef ALT1250
#define LED_CTRL_3 (1 << 3) /*!< LED controller 3 offset */
#define LED_CTRL_4 (1 << 4) /*!< LED controller 4 offset */
#define LED_CTRL_5 (1 << 5) /*!< LED controller 5 offset */
#endif
/* @} LED_const */

/**
 *  @defgroup LED_types LED Types
 *  @{
 */
/**
 * @brief Supported DIM controller
 */
typedef enum { eDIM0 = 2, eDIM1 = 3 } DRV_LED_Dim_Sel;

/**
 * @brief Structure for LED timing configuration
 */
typedef struct {
  uint8_t polarity; /*!< configure according to physical polarity of the LED
                         0 - led is lit when 0
                         1 - led is lit when 1*/
  uint16_t
      offset; /*!< defines the led controllers blink offset in ms from the free running counter;
                   used to sync 2 or more led controller. 12 valid bits ( valid value: 0~8191 )*/
} DRV_LED_Timing;

/**
 * @brief Structure LED function configuration
 * LED function outputs a waveform with a predefined period of high signal and a predefined period
 * of low signal
 */
typedef struct {
  DRV_LED_Timing timing; /*!< LED timing information */
  uint16_t on_duration;  /*!< defines the on duration of the LED in ms; used to model the led blink
                            wave form  12 valid bits ( valid value: 0~8191 )*/
  uint16_t off_duration; /*!< defines the off duration of the LED in ms; used to model the led blink
                            wave form 12 valid bits ( valid value: 0~8191 )*/
} DRV_LED_Param;

/**
 * @brief
 * Structure DIM function configuration
 * The software configures the dim_bright_levels and dim_base_unit_repeats to define the number of
 * brightness levels
 * and how long the led stays on each brightness level. Configuring a higher value to the
 * dim_bright_levels and a lower
 * value to dim_base_unit_repeats will create a smooth dim effect, but might create a flicker.
 */

/*  Example: bright level = 4
            repeat times = 2
            brightest hold = 0
            darkest hold = 0
*     __        __        ____      ____      ______    ______    ________________
*  __|  |______|  |______|    |____|    |____|      |__|      |__|
*
*/
typedef struct {
  DRV_LED_Timing timing;   /*!< LED timing information */
  DRV_LED_Dim_Sel dim_sel; /*!< Select DIM controller */
  uint8_t bright_level; /*!< Defines the number of brightness steps the LED transitions through from
                           darkest to brightest. 6 valid bits ( valid value: 0~63 ) */
  uint16_t repeat_times;  /*!< defines how many times each brightness level is repeated before
                             transitioning to the next one  10 valid bits ( valid value: 0~1023 )*/
  uint8_t brightest_hold; /*!< hold time when LED is brightest
                               In 4ms resolution. 8 valid bits ( valid value: 0~255 )*/
  uint8_t darkest_hold;   /*!< hold time when LED is darkest
                               In 4ms resolution. 8 valid bits ( valid value: 0~255 )*/
} DRV_LED_Dim_Param;

/*! LED error code */
typedef enum {
  LED_ERROR_NONE = 0x0U, /**< Operation succeeded */
  LED_ERROR_PARAM,       /**< Waiting callback when request complete */
} Led_Err_Code;

/** @} LED_types */
/**
 * @defgroup led_api LED APIs
 * @{
 */

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

/**
 * @brief Initialize LED funtionality. This function have to be called before calling any led-related
 * function.
 *
 * @return Led error code. Refer to @ref Led_Err_Code
 */
Led_Err_Code DRV_LED_Initialize(void);

/**
 * @brief Uninitialize LED functionality
 *
 * @return Led error code. Refer to @ref Led_Err_Code
 */
Led_Err_Code DRV_LED_Uninitialize(void);
/**
 * @brief Set the led controller to led mode
 *
 * @param [in] led_channel: Select which led controller to be configured. Bit 0 represents LED0, bit
 * 1 represents LED1 and so on.
 *
 * @param param Refer to @ref DRV_LED_Param
 *
 * @return Led error code. Refer to @ref Led_Err_Code
 */
Led_Err_Code DRV_LED_Set_Led_Mode(int led_channel, DRV_LED_Param* param);

/**
 * @brief Set the led controller to dim mode
 *
 * @param [in] led_channel: Select which led controller to be configured. Bit 0 represents LED0, bit
 * 1 represents LED1 and so on.
 * @param [in] param: Refer to DRV_LED_Dim_Param
 *
 * @return Led error code. Refer to @ref Led_Err_Code
 */
Led_Err_Code DRV_LED_Set_Dim_Mode(int led_channel, DRV_LED_Dim_Param* param);

/**
 * @brief Make led controller apply the new setting. This API will change the pin mux to LED
 *
 * @param [in] led_channel: Select which led controller to be configured. Bit 0 represents LED0, bit
 * 1 represents LED1 and so on.
 *
 * @return Led error code. Refer to @ref Led_Err_Code
 */
Led_Err_Code DRV_LED_Start(int led_channel);

/**
 * @brief Make led controller stop working
 *
 * @param [in] led_channel: Select which led controller to be configured. Bit 0 represents LED0, bit
 * 1 represents LED1 and so on.
 *
 * @return Led error code. Refer to @ref Led_Err_Code
 */
Led_Err_Code DRV_LED_Stop(int led_channel);
#if defined(__cplusplus)
}
#endif
/** @} led_api */
/** @} led */
#endif
