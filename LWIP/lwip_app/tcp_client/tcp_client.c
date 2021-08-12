#include "tcp_client.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "stdio.h"
#include "string.h"

u8 tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE]; /* TCP Client�������ݻ����� */
u8 tcp_client_sendbuf[TCP_CLIENT_TX_BUFSIZE]; /* TCP Client�������ݻ����� */

//TCP Client ����ȫ��״̬��Ǳ���
//bit7:0,û������Ҫ����;1,������Ҫ����
//bit6:0,û���յ�����;1,�յ�������.
//bit5:0,û�������Ϸ�����;1,�����Ϸ�������.
//bit4~0:����
u8 tcp_client_flag = 0;

/* TCP���ӽ�������õĻص����� */
err_t tcp_client_connected ( void *arg, struct tcp_pcb *tpcb, err_t err ) {
    struct tcp_client_struct *es = NULL;

    if ( err == ERR_OK ) {
        es = ( struct tcp_client_struct* ) mem_malloc ( sizeof ( struct tcp_client_struct ) );

        if ( es ) { /* �ڴ�����ɹ� */
            es->state = ES_TCPCLIENT_CONNECTED; /* ״̬Ϊ���ӳɹ� */
            es->pcb = tpcb;
            es->p = NULL;
            tcp_arg ( tpcb, es );                  // ʹ��es����tpcb��callback_arg
            tcp_recv ( tpcb, tcp_client_recv );    // ��ʼ��LwIP��tcp_recv�ص�����
            tcp_err ( tpcb, tcp_client_error );    // ��ʼ��tcp_err�ص�����
            tcp_sent ( tpcb, tcp_client_sent );    // ��ʼ��LwIP��tcp_sent�ص�����
            tcp_poll ( tpcb, tcp_client_poll, 1 ); // ��ʼ��LwIP��tcp_poll�ص�����
            tcp_client_flag |= LWIP_CONNECTED;     // ������ӵ���������
            err = ERR_OK;
        } else {
            tcp_client_connection_close ( tpcb, es ); // �ر�����
            err = ERR_MEM; // �ڴ�������
        }
    } else {
        tcp_client_connection_close ( tpcb, 0 ); // �ر�����
    }

    return err;
}

err_t tcp_client_recv ( void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err ) {
    u32 data_len = 0;
    struct pbuf *q;
    struct tcp_client_struct *es;
    err_t ret_err;
    LWIP_ASSERT ( "arg != NULL", arg != NULL );
    es = ( struct tcp_client_struct * ) arg;

    if ( p == NULL ) { // ����ӷ��������յ��յ�����֡���͹ر�����
        es->state = ES_TCPCLIENT_CLOSING; // ��Ҫ�ر�TCP����
        es->p = p;
        ret_err = ERR_OK;
    } else if ( err != ERR_OK ) { // ���յ�һ���ǿյ�����֡������err != ERR_OK
        if ( p ) {
            pbuf_free ( p ); // �ͷŽ���pbuf
        }

        ret_err = err;
    } else if ( es->state == ES_TCPCLIENT_CONNECTED ) { // ����������״̬ʱ
        if ( p != NULL ) { // ����������״̬�����ҽ��յ������ݲ�Ϊ��
            memset ( tcp_client_recvbuf, 0, TCP_CLIENT_RX_BUFSIZE ); // ���ݽ��ջ���������

            for ( q = p; q != NULL; q = q->next ) { // ����������pbuf����
                if ( q->len > ( TCP_CLIENT_RX_BUFSIZE - data_len ) ) {
                    memcpy ( tcp_client_recvbuf + data_len, q->payload, ( TCP_CLIENT_RX_BUFSIZE - data_len ) );
                } else {
                    memcpy ( tcp_client_recvbuf + data_len, q->payload, q->len );
                }

                data_len += q->len;

                if ( data_len > TCP_CLIENT_RX_BUFSIZE ) {
                    break;
                }
            }

            tcp_client_flag |= LWIP_NEW_DATA; // ��ǽ��յ�������
            tcp_recved ( tpcb, p->tot_len ); // ���ڻ�ȡ�������ݣ�֪ͨLwIP���Ի�ȡ��������
            pbuf_free ( p ); // �ͷ��ڴ�
            ret_err = ERR_OK;
        }
    } else { // ���յ����ݣ����������Ѿ��ر�
        tcp_recved ( tpcb, p->tot_len ); // ���ڻ�ȡ�������ݣ�֪ͨLwIP���Ի�ȡ��������
        es->p = NULL;
        pbuf_free ( p ); // �ͷ��ڴ�
        ret_err = ERR_OK;
    }

    return ret_err;
}

void tcp_client_error ( void *arg, err_t err ) {
}

err_t tcp_client_poll ( void *arg, struct tcp_pcb *tpcb ) {
    err_t ret_err;
    struct tcp_client_struct *es;
    es = ( struct tcp_client_struct* ) arg;

    if ( es != NULL ) { // ���Ӵ��ڿ��У����Է�������
        if ( tcp_client_flag & LWIP_SEND_DATA ) { // �ж��Ƿ�������Ҫ����
            es->p = pbuf_alloc ( PBUF_TRANSPORT, strlen ( ( char* ) tcp_client_sendbuf ), PBUF_POOL );
            pbuf_take ( es->p, ( char* ) tcp_client_sendbuf, strlen ( ( char* ) tcp_client_sendbuf ) );
            tcp_client_senddata ( tpcb, es );
            tcp_client_flag &= ~LWIP_SEND_DATA; // ������ݷ��ͱ�־

            if ( es->p ) {
                pbuf_free ( es->p );
            }
        } else if ( es->state == ES_TCPCLIENT_CLOSING ) {
            tcp_client_connection_close ( tpcb, es ); /* �ر�TCP���� */
        }

        ret_err = ERR_OK;
    } else {
        tcp_abort ( tpcb ); /* ��ֹ���ӣ���ɾ��pcb���ƿ� */
        ret_err = ERR_ABRT;
    }

    return ret_err;
}

/* tcp_sent�Ļص�����������Զ���������յ�ACK�źź������� */
err_t tcp_client_sent ( void *arg, struct tcp_pcb *tpcb, u16_t len ) {
    struct tcp_client_struct *es;
    LWIP_UNUSED_ARG ( len );
    es = ( struct tcp_client_struct* ) arg;

    if ( es->p ) {
        tcp_client_senddata ( tpcb, es ); /* �������� */
    }

    return ERR_OK;
}

void tcp_client_senddata ( struct tcp_pcb *tpcb, struct tcp_client_struct * es ) {
    struct pbuf *ptr;
    err_t wr_err = ERR_OK;

    while ( ( wr_err == ERR_OK ) && es->p && ( es->p->len <= tcp_sndbuf ( tpcb ) ) ) {
        ptr = es->p;
        /* ��Ҫ���͵����ݼ��뵽���ͻ�������� */
        wr_err = tcp_write ( tpcb, ptr->payload, ptr->len, 1 );

        if ( wr_err == ERR_OK ) {
            es->p = ptr->next; /* ָ����һ��pbuf */

            if ( es->p ) {
                pbuf_ref ( es->p ); /* pbuf��ref��1 */
            }

            pbuf_free ( ptr );
        } else if ( wr_err == ERR_MEM ) {
            es->p = ptr;
        }

        tcp_output ( tpcb ); /* �����ͻ�������е������������ͳ�ȥ */
    }
}

/* �ر�������������� */
void tcp_client_connection_close ( struct tcp_pcb *tpcb, struct tcp_client_struct * es ) {
    /* �Ƴ��ص����� */
    tcp_abort ( tpcb ); /* ��ֹ���ӣ���ɾ��pcb���ƿ� */
    tcp_arg ( tpcb, NULL );
    tcp_recv ( tpcb, NULL );
    tcp_sent ( tpcb, NULL );
    tcp_err ( tpcb, NULL );
    tcp_poll ( tpcb, NULL, 0 );

    if ( es ) {
        mem_free ( es );
    }

    tcp_client_flag &= ~LWIP_CONNECTED; /* ������ӶϿ��� */
}
