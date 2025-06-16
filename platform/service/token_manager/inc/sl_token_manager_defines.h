/***************************************************************************//**
 * @file sl_token_manager_defines.h
 * @brief Header file for generic token manager defines
 * @version 1.0.0
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories, Inc, www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#ifndef SL_TOKEN_MANAGER_DEFINES_H
#define SL_TOKEN_MANAGER_DEFINES_H

// MACRO's for mask values to identify token as
// NVM3 or STATIC DEVICE or STATIC SECURE
#define SL_TOKEN_DYNAMIC_MASK                     (0xF8000000)
#define SL_TOKEN_STATIC_MASK                      (0xE0000000)

/*
 * The new 32-bit token identifier used for creating the key-length-value chain.
 * To differentiate between different objects we utilize the
 * upper 5 bits of the key identifier:
 *
 * bit (31:27) Maps to:
 *
 * 0b00000 NVM3 (Default instance)
 * 0b00001 NVM3 (Secondary instance - Zwave usage)
 * 0b0001x Reserved for additional NVM3 instances
 * 0b110xx Static device tokens
 * 0b111xx Static secure tokens
 * 0b100xx Reserved for future use
 * 0b101xx Reserved for future use
 *
 * The Key-Length-Value system works as a chain of KLV objects.
 * A KLV object is identified in the following way:
 *
 * 31:29 | 28:16    | 15:0
 * 0b1xx | Reserved | Key
 *
 * */
#define SL_TOKEN_TYPE_NVM3                        (0x00000000U)
#define SL_TOKEN_TYPE_NVM3_SECONDARY              (0x08000000U)
#define SL_TOKEN_TYPE_STATIC_DEVICE               (0xC0000000U)
#define SL_TOKEN_TYPE_STATIC_SECURE               (0xE0000000U)

// MACRO's for NVM3 Default Instance Key Space Regions
// NVM3 is going to be used for Non-Volatile storage across all our device types (zigbee, Bluetooth, MCU...).
// Each object in the NVM3 is addressed by a 20-bit key. These keys need to be unique within an NVM3 instance
// to avoid collisions. Such collisions can for instance occur if a user application writes to an object
// that the RF stack is using for a different purpose.
// For multi-protocol applications we also risk collisions when where two stacks inadvertently use the
// same object key in a non compatible way. By default an NVM3 default instance will be shared among
// all stacks and application code.

// To avoid collisions it is proposed to use the 4 MSBs of the default instance NVM3 key to split
// the NVM3 ID space into the regions in the table below. Each stack team is responsible for
// managing the keys within their regions. A user should be able to use the keys in the User region as they wish.
// If any configurator tools use any keys in the User region these keys should be made clearly
// visible to the user so they can avoid using the same keys for NVM3 objects created manually in
// the application code.

// NOTE: NVM3 region (0xC0000) to (0xF0000) are reserved.

#define SL_TOKEN_NVM3_REGION_USER                 (0x00000) //User
#define SL_TOKEN_NVM3_REGION_ZIGBEE               (0x10000) //Zigbee stack
#define SL_TOKEN_NVM3_REGION_THREAD               (0x20000) //Thread stack
#define SL_TOKEN_NVM3_REGION_CONNECT              (0x30000) //Connect stack
#define SL_TOKEN_NVM3_REGION_BT                   (0x40000) //Bluetooth stack
#define SL_TOKEN_NVM3_REGION_ZWAVE                (0x50000) //Z-wave
#define SL_TOKEN_NVM3_REGION_BTMESH               (0x60000) //Bluetooth Mesh
#define SL_TOKEN_NVM3_REGION_RESERVED0            (0x70000) //Reserved
// NVM3 Common region
#define SL_TOKEN_NVM3_REGION_APPLE_HK             (0x80000) //Apple Homekit
#define SL_TOKEN_NVM3_REGION_ZCL                  (0x81000) //ZCL
#define SL_TOKEN_NVM3_REGION_DOTDOT               (0x82000) //DotDot
#define SL_TOKEN_NVM3_REGION_PLT_CLISM            (0x83000) //Platform CLI SM
#define SL_TOKEN_NVM3_REGION_PLT_CRYPTO           (0x83100) //Platform Crypto
#define SL_TOKEN_NVM3_REGION_BTL                  (0x87100) //Bootloader
#define SL_TOKEN_NVM3_REGION_MATTER               (0x87200) //Matter
#define SL_TOKEN_NVM3_REGION_AWS_LATE_PRN         (0x88000) //AWS Late Provision
#define SL_TOKEN_NVM3_REGION_KNX_IOT              (0x89000) //KNX IoT
#define SL_TOKEN_NVM3_REGION_ANT_CAL_PLT          (0x89800) //Antenna Calibration (Platform)
#define SL_TOKEN_NVM3_REGION_ANT_CAL_RAIL         (0x89880) //Antenna Calibration (RAIL)
// NOTE: 0x8E000 and 0x8F000 are used for static tokens and are not available.
#define SL_TOKEN_NVM3_REGION_WISUN                (0x90000) //Wi-SUN
#define SL_TOKEN_NVM3_REGION_SW                   (0xA0000) //Sidewalk
#define SL_TOKEN_NVM3_REGION_WIFI                 (0xB0000) //Wi-Fi
// NOTE: NVM3 region (0xC0000) to (0xF0000) are reserved.

#ifndef SL_TOKEN_NVM3_OVERRIDE
//To store a static token in NVM3 region we shall follow NVM3 key format.
//Each object in the NVM3 is addressed by a 20-bit key
//4MSB bits(19:16) of the NVM3 key are used to identify the static tokens
//that are stored in NVM3 memory space. The values used for representing static tokens in NVM3 region are
//0x8E->for static device tokens
//0x8F->for static secure tokens is used along with the existing static token ID to form a 20bit NVM3 key.
#define SL_TOKEN_STATIC_TOKEN_KEY_MASK                      (0x0FFF)
#define SL_TOKEN_STATIC_TOKEN_RANGE_MASK                    (0xFF000)
#define SL_TOKEN_STATIC_TOKEN_BASE_TYPE_KEY_MASK            (0x3000)
#define SL_TOKEN_NVM3_REGION_STATIC_TOKEN_RANGE_OFFSET      (0xC)
#define SL_TOKEN_NVM3_REGION_STATIC_DEVICE_TOKEN_RANGE_ID   (0x8E)
#define SL_TOKEN_NVM3_REGION_STATIC_SECURE_TOKEN_RANGE_ID   (0x8F)
#define SL_TOKEN_NVM3_REGION_STATIC_DEVICE_TOKEN_RANGE      (SL_TOKEN_NVM3_REGION_STATIC_DEVICE_TOKEN_RANGE_ID \
                                                             << SL_TOKEN_NVM3_REGION_STATIC_TOKEN_RANGE_OFFSET)

#define SL_TOKEN_NVM3_REGION_STATIC_SECURE_TOKEN_RANGE      (SL_TOKEN_NVM3_REGION_STATIC_SECURE_TOKEN_RANGE_ID \
                                                             << SL_TOKEN_NVM3_REGION_STATIC_TOKEN_RANGE_OFFSET)

#endif

//The bits 28:16 has the size of the static token embedded
#define SL_TOKEN_STATIC_TOKEN_SIZE_OFFSET                (0x10)
#define SL_TOKEN_STATIC_TOKEN_SIZE_MASK                  (0x1FFF)
//The bit 20 defines if the NVM3 token is a counter object or a data object
#define SL_TOKEN_NVM3_OBJ_TYPE_COUNTER_TOKEN             (1)
#define SL_TOKEN_NVM3_OBJ_TYPE_OFFSET                    (0x14)
#define SL_TOKEN_NVM3_OBJ_TYPE_BIT_MASK                  (SL_TOKEN_NVM3_OBJ_TYPE_COUNTER_TOKEN << SL_TOKEN_NVM3_OBJ_TYPE_OFFSET)

// Helper MACRO to get size of a static token
#define SL_TOKEN_GET_STATIC_TOKEN_SIZE(token_name)       token_name##_SIZE

/*
   token_type  : Static Device Token.
   description : Helper MACRO's to create a new Static Device token in 32-bit identifier format.
              These tokens are stored in SE MTP region and are mass erase protected.

   Key format  :
 | 31:30 | 29                 | 28:16       | 15:0 |
 | type  | static token type  | Data length | key  |

   static token type : 0 - Static device token page (SE MTP)
   key               : 16-bit unique key.

   @param[in] 'token' 16-bit unique key \ref sl_token_manager_manufacturing.h file
                      or sl_custom_manufacturing_token_header.h.
 */
#define SL_TOKEN_GET_STATIC_DEVICE_TOKEN(token)    (SL_TOKEN_TYPE_STATIC_DEVICE \
                                                    | token                     \
                                                    | (token##_SIZE << SL_TOKEN_STATIC_TOKEN_SIZE_OFFSET))

/*
   token_type  : Static Secure Token.
   description : Helper MACRO's to create a new Static Device token in 32-bit identifier format.
              These tokens are stored in dedicated flash regions.
              Token value's are defined in sl_token_manager_manufacturing.h

   Key format  :
 | 31:30 | 29                 | 28:16       | 15:0 |
 | type  | static token type  | Data length | key  |

   static token type : 1 - Static secure token page (flash)
   key               : 16-bit unique key.

   @param[in] 'token' 16-bit unique key \ref sl_token_manager_manufacturing.h file
                      or sl_custom_manufacturing_token_header.h.
 */
#define SL_TOKEN_GET_STATIC_SECURE_TOKEN(token)    (SL_TOKEN_TYPE_STATIC_SECURE \
                                                    | token                     \
                                                    | (token##_SIZE << SL_TOKEN_STATIC_TOKEN_SIZE_OFFSET))

/*
   token_type  : SL_TOKEN_TYPE_NVM3
   description : Creates NVM3 tokens. User has to take care of providing the
              unique token for each NVM3 token created.
              These tokens are stored in NVM3 region and are not
              protected by mass/device/nvm3 erase

   Key format  :
 | 31:27 | 26:21    | 20               |19:16        | 15:0     |
 | type  | reserved | NVM3 object type | NVM3 domain | NVM3 key |

   NVM3 object type : 0 - Data object
                      1 - Counter object
   NVM3 domain      : NVM3 Default Instance Key Space Regions.
   NVM3 key         : 16-bit unique key.

   @param[in] 'token' 20-bit NVM3 key
   @param[in] 'IsCounterObj' should be set to 1, if a token is created for counter objects.
 */
#define SL_TOKEN_GET_DYNAMIC_TOKEN(token, IsCounterObj)   (SL_TOKEN_TYPE_NVM3 \
                                                           | token            \
                                                           | (IsCounterObj << SL_TOKEN_NVM3_OBJ_TYPE_OFFSET))

/*
   token_type  : SL_TOKEN_TYPE_NVM3_SECONDARY
   description : Creates Zwave usage NVM3 tokens. User has to take care of providing the
              unique token for each NVM3 token created.
              These tokens are stored in NVM3 region and are not
              protected by mass/device/nvm3 erase

   Key format  :
 | 31:27 | 26:21    | 20               |19:16        | 15:0     |
 | type  | reserved | NVM3 object type | NVM3 domain | NVM3 key |

   NVM3 object type : 0 - Data object
                      1 - Counter object
   NVM3 domain      : NVM3 Default Instance Key Space Regions.
   NVM3 key         : 16-bit unique key.

   @param[in] 'token' 20-bit NVM3 key
   @param[in] 'IsCounterObj' should be set to 1, if a token is created for counter objects.
 */
#define SL_TOKEN_GET_DYNAMIC_SECONDARY_INSTANCE_TOKEN(token, IsCounterObj)   (SL_TOKEN_TYPE_NVM3_SECONDARY \
                                                                              | token                      \
                                                                              | (IsCounterObj << SL_TOKEN_NVM3_OBJ_TYPE_OFFSET))

/*
   token_type  : NVM3 Override tokens and follows the NVM3 token creation rules.
   description : Helper MACRO's to create a NVM3 override key for static tokens.
              NVM3 Domain region for a static token override key is predefined
              0x8E->for static device tokens
              0x8F->for static secure tokens
 */
#ifndef SL_TOKEN_GET_DYNAMIC_OVER_RIDE_TOKEN
#define SL_TOKEN_GET_DYNAMIC_OVER_RIDE_TOKEN(token)   (SL_TOKEN_TYPE_NVM3                                                                       \
                                                       | (token & SL_TOKEN_STATIC_TOKEN_KEY_MASK)                                               \
                                                       | (((token & SL_TOKEN_STATIC_TOKEN_BASE_TYPE_KEY_MASK) == SL_TOKEN_STATIC_DEVICE_TOKENS) \
                                                          ? SL_TOKEN_NVM3_REGION_STATIC_DEVICE_TOKEN_RANGE                                      \
                                                          : SL_TOKEN_NVM3_REGION_STATIC_SECURE_TOKEN_RANGE))
#endif

#endif // SL_TOKEN_MANAGER_DEFINES_H
