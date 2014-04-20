/**
 * @file enc28j60.c
 * @brief 
 * @author Li Ming <Lee.dhucst@gmail.com>
 * @url http://blog.csdn.net/lim680
 * @version V1.0
 * @date 2014-04-15
 * @Copyright (c) DHU, CST.  All Rights Reserved
 */

#include "enc28j60.h"
#include "ticks.h"
#include "spi.h"
#include "stdio.h"

static u8 enc28j60_bank;
static u16 next_packet_ptr;

static void en28j60_reset(u8 lag)
{
	spi1_init();
	enc28j60_rst = 0;
	delay_ms(lag);
	enc28j60_rst = 1;
	delay_ms(lag);
}

u8 enc28j60_read_op(u8 cmd, u8 address)
{
	u8 dat = 0;
	
	enc28j60_cs = 0;
	
	dat = cmd|(address&ADDR_MASK);
	spi1_rw_byte(dat);
	dat = spi1_rw_byte(0xff);
	/*
	 *when read MAC and MII Registers, the second byte is the
	 *    data we want
	 */
	if((address&0x80) == 0x80) {
		dat = spi1_rw_byte(0xff);
	}
	enc28j60_cs = 1;
	return dat;
}


void enc28j60_write_op(u8 cmd, u8 addr, u8 data)
{
	u8 dat = 0;
	enc28j60_cs = 0;
	dat = cmd|(addr&ADDR_MASK);
	spi1_rw_byte(dat);
	spi1_rw_byte(data);
	enc28j60_cs = 1;
}

void enc28j60_read_buf(u32 len, u8 *dest)
{
	enc28j60_cs = 0;
	spi1_rw_byte(CMD_READ_BUF_MEM);
	while(len) {
		len--;
		*dest = (u8)spi1_rw_byte(0);
		dest++;
	}
	*dest = '\0';
	enc28j60_cs = 1;
}

void enc28j60_write_buf(u32 len, u8 *src)
{
	enc28j60_cs = 0;
	spi1_rw_byte(CMD_WRITE_BUF_MEM);
	while(len) {
		len--;
		spi1_rw_byte(*src);
		src++;
	}
	enc28j60_cs = 1;
}

u8 enc28j60_read(u8 address)
{
	enc28j60_set_bank(address);
	return enc28j60_read_op(CMD_READ_CTRL_REG, address);
}

void enc28j60_write(u8 address, u8 data)
{
	enc28j60_set_bank(address);
	enc28j60_write_op(CMD_WRITE_CTRL_REG, address, data);
}

void enc28j60_phy_write(u8 address, u16 data)
{
	u16 retry = 0;
	enc28j60_write(MIREGADR, address);
	enc28j60_write(MIWRL, data&0xff);
	enc28j60_write(MIRDH, data>>8);
	while((enc28j60_read(MISTAT)&MISTAT_BUSY) == 0 &&  retry < 500) {
		retry++;
	}
}

void enc28j60_set_bank(u8 address)
{
	/*if current bank not match, then switch it */
	if((address & BANK_MASK) != enc28j60_bank) {
		enc28j60_write_op(CMD_BIT_FIELD_CLR, ECON1, ECON1_BSEL1|ECON1_BSEL0);
		enc28j60_write_op(CMD_BIT_FIELD_SET, ECON1, (address&BANK_MASK)>>5);
		enc28j60_bank = (address&BANK_MASK);
	}
}



void enc28j60_init(u8 *mac_addr)
{
	u16 retry = 0;
	
	/**
	 * @brief reset en28j60
	 * @Param 10  delay 10ms when after reset
	 */
	en28j60_reset(10);

	enc28j60_write_op(CMD_SOFT_RESET, 0, CMD_SOFT_RESET);

	while(!(enc28j60_read(ESTAT)&ESTAT_CLKRDY) && retry < 250) {
		retry++;
		delay_ms(1);
	}

	if(retry >= 250) {
	//	return 1;
	}

	/*set receive buffer start address*/
	next_packet_ptr = RXBUF_START;

//bank0 register initial {
	/*set receive buffer start location*/
	enc28j60_write(ERXSTL, RXBUF_START&0xFF);
	enc28j60_write(ERXSTH, RXBUF_START>>8);
	/*set receive buffer read pointer*/
	enc28j60_write(ERXRDPTL, RXBUF_START&0xFF); 
	enc28j60_write(ERXRDPTH, RXBUF_START>>8);
	/*set receive buffer end locatin*/
	enc28j60_write(ERXNDL, RXBUF_END&0xFF);
	enc28j60_write(ERXNDH, RXBUF_END>>8);
	/*set transmit buffer start location*/
	enc28j60_write(ETXSTL, TXBUF_START&0xFF);
	enc28j60_write(ETXSTH, TXBUF_START>>8);
	/*set transmit buffer end location*/
	enc28j60_write(ETXNDL, TXBUF_END&0xFF);
	enc28j60_write(ETXNDH, TXBUF_END>>8);
//} 

// bank1 register initial {
	/**
	 * @brief 
	 *   packets with a destination address matching the local MAC
	 *   addrees will be accepted.
	 *   All packets with an invalid CRC will be discarded.
	 *   Packets meet the pattern match criteria will be accepted.
	 *
	 * @Param ERXFCON  ethernet receive filter control register address
	 * @Param ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN
	 */
	enc28j60_write(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN);
	/*
	 * enable getting arp packets
	 * frame type : arp  0x0806
	 * ds : ff ff ff ff ff ff
	 */
	enc28j60_write(EPMM0,  0x3f);
	enc28j60_write(EPMM1,  0x30);
	enc28j60_write(EPMCSL, 0xf9);
	enc28j60_write(EPMCSH, 0xf7);
//}

// bank2 register initial {
	enc28j60_write(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);
	enc28j60_write(MACON2, 0x00);
	
	/**
	 * @brief 1. PADCFG2:PADCFG0 :001 all short frames will be zero padded 
	 * to 60 bytes and a valid CRC will then be appended
	 *        2.Transmit CRC enable
	 *        3.frame length checking enable
	 *        4.mac full-duplex enable
	 * @Param CMD_BIT_FIELD_CLR
	 * @Param MACON3
	 * @Param MACON3_FULDPX|MACON3_FRMLNEN
	 */
	enc28j60_write_op(CMD_BIT_FIELD_SET, MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|
					  MACON3_FULDPX|MACON3_FRMLNEN);
	
	/*configure non-back-to-back inter-packet gap*/
	enc28j60_write(MAIPGL, 0x12);
	enc28j60_write(MAIPGH, 0x0C);
	/*configure back-to-bak inter-packet gap*/
	enc28j60_write(MABBIPG, 0x15);
	/*set the maxmium frame length */
	enc28j60_write(MAMXFLL, MAX_FRAMELEN&0xFF);
	enc28j60_write(MAMXFLH, MAX_FRAMELEN>>8);
//}

// bank3 register initial {
	/*write mac address*/
	enc28j60_write(MAADR5, mac_addr[0]);
	enc28j60_write(MAADR4, mac_addr[1]);
	enc28j60_write(MAADR3, mac_addr[2]);
	enc28j60_write(MAADR2, mac_addr[3]);
	enc28j60_write(MAADR1, mac_addr[4]);
	enc28j60_write(MAADR0, mac_addr[5]);
	
	/**
	 *  full-duplex
	 *  disable loopback of transmit
	 */
	enc28j60_phy_write(PHCON1, PHCON1_PDPXMD);
	enc28j60_phy_write(PHCON2, PHCON2_HDLDIS);

	enc28j60_set_bank(ECON1);
	enc28j60_write_op(CMD_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);
	enc28j60_write_op(CMD_BIT_FIELD_SET, ECON1, ECON1_RXEN);
//}
	
	if(enc28j60_read(MAADR5) == mac_addr[0]) {
		lwip_log("yes mac set right, %d\r\n", enc28j60_read(MAADR5));
	}
}

void enc28j60_send_packet(u32 len, u8 *packet)
{
	/*set the mcu write buffer pointer*/
	enc28j60_write(EWRPTL, TXBUF_START&0xff);
	enc28j60_write(EWRPTH, TXBUF_START>>8);

	/*set the waiting transmit buffer over address*/
	enc28j60_write(ETXNDL, (TXBUF_START + len)&0xff);
	enc28j60_write(ETXNDH, (TXBUF_START + len)>>8);
	
	/*sending the packet control byte*/
	enc28j60_write_op(CMD_WRITE_BUF_MEM, 0, 0x00);
	/*write packet to transmit buffer*/
	enc28j60_write_buf(len, packet);
	/*starting send packet*/
	enc28j60_write_op(CMD_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
	if((enc28j60_read(EIR)&EIR_TXERIF) == 1) {
		enc28j60_write_op(CMD_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
	}

}

u16 enc28j60_receive_packet(u32 max_len, u8 *packet)
{
	u16 rx_state;
	u16 len;
	
	/*check whether it received packets or not*/
	if(enc28j60_read(EPKTCNT) == 0) {
		return 0;
	}
	
	/*set the pointer to the start of received packet*/
	enc28j60_write(ERDPTL, next_packet_ptr&0xff);
	enc28j60_write(ERDPTH, next_packet_ptr>>8);
	
	/*read next packet pointer*/
	next_packet_ptr = enc28j60_read_op(CMD_READ_BUF_MEM, 0);
	next_packet_ptr |= enc28j60_read_op(CMD_READ_BUF_MEM, 0)<<8;

	/*get the packet lenth, remove CRC*/
	len = enc28j60_read_op(CMD_READ_BUF_MEM, 0);
	len |= enc28j60_read_op(CMD_READ_BUF_MEM, 0)<<8;
	len -= 4;

	rx_state = enc28j60_read_op(CMD_READ_BUF_MEM, 0);
	rx_state |= enc28j60_read_op(CMD_READ_BUF_MEM, 0)<<8;
	
	if(len > max_len - 1) {
		len = max_len - 1;
	}

	/*check CRC and symbol erros*/
	if((rx_state&0x80) == 0) {
		len = 0;
	}
	else {
		enc28j60_read_buf(len, packet);
	}
	
	/*free the memory we have readed*/
	enc28j60_write(ERXRDPTL, next_packet_ptr&0xff);
	enc28j60_write(ERXRDPTH, next_packet_ptr>>8);
	
	/*decrease the packet counter*/
	enc28j60_write_op(CMD_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);

	return len;
}
