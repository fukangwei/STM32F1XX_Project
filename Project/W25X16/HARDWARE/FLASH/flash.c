#include "flash.h"
#include "spi.h"
#include "delay.h"

void SPI_Flash_Init ( void ) { /* 4Kbytes为一个Sector，16个扇区为1个Block。W25X16容量为2M字节，共有32个Block，512个Sector */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init ( GPIOA, &GPIO_InitStructure );
    GPIO_SetBits ( GPIOA, GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 );
    SPIx_Init();
}

u8 SPI_Flash_ReadSR ( void ) { /* 读取SPI_FLASH的状态寄存器 */
    u8 byte = 0;
    SPI_FLASH_CS = 0; /* 使能器件 */
    SPIx_ReadWriteByte ( W25X_ReadStatusReg ); /* 发送读取状态寄存器命令 */
    byte = SPIx_ReadWriteByte ( 0Xff ); /* 读取一个字节 */
    SPI_FLASH_CS = 1; /* 取消片选 */
    return byte;
}

void SPI_FLASH_Write_SR ( u8 sr ) { /* 写SPI_FLASH状态寄存器 */
    SPI_FLASH_CS = 0; /* 使能器件 */
    SPIx_ReadWriteByte ( W25X_WriteStatusReg ); /* 发送写取状态寄存器命令 */
    SPIx_ReadWriteByte ( sr ); /* 写入一个字节 */
    SPI_FLASH_CS = 1; /* 取消片选 */
}

void SPI_FLASH_Write_Enable ( void ) { /* SPI_FLASH写使能 */
    SPI_FLASH_CS = 0; /* 使能器件 */
    SPIx_ReadWriteByte ( W25X_WriteEnable ); /* 发送写使能 */
    SPI_FLASH_CS = 1; /* 取消片选 */
}

void SPI_FLASH_Write_Disable ( void ) { /* SPI_FLASH写禁止 */
    SPI_FLASH_CS = 0; /* 使能器件 */
    SPIx_ReadWriteByte ( W25X_WriteDisable ); /* 发送写禁止指令 */
    SPI_FLASH_CS = 1; /* 取消片选 */
}

u16 SPI_Flash_ReadID ( void ) { /* 读取芯片ID，W25X16的ID是0XEF14 */
    u16 Temp = 0;
    SPI_FLASH_CS = 0;
    SPIx_ReadWriteByte ( 0x90 ); /* 发送读取ID命令 */
    SPIx_ReadWriteByte ( 0x00 );
    SPIx_ReadWriteByte ( 0x00 );
    SPIx_ReadWriteByte ( 0x00 );
    Temp |= SPIx_ReadWriteByte ( 0xFF ) << 8;
    Temp |= SPIx_ReadWriteByte ( 0xFF );
    SPI_FLASH_CS = 1;
    return Temp;
}

void SPI_Flash_Read ( u8 *pBuffer, u32 ReadAddr, u16 NumByteToRead ) { /* 在指定地址开始读取指定长度的数据 */
    u16 i;
    SPI_FLASH_CS = 0; /* 使能器件 */
    SPIx_ReadWriteByte ( W25X_ReadData ); /* 发送读取命令 */
    SPIx_ReadWriteByte ( ( u8 ) ( ( ReadAddr ) >> 16 ) ); /* 发送24bit地址 */
    SPIx_ReadWriteByte ( ( u8 ) ( ( ReadAddr ) >> 8 ) );
    SPIx_ReadWriteByte ( ( u8 ) ReadAddr );

    for ( i = 0; i < NumByteToRead; i++ ) {
        pBuffer[i] = SPIx_ReadWriteByte ( 0XFF ); /* 循环读数 */
    }

    SPI_FLASH_CS = 1; /* 取消片选 */
}

/* SPI在一页(0至65535)内写入少于256个字节的数据。参数pBuffer是数据存储区；WriteAddr是开始写入的地址；
   NumByteToWrite是要写入的字节数(最大256)，该数不应该超过该页的剩余字节数 */
void SPI_Flash_Write_Page ( u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite ) {
    u16 i;
    SPI_FLASH_Write_Enable();
    SPI_FLASH_CS = 0; /* 使能器件 */
    SPIx_ReadWriteByte ( W25X_PageProgram ); /* 发送写页命令 */
    SPIx_ReadWriteByte ( ( u8 ) ( ( WriteAddr ) >> 16 ) ); /* 发送24bit地址 */
    SPIx_ReadWriteByte ( ( u8 ) ( ( WriteAddr ) >> 8 ) );
    SPIx_ReadWriteByte ( ( u8 ) WriteAddr );

    for ( i = 0; i < NumByteToWrite; i++ ) {
        SPIx_ReadWriteByte ( pBuffer[i] ); /* 循环写数 */
    }

    SPI_FLASH_CS = 1; /* 取消片选 */
    SPI_Flash_Wait_Busy(); /* 等待写入结束 */
}

/* 无检验写SPI_FLASH，必须确保所写的地址范围内的数据全部为0XFF，否则在非0XFF处写入的数据将失败，具有自动换页功能
   参数pBuffer是数据存储区，WriteAddr是开始写入的地址，NumByteToWrite是要写入的字节数(最大65535) */
void SPI_Flash_Write_NoCheck ( u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite ) {
    u16 pageremain;
    pageremain = 256 - WriteAddr % 256; /* 单页剩余的字节数 */

    if ( NumByteToWrite <= pageremain ) {
        pageremain = NumByteToWrite; /* 不大于256个字节 */
    }

    while ( 1 ) {
        SPI_Flash_Write_Page ( pBuffer, WriteAddr, pageremain );

        if ( NumByteToWrite == pageremain ) {
            break; /* 写入结束了 */
        } else { /* NumByteToWrite > pageremain */
            pBuffer += pageremain;
            WriteAddr += pageremain;
            NumByteToWrite -= pageremain; /* 减去已经写入了的字节数 */

            if ( NumByteToWrite > 256 ) {
                pageremain = 256; /* 一次可以写入256个字节 */
            } else {
                pageremain = NumByteToWrite; /* 不够256个字节了 */
            }
        }
    };
}

u8 SPI_FLASH_BUF[4096];
void SPI_Flash_Write ( u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite ) { /* 在指定地址开始写入指定长度的数据 */
    u32 secpos;
    u16 secoff;
    u16 secremain;
    u16 i;
    secpos = WriteAddr / 4096; /* 扇区地址(0至511 for w25x16) */
    secoff = WriteAddr % 4096; /* 在扇区内的偏移 */
    secremain = 4096 - secoff; /* 扇区剩余空间大小 */

    if ( NumByteToWrite <= secremain ) {
        secremain = NumByteToWrite; /* 不大于4096个字节 */
    }

    while ( 1 ) {
        SPI_Flash_Read ( SPI_FLASH_BUF, secpos * 4096, 4096 ); /* 读出整个扇区的内容 */

        for ( i = 0; i < secremain; i++ ) { /* 校验数据 */
            if ( SPI_FLASH_BUF[secoff + i] != 0XFF ) {
                break; /* 需要擦除 */
            }
        }

        if ( i < secremain ) { /* 需要擦除 */
            SPI_Flash_Erase_Sector ( secpos ); /* 擦除这个扇区 */

            for ( i = 0; i < secremain; i++ ) { /* 复制 */
                SPI_FLASH_BUF[i + secoff] = pBuffer[i];
            }

            SPI_Flash_Write_NoCheck ( SPI_FLASH_BUF, secpos * 4096, 4096 ); /* 写入整个扇区 */
        } else {
            SPI_Flash_Write_NoCheck ( pBuffer, WriteAddr, secremain ); /* 写已经擦除了的，直接写入扇区剩余区间 */
        }

        if ( NumByteToWrite == secremain ) {
            break; /* 写入结束了 */
        } else { /* 写入未结束 */
            secpos++; /* 扇区地址增1 */
            secoff = 0; /* 偏移位置为0 */
            pBuffer += secremain; /* 指针偏移 */
            WriteAddr += secremain; /* 写地址偏移 */
            NumByteToWrite -= secremain; /* 字节数递减 */

            if ( NumByteToWrite > 4096 ) {
                secremain = 4096; /* 下一个扇区还是写不完 */
            } else {
                secremain = NumByteToWrite; /* 下一个扇区可以写完了 */
            }
        }
    };
}

void SPI_Flash_Erase_Chip ( void ) { /* 擦除整个芯片，整片擦除时间：W25X16为25s，W25X32为40s，W25X64为40s */
    SPI_FLASH_Write_Enable();
    SPI_Flash_Wait_Busy();
    SPI_FLASH_CS = 0; /* 使能器件 */
    SPIx_ReadWriteByte ( W25X_ChipErase ); /* 发送片擦除命令 */
    SPI_FLASH_CS = 1; /* 取消片选 */
    SPI_Flash_Wait_Busy(); /* 等待芯片擦除结束 */
}

void SPI_Flash_Erase_Sector ( u32 Dst_Addr ) { /* 擦除一个扇区，参数Dst_Addr是扇区地址，擦除一个扇区的最少时间是150ms */
    Dst_Addr *= 4096;
    SPI_FLASH_Write_Enable();
    SPI_Flash_Wait_Busy();
    SPI_FLASH_CS = 0; /* 使能器件 */
    SPIx_ReadWriteByte ( W25X_SectorErase ); /* 发送扇区擦除指令 */
    SPIx_ReadWriteByte ( ( u8 ) ( ( Dst_Addr ) >> 16 ) ); /* 发送24bit地址 */
    SPIx_ReadWriteByte ( ( u8 ) ( ( Dst_Addr ) >> 8 ) );
    SPIx_ReadWriteByte ( ( u8 ) Dst_Addr );
    SPI_FLASH_CS = 1; /* 取消片选 */
    SPI_Flash_Wait_Busy(); /* 等待擦除完成 */
}

void SPI_Flash_Wait_Busy ( void ) { /* 等待空闲 */
    while ( ( SPI_Flash_ReadSR() & 0x01 ) == 0x01 ); /* 等待BUSY位清空 */
}

void SPI_Flash_PowerDown ( void ) { /* 进入掉电模式 */
    SPI_FLASH_CS = 0; /* 使能器件 */
    SPIx_ReadWriteByte ( W25X_PowerDown ); /* 发送掉电命令 */
    SPI_FLASH_CS = 1; /* 取消片选 */
    delay_us ( 3 ); /* 等待TPD */
}

void SPI_Flash_WAKEUP ( void ) { /* 唤醒 */
    SPI_FLASH_CS = 0; /* 使能器件 */
    SPIx_ReadWriteByte ( W25X_ReleasePowerDown );
    SPI_FLASH_CS = 1; /* 取消片选 */
    delay_us ( 3 ); /* 等待TRES1 */
}
