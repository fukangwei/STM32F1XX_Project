#include "enc28j60.h"
#include "spi.h"
#include "delay.h"
#include "usart.h"

dev_strucrt enc28j60_dev;

u8 ENC28J60_Init ( void ) {
    u8 version;
    u16 retry = 0;
    u32 temp;
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;
    RCC_APB2PeriphClockCmd ( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE );
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init ( GPIOA, &GPIO_InitStructure );
    GPIO_SetBits ( GPIOA, GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 );
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init ( GPIOC, &GPIO_InitStructure );
    GPIO_SetBits ( GPIOC, GPIO_Pin_4 );
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init ( GPIOA, &GPIO_InitStructure );
    SPI1_Init();
    SPI_Cmd ( SPI1, DISABLE );
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init ( SPI1, &SPI_InitStructure );
    SPI_Cmd ( SPI1, ENABLE );
    SPI1_SetSpeed ( SPI_BaudRatePrescaler_8 );
    temp = * ( vu32* ) ( 0x1FFFF7E8 ); // STM32唯一ID的前24位作为MAC地址后三字节
    enc28j60_dev.macaddr[0] = 2;
    enc28j60_dev.macaddr[1] = 0;
    enc28j60_dev.macaddr[2] = 0;
    enc28j60_dev.macaddr[3] = ( temp >> 16 ) & 0XFF;
    enc28j60_dev.macaddr[4] = ( temp >> 8 ) & 0XFFF;
    enc28j60_dev.macaddr[5] = temp & 0XFF;
    ENC28J60_RST = 0;
    delay_ms ( 10 );
    ENC28J60_RST = 1;
    delay_ms ( 10 );
    ENC28J60_Write_Op ( ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET );

    while ( ! ( ENC28J60_Read ( ESTAT ) &ESTAT_CLKRDY ) && retry < 250 ) {
        retry++;
        delay_ms ( 1 );
    }

    if ( retry >= 250 ) {
        return 1;
    }

    version = ENC28J60_Get_EREVID();
    printf ( "ENC28J60 Version: %d\r\n", version );
    enc28j60_dev.NextPacketPtr = RXSTART_INIT;
    ENC28J60_Write ( ERXSTL, RXSTART_INIT & 0XFF ); // 设置接收缓冲区起始地址低8位
    ENC28J60_Write ( ERXSTH, RXSTART_INIT >> 8 );   // 设置接收缓冲区起始地址高8位
    // 设置接收接收字节
    ENC28J60_Write ( ERXNDL, RXSTOP_INIT & 0XFF );
    ENC28J60_Write ( ERXNDH, RXSTOP_INIT >> 8 );
    // 设置发送起始字节
    ENC28J60_Write ( ETXSTL, TXSTART_INIT & 0XFF );
    ENC28J60_Write ( ETXSTH, TXSTART_INIT >> 8 );
    // 设置发送结束字节
    ENC28J60_Write ( ETXNDL, TXSTOP_INIT & 0XFF );
    ENC28J60_Write ( ETXNDH, TXSTOP_INIT >> 8 );
    // 设置接收读指针字节
    ENC28J60_Write ( ERXRDPTL, RXSTART_INIT & 0XFF );
    ENC28J60_Write ( ERXRDPTH, RXSTART_INIT >> 8 );
    // 接收过滤器
    // UCEN：单播过滤器使能位
    //     当ANDOR = 1时：
    //         1 = 目标地址与本地MAC地址不匹配的数据包将被丢弃
    //         0 = 禁止过滤器
    //     当ANDOR = 0 时：
    //         1 = 目标地址与本地MAC地址匹配的数据包会被接受
    //         0 = 禁止过滤器
    // CRCEN：后过滤器CRC校验使能位
    //     1 = 所有CRC无效的数据包都将被丢弃
    //     0 = 不考虑CRC是否有效
    // PMEN：格式匹配过滤器使能位
    //     当ANDOR = 1时：
    //         1 = 数据包必须符合格式匹配条件，否则将被丢弃
    //         0 = 禁止过滤器
    //     当ANDOR = 0时：
    //         1 = 符合格式匹配条件的数据包将被接受
    //         0 = 禁止过滤器
    // ENC28J60_Write ( ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_PMEN );
    ENC28J60_Write ( ERXFCON, 0 );
    ENC28J60_Write ( EPMM0, 0X3F );
    ENC28J60_Write ( EPMM1, 0X30 );
    ENC28J60_Write ( EPMCSL, 0Xf9 );
    ENC28J60_Write ( EPMCSH, 0Xf7 );
    // bit 0 MARXEN：MAC接收使能位
    //     1 = 允许MAC接收数据包
    //     0 = 禁止数据包接收
    // bit 3 TXPAUS：暂停控制帧发送使能位
    //     1 = 允许MAC发送暂停控制帧(用于全双工模式下的流量控制)
    //     0 = 禁止暂停帧发送
    // bit 2 RXPAUS：暂停控制帧接收使能位
    //     1 = 当接收到暂停控制帧时，禁止发送(正常操作)
    //     0 = 忽略接收到的暂停控制帧
    ENC28J60_Write ( MACON1, MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS );
    // 将MACON2中的MARST位清零，使MAC退出复位状态
    ENC28J60_Write ( MACON2, 0x00 );
    // bit 7-5 PADCFG2:PACDFG0：自动填充和CRC配置位
    //     111 = 用0填充所有短帧至64字节长，并追加一个有效的CRC
    //     110 = 不自动填充短帧
    //     101 = MAC自动检测具有8100h类型字段的VLAN协议帧，并自动填充到64字节长。
    //           如果不是VLAN帧，则填充至60字节长。填充后还要追加一个有效的CRC
    //     100 = 不自动填充短帧
    //     011 = 用0填充所有短帧至64字节长，并追加一个有效的CRC
    //     010 = 不自动填充短帧
    //     001 = 用0填充所有短帧至60字节长，并追加一个有效的CRC
    //     000 = 不自动填充短帧
    // bit 4 TXCRCEN：发送CRC使能位
    //     1 = 不管PADCFG如何，MAC都会在发送帧的末尾追加一个有效的CRC。
    //         如果PADCFG规定要追加有效的CRC，则必须将TXCRCEN 置1。
    //     0 = MAC不会追加CRC。检查最后4个字节，如果不是有效的CRC，则报告给发送状态向量
    //bit 0 FULDPX：MAC全双工使能位
    //    1 = MAC工作在全双工模式下。PHCON1.PDPXMD位必须置1。
    //    0 = MAC工作在半双工模式下。PHCON1.PDPXMD位必须清零。
    ENC28J60_Write ( MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN | MACON3_FULDPX );
    // 最大帧长度为1518
    ENC28J60_Write ( MAMXFLL, MAX_FRAMELEN & 0XFF );
    ENC28J60_Write ( MAMXFLH, MAX_FRAMELEN >> 8 );
    // 配置背对背包间间隔寄存器MABBIPG
    ENC28J60_Write ( MABBIPG, 0x15 );
    // 配置非背对背包间间隔寄存器
    ENC28J60_Write ( MAIPGL, 0x12 );
    ENC28J60_Write ( MAIPGH, 0x0C );
    // 设置MAC地址
    ENC28J60_Write ( MAADR5, enc28j60_dev.macaddr[0] );
    ENC28J60_Write ( MAADR4, enc28j60_dev.macaddr[1] );
    ENC28J60_Write ( MAADR3, enc28j60_dev.macaddr[2] );
    ENC28J60_Write ( MAADR2, enc28j60_dev.macaddr[3] );
    ENC28J60_Write ( MAADR1, enc28j60_dev.macaddr[4] );
    ENC28J60_Write ( MAADR0, enc28j60_dev.macaddr[5] );
    // 配置PHY为全双工，LEDB为拉电流
    ENC28J60_PHY_Write ( PHCON1, PHCON1_PDPXMD );
    // HDLDIS：PHY半双工环回禁止位
    //     当PHCON1.PDPXMD = 1或PHCON1.PLOOPBK = 1时，此位可被忽略
    //     当PHCON1.PDPXMD = 0且PHCON1.PLOOPBK = 0时：
    //         1 = 要发送的数据仅通过双绞线接口发出
    //         0 = 要发送的数据会环回到MAC，并通过双绞线接口发出
    ENC28J60_PHY_Write ( PHCON2, PHCON2_HDLDIS );
    // ECON1寄存器用于控制ENC28J60的主要功能
    ENC28J60_Set_Bank ( ECON1 );
    // EIE：以太网中断允许寄存器
    //     bit 7 INTIE：全局INT中断允许位
    //         1 = 允许中断事件驱动INT引脚
    //         0 = 禁止所有INT引脚的活动(引脚始终被驱动为高电平)
    //     bit 6 PKTIE：接收数据包待处理中断允许位
    //         1 = 允许接收数据包待处理中断
    //         0 = 禁止接收数据包待处理中断
    ENC28J60_Write_Op ( ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE | EIE_PKTIE | EIE_TXIE | EIE_TXERIE | EIE_RXERIE );
    // bit 2 RXEN：接收使能位
    //     1 = 通过当前过滤器的数据包将被写入接收缓冲器
    //     0 = 忽略所有接收的数据包
    ENC28J60_Write_Op ( ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN );
    printf ( "ENC28J60 Duplex: %s\r\n", ENC28J60_Get_Duplex() ? "Full Duplex" : "Half Duplex" ); // 获取双工方式
    return 0;
}

u8 ENC28J60_Read_Op ( u8 op, u8 addr ) {
    u8 dat = 0;
    ENC28J60_CS = 0;
    dat = op | ( addr & ADDR_MASK );
    SPI1_ReadWriteByte ( dat );
    dat = SPI1_ReadWriteByte ( 0XFF );

    // 如果是读取MAC/MII寄存器，则第二次读到的数据才是正确的
    if ( addr & 0x80 ) {
        dat = SPI1_ReadWriteByte ( 0XFF );
    }

    ENC28J60_CS = 1;
    return dat;
}

void ENC28J60_Write_Op ( u8 op, u8 addr, u8 data ) {
    u8 dat = 0;
    ENC28J60_CS = 0;
    dat = op | ( addr & ADDR_MASK );
    SPI1_ReadWriteByte ( dat );
    SPI1_ReadWriteByte ( data );
    ENC28J60_CS = 1;
}

void ENC28J60_Read_Buf ( u32 len, u8* data ) {
    ENC28J60_CS = 0;
    SPI1_ReadWriteByte ( ENC28J60_READ_BUF_MEM );

    while ( len ) {
        len--;
        *data = ( u8 ) SPI1_ReadWriteByte ( 0 );
        data++;
    }

    *data = '\0';
    ENC28J60_CS = 1;
}

void ENC28J60_Write_Buf ( u32 len, u8* data ) {
    ENC28J60_CS = 0;
    SPI1_ReadWriteByte ( ENC28J60_WRITE_BUF_MEM );

    while ( len ) {
        len--;
        SPI1_ReadWriteByte ( *data );
        data++;
    }

    ENC28J60_CS = 1;
}

void ENC28J60_Set_Bank ( u8 bank ) {
    if ( ( bank & BANK_MASK ) != enc28j60_dev.enc28j60bank ) { // 和当前bank不一致的时候才设置
        ENC28J60_Write_Op ( ENC28J60_BIT_FIELD_CLR, ECON1, ( ECON1_BSEL1 | ECON1_BSEL0 ) );
        ENC28J60_Write_Op ( ENC28J60_BIT_FIELD_SET, ECON1, ( bank & BANK_MASK ) >> 5 );
        enc28j60_dev.enc28j60bank = ( bank & BANK_MASK );
    }
}

u8 ENC28J60_Read ( u8 addr ) {
    ENC28J60_Set_Bank ( addr );
    return ENC28J60_Read_Op ( ENC28J60_READ_CTRL_REG, addr );
}

void ENC28J60_Write ( u8 addr, u8 data ) {
    ENC28J60_Set_Bank ( addr );
    ENC28J60_Write_Op ( ENC28J60_WRITE_CTRL_REG, addr, data );
}

void ENC28J60_PHY_Write ( u8 addr, u16 data ) {
    u16 retry = 0;
    ENC28J60_Write ( MIREGADR, addr );   // 向MIREGADR写入PHY寄存器地址
    ENC28J60_Write ( MIWRL, data );      // 写入数据的低8字节
    ENC28J60_Write ( MIWRH, data >> 8 ); // 写入数据的高8字节

    while ( ( ENC28J60_Read ( MISTAT ) &MISTAT_BUSY ) && retry < 0XFFFF ) {
        retry++;
    }
}

u16 ENC28J60_PHY_Read ( u8 addr ) {
    u8 temp;
    u16 phyvalue, retry = 0;
    temp = ENC28J60_Read ( MICMD );
    ENC28J60_Write ( MIREGADR, addr );
    ENC28J60_Write ( MICMD, temp | MICMD_MIIRD ); // 开始读PHY寄存器

    while ( ( ENC28J60_Read ( MISTAT ) &MISTAT_BUSY ) && retry < 0XFFFF ) {
        retry++; // 等待读PHY完成
    }

    ENC28J60_Write ( MICMD, temp & ( ~MICMD_MIIRD ) ); // 读PHY完成
    phyvalue = ENC28J60_Read ( MIRDL );                // 读取低8位
    phyvalue |= ( ENC28J60_Read ( MIRDH ) << 8 );      // 读取高8位
    return phyvalue;
}

u8 ENC28J60_Get_EREVID ( void ) {
    // EREVID用来标识器件特定硅片的版本号
    return ENC28J60_Read ( EREVID ) & 0X1F;
}

u16 ENC28J60_Get_Duplex ( void ) {
    u16 temp;
    temp = ENC28J60_PHY_Read ( PHCON1 ) & 0X0100;
    return ( temp >> 8 );
}

void ENC28J60_Packet_Send ( u32 len, u8* packet ) {
    while ( ENC28J60_Read ( ECON1 ) & 0X08 ); // 正在发送数据，等待发送完成

    // 设置发送缓冲区地址写指针入口
    ENC28J60_Write ( EWRPTL, TXSTART_INIT & 0XFF );
    ENC28J60_Write ( EWRPTH, TXSTART_INIT >> 8 );
    // 设置TXND指针，以对应给定的数据包大小
    ENC28J60_Write ( ETXNDL, ( TXSTART_INIT + len ) & 0XFF );
    ENC28J60_Write ( ETXNDH, ( TXSTART_INIT + len ) >> 8 );

    // 写每包控制字节
    if ( len > MAX_FRAMELEN ) { // 超大帧
        ENC28J60_Write ( MACON3, MACON3_HFRMEN );
        ENC28J60_Write_Op ( ENC28J60_WRITE_BUF_MEM, 0, PKTCTRL_POVERRIDE | PKTCTRL_PHUGEEN );
    } else {
        ENC28J60_Write_Op ( ENC28J60_WRITE_BUF_MEM, 0, 0x00 );
    }

    // 复制数据包到发送缓冲区
    ENC28J60_Write_Buf ( len, packet );
    // 发送数据到网络
    ENC28J60_Write_Op ( ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS );

    // 复位发送逻辑的问题
    if ( ( ENC28J60_Read ( EIR ) &EIR_TXERIF ) ) {
        ENC28J60_Write_Op ( ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS );
    }
}

u32 ENC28J60_Packet_Receive ( u32 maxlen, u8* packet ) {
    u32 rxstat;
    u32 len;

    if ( ENC28J60_Read ( EPKTCNT ) == 0 ) {
        return 0;
    }

    while ( ( ENC28J60_Read ( ESTAT ) & 0X04 ) ); // 接收忙，等待接收完成

    // 设置接收缓冲器读指针
    ENC28J60_Write ( ERDPTL, ( enc28j60_dev.NextPacketPtr ) );
    ENC28J60_Write ( ERDPTH, ( enc28j60_dev.NextPacketPtr ) >> 8 );
    // 读下一个包的指针
    enc28j60_dev.NextPacketPtr = ENC28J60_Read_Op ( ENC28J60_READ_BUF_MEM, 0 );
    enc28j60_dev.NextPacketPtr |= ENC28J60_Read_Op ( ENC28J60_READ_BUF_MEM, 0 ) << 8;
    // 读包的长度
    len = ENC28J60_Read_Op ( ENC28J60_READ_BUF_MEM, 0 );
    len |= ENC28J60_Read_Op ( ENC28J60_READ_BUF_MEM, 0 ) << 8;
    len -= 4; // 去掉CRC计数
    // 读取接收状态
    rxstat = ENC28J60_Read_Op ( ENC28J60_READ_BUF_MEM, 0 );
    rxstat |= ENC28J60_Read_Op ( ENC28J60_READ_BUF_MEM, 0 ) << 8;

    // 限制接收长度
    if ( len > maxlen - 1 ) {
        len = maxlen - 1;
    }

    // 检查CRC和符号错误
    // ERXFCON.CRCEN为默认设置，一般我们不需要检查
    if ( ( rxstat & 0x80 ) == 0 ) {
        len = 0;
    } else {
        ENC28J60_Read_Buf ( len, packet ); // 从接收缓冲器中复制数据包
    }

    // RX读指针移动到下一个接收到的数据包的开始位置，并释放我们刚才读出过的内存
    ENC28J60_Write ( ERXRDPTL, ( enc28j60_dev.NextPacketPtr ) );
    ENC28J60_Write ( ERXRDPTH, ( enc28j60_dev.NextPacketPtr ) >> 8 );
    // 递减数据包计数器标志我们已经得到了这个包
    ENC28J60_Write_Op ( ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC );
    return ( len );
}
