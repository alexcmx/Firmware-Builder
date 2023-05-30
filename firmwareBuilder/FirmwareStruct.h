/*
 * FirmwareStruct.h
 *
 *  Created on: May 24, 2023
 *      Author: user
 */

#ifndef INC_FIRMWARESTRUCT_H_
#define INC_FIRMWARESTRUCT_H_
#include <stdint.h>

struct FirmwareHeader{
	uint16_t version;
	uint16_t num_blocks;
	uint8_t blocksize;
	uint8_t crc;
};

struct FirmwareBlock {
    uint8_t* data;
    uint8_t crc;
};

void change_ending(FirmwareHeader* header)
{
    header->version = (header->version << 8) + (header->version >> 8);
    header->num_blocks = (header->num_blocks << 8) + (header->num_blocks >> 8);
}

uint8_t gencrc(void *data, size_t len)
{
    uint8_t crc = 0xff;
    size_t i, j;
    for (i = 0; i < len; i++) {
        crc ^= ((uint8_t *)data)[i];
        for (j = 0; j < 8; j++) {
            if ((crc & 0x80) != 0)
                crc = (uint8_t)((crc << 1) ^ 0x31);
            else
                crc <<= 1;
        }
    }
    return crc;
}

void calc_crc_header(FirmwareHeader* header)
{
	header->crc = gencrc(header, sizeof(*header) - 1);
}
void calc_crc_block(FirmwareBlock* ptr, uint8_t size) {
    ptr->crc = gencrc(ptr->data, size);
}

bool check_crc_header(FirmwareHeader* header)
{
	return 0x00 == gencrc(header, sizeof(*header));
}

#endif /* INC_FIRMWARESTRUCT_H_ */
