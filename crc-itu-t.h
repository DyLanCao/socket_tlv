#ifndef CRC_ITU_T_H
#define CRC_ITU_T_H

#define MAGIC_CRC 0x1E50

extern const unsigned short crc_itu_t_table[256];

extern unsigned short crc_itu_t(unsigned short magic_crc,const unsigned char *buffer,unsigned int len);

extern int ushort_to_bytes(unsigned char *bytes,unsigned short val);

extern unsigned short bytes_to_ushort(unsigned char *bytes,int len);

static inline unsigned short crc_itu_t_byte(unsigned short crc,const unsigned char data)
{
	return (crc << 8) ^ crc_itu_t_table[((crc >> 8) ^ data) & 0xff];
}
#endif
