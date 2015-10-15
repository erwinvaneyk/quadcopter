#ifndef PKT_CHECKSUM_H_
#define PKT_CHECKSUM_H_

#define VERIFY_CHECKSUM(modecommand, data1, data2, data3, data4, checksum)\
			modecommand ^ data1 ^ data2 ^ data3 ^ data4 ^ checksum;

#define CALCULATE_CHECKSUM(modecommand, data)\
			modecommand ^ (data & 0x000000FF) ^ (data >> 8 & 0x000000FF) \
						^ (data >> 16 & 0x000000FF) ^ (data >> 24 & 0x000000FF);

#endif