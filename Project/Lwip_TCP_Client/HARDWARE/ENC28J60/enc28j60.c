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
    temp = * ( vu32* ) ( 0x1FFFF7E8 ); // STM32ΨһID��ǰ24λ��ΪMAC��ַ�����ֽ�
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
    ENC28J60_Write ( ERXSTL, RXSTART_INIT & 0XFF ); // ���ý��ջ�������ʼ��ַ��8λ
    ENC28J60_Write ( ERXSTH, RXSTART_INIT >> 8 );   // ���ý��ջ�������ʼ��ַ��8λ
    // ���ý��ս����ֽ�
    ENC28J60_Write ( ERXNDL, RXSTOP_INIT & 0XFF );
    ENC28J60_Write ( ERXNDH, RXSTOP_INIT >> 8 );
    // ���÷�����ʼ�ֽ�
    ENC28J60_Write ( ETXSTL, TXSTART_INIT & 0XFF );
    ENC28J60_Write ( ETXSTH, TXSTART_INIT >> 8 );
    // ���÷��ͽ����ֽ�
    ENC28J60_Write ( ETXNDL, TXSTOP_INIT & 0XFF );
    ENC28J60_Write ( ETXNDH, TXSTOP_INIT >> 8 );
    // ���ý��ն�ָ���ֽ�
    ENC28J60_Write ( ERXRDPTL, RXSTART_INIT & 0XFF );
    ENC28J60_Write ( ERXRDPTH, RXSTART_INIT >> 8 );
    // ���չ�����
    // UCEN������������ʹ��λ
    //     ��ANDOR = 1ʱ��
    //         1 = Ŀ���ַ�뱾��MAC��ַ��ƥ������ݰ���������
    //         0 = ��ֹ������
    //     ��ANDOR = 0 ʱ��
    //         1 = Ŀ���ַ�뱾��MAC��ַƥ������ݰ��ᱻ����
    //         0 = ��ֹ������
    // CRCEN���������CRCУ��ʹ��λ
    //     1 = ����CRC��Ч�����ݰ�����������
    //     0 = ������CRC�Ƿ���Ч
    // PMEN����ʽƥ�������ʹ��λ
    //     ��ANDOR = 1ʱ��
    //         1 = ���ݰ�������ϸ�ʽƥ�����������򽫱�����
    //         0 = ��ֹ������
    //     ��ANDOR = 0ʱ��
    //         1 = ���ϸ�ʽƥ�����������ݰ���������
    //         0 = ��ֹ������
    // ENC28J60_Write ( ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_PMEN );
    ENC28J60_Write ( ERXFCON, 0 );
    ENC28J60_Write ( EPMM0, 0X3F );
    ENC28J60_Write ( EPMM1, 0X30 );
    ENC28J60_Write ( EPMCSL, 0Xf9 );
    ENC28J60_Write ( EPMCSH, 0Xf7 );
    // bit 0 MARXEN��MAC����ʹ��λ
    //     1 = ����MAC�������ݰ�
    //     0 = ��ֹ���ݰ�����
    // bit 3 TXPAUS����ͣ����֡����ʹ��λ
    //     1 = ����MAC������ͣ����֡(����ȫ˫��ģʽ�µ���������)
    //     0 = ��ֹ��ͣ֡����
    // bit 2 RXPAUS����ͣ����֡����ʹ��λ
    //     1 = �����յ���ͣ����֡ʱ����ֹ����(��������)
    //     0 = ���Խ��յ�����ͣ����֡
    ENC28J60_Write ( MACON1, MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS );
    // ��MACON2�е�MARSTλ���㣬ʹMAC�˳���λ״̬
    ENC28J60_Write ( MACON2, 0x00 );
    // bit 7-5 PADCFG2:PACDFG0���Զ�����CRC����λ
    //     111 = ��0������ж�֡��64�ֽڳ�����׷��һ����Ч��CRC
    //     110 = ���Զ�����֡
    //     101 = MAC�Զ�������8100h�����ֶε�VLANЭ��֡�����Զ���䵽64�ֽڳ���
    //           �������VLAN֡���������60�ֽڳ�������Ҫ׷��һ����Ч��CRC
    //     100 = ���Զ�����֡
    //     011 = ��0������ж�֡��64�ֽڳ�����׷��һ����Ч��CRC
    //     010 = ���Զ�����֡
    //     001 = ��0������ж�֡��60�ֽڳ�����׷��һ����Ч��CRC
    //     000 = ���Զ�����֡
    // bit 4 TXCRCEN������CRCʹ��λ
    //     1 = ����PADCFG��Σ�MAC�����ڷ���֡��ĩβ׷��һ����Ч��CRC��
    //         ���PADCFG�涨Ҫ׷����Ч��CRC������뽫TXCRCEN ��1��
    //     0 = MAC����׷��CRC��������4���ֽڣ����������Ч��CRC���򱨸������״̬����
    //bit 0 FULDPX��MACȫ˫��ʹ��λ
    //    1 = MAC������ȫ˫��ģʽ�¡�PHCON1.PDPXMDλ������1��
    //    0 = MAC�����ڰ�˫��ģʽ�¡�PHCON1.PDPXMDλ�������㡣
    ENC28J60_Write ( MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN | MACON3_FULDPX );
    // ���֡����Ϊ1518
    ENC28J60_Write ( MAMXFLL, MAX_FRAMELEN & 0XFF );
    ENC28J60_Write ( MAMXFLH, MAX_FRAMELEN >> 8 );
    // ���ñ��Ա��������Ĵ���MABBIPG
    ENC28J60_Write ( MABBIPG, 0x15 );
    // ���÷Ǳ��Ա��������Ĵ���
    ENC28J60_Write ( MAIPGL, 0x12 );
    ENC28J60_Write ( MAIPGH, 0x0C );
    // ����MAC��ַ
    ENC28J60_Write ( MAADR5, enc28j60_dev.macaddr[0] );
    ENC28J60_Write ( MAADR4, enc28j60_dev.macaddr[1] );
    ENC28J60_Write ( MAADR3, enc28j60_dev.macaddr[2] );
    ENC28J60_Write ( MAADR2, enc28j60_dev.macaddr[3] );
    ENC28J60_Write ( MAADR1, enc28j60_dev.macaddr[4] );
    ENC28J60_Write ( MAADR0, enc28j60_dev.macaddr[5] );
    // ����PHYΪȫ˫����LEDBΪ������
    ENC28J60_PHY_Write ( PHCON1, PHCON1_PDPXMD );
    // HDLDIS��PHY��˫�����ؽ�ֹλ
    //     ��PHCON1.PDPXMD = 1��PHCON1.PLOOPBK = 1ʱ����λ�ɱ�����
    //     ��PHCON1.PDPXMD = 0��PHCON1.PLOOPBK = 0ʱ��
    //         1 = Ҫ���͵����ݽ�ͨ��˫���߽ӿڷ���
    //         0 = Ҫ���͵����ݻỷ�ص�MAC����ͨ��˫���߽ӿڷ���
    ENC28J60_PHY_Write ( PHCON2, PHCON2_HDLDIS );
    // ECON1�Ĵ������ڿ���ENC28J60����Ҫ����
    ENC28J60_Set_Bank ( ECON1 );
    // EIE����̫���ж�����Ĵ���
    //     bit 7 INTIE��ȫ��INT�ж�����λ
    //         1 = �����ж��¼�����INT����
    //         0 = ��ֹ����INT���ŵĻ(����ʼ�ձ�����Ϊ�ߵ�ƽ)
    //     bit 6 PKTIE���������ݰ��������ж�����λ
    //         1 = ����������ݰ��������ж�
    //         0 = ��ֹ�������ݰ��������ж�
    ENC28J60_Write_Op ( ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE | EIE_PKTIE | EIE_TXIE | EIE_TXERIE | EIE_RXERIE );
    // bit 2 RXEN������ʹ��λ
    //     1 = ͨ����ǰ�����������ݰ�����д����ջ�����
    //     0 = �������н��յ����ݰ�
    ENC28J60_Write_Op ( ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN );
    printf ( "ENC28J60 Duplex: %s\r\n", ENC28J60_Get_Duplex() ? "Full Duplex" : "Half Duplex" ); // ��ȡ˫����ʽ
    return 0;
}

u8 ENC28J60_Read_Op ( u8 op, u8 addr ) {
    u8 dat = 0;
    ENC28J60_CS = 0;
    dat = op | ( addr & ADDR_MASK );
    SPI1_ReadWriteByte ( dat );
    dat = SPI1_ReadWriteByte ( 0XFF );

    // ����Ƕ�ȡMAC/MII�Ĵ�������ڶ��ζ��������ݲ�����ȷ��
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
    if ( ( bank & BANK_MASK ) != enc28j60_dev.enc28j60bank ) { // �͵�ǰbank��һ�µ�ʱ�������
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
    ENC28J60_Write ( MIREGADR, addr );   // ��MIREGADRд��PHY�Ĵ�����ַ
    ENC28J60_Write ( MIWRL, data );      // д�����ݵĵ�8�ֽ�
    ENC28J60_Write ( MIWRH, data >> 8 ); // д�����ݵĸ�8�ֽ�

    while ( ( ENC28J60_Read ( MISTAT ) &MISTAT_BUSY ) && retry < 0XFFFF ) {
        retry++;
    }
}

u16 ENC28J60_PHY_Read ( u8 addr ) {
    u8 temp;
    u16 phyvalue, retry = 0;
    temp = ENC28J60_Read ( MICMD );
    ENC28J60_Write ( MIREGADR, addr );
    ENC28J60_Write ( MICMD, temp | MICMD_MIIRD ); // ��ʼ��PHY�Ĵ���

    while ( ( ENC28J60_Read ( MISTAT ) &MISTAT_BUSY ) && retry < 0XFFFF ) {
        retry++; // �ȴ���PHY���
    }

    ENC28J60_Write ( MICMD, temp & ( ~MICMD_MIIRD ) ); // ��PHY���
    phyvalue = ENC28J60_Read ( MIRDL );                // ��ȡ��8λ
    phyvalue |= ( ENC28J60_Read ( MIRDH ) << 8 );      // ��ȡ��8λ
    return phyvalue;
}

u8 ENC28J60_Get_EREVID ( void ) {
    // EREVID������ʶ�����ض���Ƭ�İ汾��
    return ENC28J60_Read ( EREVID ) & 0X1F;
}

u16 ENC28J60_Get_Duplex ( void ) {
    u16 temp;
    temp = ENC28J60_PHY_Read ( PHCON1 ) & 0X0100;
    return ( temp >> 8 );
}

void ENC28J60_Packet_Send ( u32 len, u8* packet ) {
    while ( ENC28J60_Read ( ECON1 ) & 0X08 ); // ���ڷ������ݣ��ȴ��������

    // ���÷��ͻ�������ַдָ�����
    ENC28J60_Write ( EWRPTL, TXSTART_INIT & 0XFF );
    ENC28J60_Write ( EWRPTH, TXSTART_INIT >> 8 );
    // ����TXNDָ�룬�Զ�Ӧ���������ݰ���С
    ENC28J60_Write ( ETXNDL, ( TXSTART_INIT + len ) & 0XFF );
    ENC28J60_Write ( ETXNDH, ( TXSTART_INIT + len ) >> 8 );

    // дÿ�������ֽ�
    if ( len > MAX_FRAMELEN ) { // ����֡
        ENC28J60_Write ( MACON3, MACON3_HFRMEN );
        ENC28J60_Write_Op ( ENC28J60_WRITE_BUF_MEM, 0, PKTCTRL_POVERRIDE | PKTCTRL_PHUGEEN );
    } else {
        ENC28J60_Write_Op ( ENC28J60_WRITE_BUF_MEM, 0, 0x00 );
    }

    // �������ݰ������ͻ�����
    ENC28J60_Write_Buf ( len, packet );
    // �������ݵ�����
    ENC28J60_Write_Op ( ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS );

    // ��λ�����߼�������
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

    while ( ( ENC28J60_Read ( ESTAT ) & 0X04 ) ); // ����æ���ȴ��������

    // ���ý��ջ�������ָ��
    ENC28J60_Write ( ERDPTL, ( enc28j60_dev.NextPacketPtr ) );
    ENC28J60_Write ( ERDPTH, ( enc28j60_dev.NextPacketPtr ) >> 8 );
    // ����һ������ָ��
    enc28j60_dev.NextPacketPtr = ENC28J60_Read_Op ( ENC28J60_READ_BUF_MEM, 0 );
    enc28j60_dev.NextPacketPtr |= ENC28J60_Read_Op ( ENC28J60_READ_BUF_MEM, 0 ) << 8;
    // �����ĳ���
    len = ENC28J60_Read_Op ( ENC28J60_READ_BUF_MEM, 0 );
    len |= ENC28J60_Read_Op ( ENC28J60_READ_BUF_MEM, 0 ) << 8;
    len -= 4; // ȥ��CRC����
    // ��ȡ����״̬
    rxstat = ENC28J60_Read_Op ( ENC28J60_READ_BUF_MEM, 0 );
    rxstat |= ENC28J60_Read_Op ( ENC28J60_READ_BUF_MEM, 0 ) << 8;

    // ���ƽ��ճ���
    if ( len > maxlen - 1 ) {
        len = maxlen - 1;
    }

    // ���CRC�ͷ��Ŵ���
    // ERXFCON.CRCENΪĬ�����ã�һ�����ǲ���Ҫ���
    if ( ( rxstat & 0x80 ) == 0 ) {
        len = 0;
    } else {
        ENC28J60_Read_Buf ( len, packet ); // �ӽ��ջ������и������ݰ�
    }

    // RX��ָ���ƶ�����һ�����յ������ݰ��Ŀ�ʼλ�ã����ͷ����ǸղŶ��������ڴ�
    ENC28J60_Write ( ERXRDPTL, ( enc28j60_dev.NextPacketPtr ) );
    ENC28J60_Write ( ERXRDPTH, ( enc28j60_dev.NextPacketPtr ) >> 8 );
    // �ݼ����ݰ���������־�����Ѿ��õ��������
    ENC28J60_Write_Op ( ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC );
    return ( len );
}
