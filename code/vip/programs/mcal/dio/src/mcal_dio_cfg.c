#include "mcal_dio_cfg.h"

/* Usable channels of port 0. Add a channel here AND set the pin up in Port. */
const Dio_PortConfigType Dio_PortCfg[DIO_MAX_PORT] =
{
    {
        .valid_mask = DIO_CHANNEL_BIT(DioConf_DioChannel_SPI_CS)      |
                      DIO_CHANNEL_BIT(DioConf_DioChannel_TestPin)     |
                      DIO_CHANNEL_BIT(DioConf_DioChannel_LED_Onboard)
    }
};

/* Channel groups. This one is a one-bit template group on the test pin;
 * widen mask / offset to group several pins, e.g. GP20..GP23:
 *   .mask = 0x00F00000u, .offset = 20u */
const Dio_ChannelGroupType Dio_ChannelGroupCfg[DIO_MAX_CHANNEL_GROUP] =
{
    {
        .mask   = DIO_CHANNEL_BIT(DioConf_DioChannel_TestPin),
        .offset = DIO_CHANNEL_PIN(DioConf_DioChannel_TestPin),
        .port   = DioConf_DioPort_GPIO0
    }
};
