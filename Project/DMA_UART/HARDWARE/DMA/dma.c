#include "dma.h"

DMA_InitTypeDef DMA_InitStructure;

u16 DMA1_MEM_LEN; /* ����DMAÿ�����ݴ��͵ĳ��� */

/* DMA1�ĸ�ͨ������
   DMA_CHx��DMAͨ��CHx��cpar�������ַ
   cmar�Ǵ洢����ַ��   cndtr�����ݴ����� */
void MYDMA_Config ( DMA_Channel_TypeDef* DMA_CHx, u32 cpar, u32 cmar, u16 cndtr ) {
    RCC_AHBPeriphClockCmd ( RCC_AHBPeriph_DMA1, ENABLE ); /* ʹ��DMA���� */
    DMA_DeInit ( DMA_CHx ); /* ��DMA��ͨ��x�Ĵ�������Ϊȱʡֵ */
    DMA1_MEM_LEN = cndtr;
    DMA_InitStructure.DMA_PeripheralBaseAddr = cpar; /* DMA����ADC����ַ */
    DMA_InitStructure.DMA_MemoryBaseAddr = cmar; /* DMA�ڴ����ַ */
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; /* ���ݴ��䷽�򣬴��ڴ��ȡ���͵����� */
    DMA_InitStructure.DMA_BufferSize = cndtr; /* DMAͨ����DMA����Ĵ�С */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; /* �����ַ�Ĵ������� */
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; /* �ڴ��ַ�Ĵ������� */
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; /* ���ݿ��Ϊ8λ */
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; /* ���ݿ��Ϊ8λ */
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; /* ��������������ģʽ */
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; /* DMAͨ��xӵ�������ȼ� */
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; /* DMAͨ��xû������Ϊ�ڴ浽�ڴ洫�� */
    DMA_Init ( DMA_CHx, &DMA_InitStructure );
}

void MYDMA_Enable ( DMA_Channel_TypeDef * DMA_CHx ) { /* ����һ��DMA���� */
    DMA_Cmd ( DMA_CHx, DISABLE ); /* �ر�USART1 TX DMA1��ָʾ��ͨ�� */
    DMA_SetCurrDataCounter ( DMA1_Channel4, DMA1_MEM_LEN ); /* ����DMA����Ĵ�С */
    DMA_Cmd ( DMA_CHx, ENABLE ); /* ʹ��USART1 TX DMA1��ָʾ��ͨ�� */
}
