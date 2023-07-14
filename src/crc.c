/* crc.c
 * Implementation of parametric CRC-8, CRC-16, CRC-32, and CRC-64 redunancy
 * checks in ANSI C. Parameters from the examples were taken from this link:
 * https://reveng.sourceforge.io/crc-catalogue/all.htm
 * Every resulting CRC value should be the one defined as its 'check' value in
 * the catalogue, as the check value means the CRC output for the '123456789'
 * string.
 * Author: Dénes Fintha
 * Year: 2023
 * -------------------------------------------------------------------------- */

/* Interface */

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t polynomial;
    uint8_t initial_value;
    uint8_t output_xor;
    int reflect_input;
    int reflect_output;
} crc8_parameters;

typedef struct {
    uint16_t polynomial;
    uint16_t initial_value;
    uint16_t output_xor;
    int reflect_input;
    int reflect_output;
} crc16_parameters;

typedef struct {
    uint32_t polynomial;
    uint32_t initial_value;
    uint32_t output_xor;
    int reflect_input;
    int reflect_output;
} crc32_parameters;

typedef struct {
    uint64_t polynomial;
    uint64_t initial_value;
    uint64_t output_xor;
    int reflect_input;
    int reflect_output;
} crc64_parameters;

uint8_t crc8(
    const void *data,
    size_t length,
    const crc8_parameters *parameters
);

uint16_t crc16(
    const void *data,
    size_t length,
    const crc16_parameters *parameters
);

uint32_t crc32(
    const void *data,
    size_t length,
    const crc32_parameters *parameters
);

uint64_t crc64(
    const void *data,
    size_t length,
    const crc64_parameters *parameters
);

/* Implementation */

static uint8_t reflect_u8(uint8_t value) {
    return ((value & 0x80) >> 7) |
           ((value & 0x40) >> 5) |
           ((value & 0x20) >> 3) |
           ((value & 0x10) >> 1) |
           ((value & 0x08) << 1) |
           ((value & 0x04) << 3) |
           ((value & 0x02) << 5) |
           ((value & 0x01) << 7);
}

static uint16_t reflect_u16(uint16_t value) {
    uint8_t i;
    uint16_t result = 0x00;
    for (i = 0; i < 16; ++i) {
        if ((value & (((uint16_t) 0x01) << i)) != 0)
            result |= (((uint16_t) 0x01) << (15 - i));
    }
    return result;
}

static uint32_t reflect_u32(uint32_t value) {
    uint8_t i;
    uint32_t result = 0x00;
    for (i = 0; i < 32; ++i) {
        if ((value & (((uint32_t) 0x01) << i)) != 0)
            result |= (((uint32_t) 0x01) << (31 - i));
    }
    return result;
}

static uint64_t reflect_u64(uint64_t value) {
    uint8_t i;
    uint64_t result = 0x00;
    for (i = 0; i < 64; ++i) {
        if ((value & (((uint64_t) 0x01) << i)) != 0)
            result |= (((uint64_t) 0x01) << (63 - i));
    }
    return result;
}

uint8_t crc8(
    const void *data,
    size_t length,
    const crc8_parameters *parameters
) {
    const uint8_t *work = (const uint8_t *) data;
    uint8_t result, temp;
    size_t i, j;

    if (length == 0)
        return parameters->initial_value;

    result = parameters->initial_value;
    for (i = 0; i < length; ++i) {
        temp = parameters->reflect_input ? reflect_u8(work[i]) : work[i];
        result ^= temp;
        for (j = 0; j < 8; ++j) {
            result = ((result & 0x80) != 0)
                ? ((result << 1) ^ parameters->polynomial)
                : (result << 1);
        }
    }

    if (parameters->reflect_output)
        result = reflect_u8(result);
    return result ^ parameters->output_xor;
}

uint16_t crc16(
    const void *data,
    size_t length,
    const crc16_parameters *parameters
) {
    const uint8_t *work = (const uint8_t *) data;
    uint16_t result, temp;
    size_t i, j;

    if (length == 0)
        return parameters->initial_value;

    result = parameters->initial_value;
    for (i = 0; i < length; ++i) {
        temp = parameters->reflect_input ? reflect_u8(work[i]) : work[i];
        result ^= (temp << 8);
        for (j = 0; j < 8; ++j) {
            result = ((result & 0x8000) != 0)
                ? ((result << 1) ^ parameters->polynomial)
                : (result << 1);
        }
    }

    if (parameters->reflect_output)
        result = reflect_u16(result);
    return result ^ parameters->output_xor;
}

uint32_t crc32(
    const void *data,
    size_t length,
    const crc32_parameters *parameters
) {
    const uint8_t* work = (const uint8_t*) data;
    uint32_t result, temp;
    size_t i, j;

    if (length == 0)
        return parameters->initial_value;

    result = parameters->initial_value;
    for (i = 0; i < length; ++i) {
        temp = parameters->reflect_input ? reflect_u8(work[i]) : work[i];
        result ^= (temp << 24);
        for (j = 0; j < 8; ++j) {
            result = ((result & 0x80000000) != 0)
                ? ((result << 1) ^ parameters->polynomial)
                : (result << 1);
        }
    }

    if (parameters->reflect_output)
        result = reflect_u32(result);
    return result ^ parameters->output_xor;
}

uint64_t crc64(
    const void *data,
    size_t length,
    const crc64_parameters *parameters
) {
    const uint8_t* work = (const uint8_t*) data;
    uint64_t result, temp;
    size_t i, j;

    if (length == 0)
        return parameters->initial_value;

    result = parameters->initial_value;
    for (i = 0; i < length; ++i) {
        temp = parameters->reflect_input ? reflect_u8(work[i]) : work[i];
        result ^= (temp << 56);
        for (j = 0; j < 8; ++j) {
            result = ((result & 0x8000000000000000) != 0)
                ? ((result << 1) ^ parameters->polynomial)
                : (result << 1);
        }
    }

    if (parameters->reflect_output)
        result = reflect_u64(result);
    return result ^ parameters->output_xor;
}

/* Demonstration */

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

static void crc8_test(const char *data) {
    const size_t length = strlen(data);
    crc8_parameters crc8_plain_parameters;
    crc8_parameters crc8_cdma2000_parameters;
    crc8_parameters crc8_darc_parameters;
    crc8_parameters crc8_dvbs2_parameters;
    crc8_parameters crc8_ebu_parameters;
    crc8_parameters crc8_icode_parameters;
    crc8_parameters crc8_itu_parameters;
    crc8_parameters crc8_maxim_parameters;
    crc8_parameters crc8_rohc_parameters;
    crc8_parameters crc8_wcdma_parameters;

    crc8_plain_parameters.initial_value = 0x00;
    crc8_plain_parameters.output_xor = 0x00;
    crc8_plain_parameters.polynomial = 0x07;
    crc8_plain_parameters.reflect_input = 0;
    crc8_plain_parameters.reflect_output = 0;
    crc8_cdma2000_parameters.initial_value = 0xFF;
    crc8_cdma2000_parameters.output_xor = 0x00;
    crc8_cdma2000_parameters.polynomial = 0x9B;
    crc8_cdma2000_parameters.reflect_input = 0;
    crc8_cdma2000_parameters.reflect_output = 0;
    crc8_darc_parameters.initial_value = 0x00;
    crc8_darc_parameters.output_xor = 0x00;
    crc8_darc_parameters.polynomial = 0x39;
    crc8_darc_parameters.reflect_input = 1;
    crc8_darc_parameters.reflect_output = 1;
    crc8_dvbs2_parameters.initial_value = 0x00;
    crc8_dvbs2_parameters.output_xor = 0x00;
    crc8_dvbs2_parameters.polynomial = 0xD5;
    crc8_dvbs2_parameters.reflect_input = 0;
    crc8_dvbs2_parameters.reflect_output = 0;
    crc8_ebu_parameters.initial_value = 0xFF;
    crc8_ebu_parameters.output_xor = 0x00;
    crc8_ebu_parameters.polynomial = 0x1D;
    crc8_ebu_parameters.reflect_input = 1;
    crc8_ebu_parameters.reflect_output = 1;
    crc8_icode_parameters.initial_value = 0xFD;
    crc8_icode_parameters.output_xor = 0x00;
    crc8_icode_parameters.polynomial = 0x1D;
    crc8_icode_parameters.reflect_input = 0;
    crc8_icode_parameters.reflect_output = 0;
    crc8_itu_parameters.initial_value = 0x00;
    crc8_itu_parameters.output_xor = 0x55;
    crc8_itu_parameters.polynomial = 0x07;
    crc8_itu_parameters.reflect_input = 0;
    crc8_itu_parameters.reflect_output = 0;
    crc8_maxim_parameters.initial_value = 0x00;
    crc8_maxim_parameters.output_xor = 0x00;
    crc8_maxim_parameters.polynomial = 0x31;
    crc8_maxim_parameters.reflect_input = 1;
    crc8_maxim_parameters.reflect_output = 1;
    crc8_rohc_parameters.initial_value = 0xFF;
    crc8_rohc_parameters.output_xor = 0x00;
    crc8_rohc_parameters.polynomial = 0x07;
    crc8_rohc_parameters.reflect_input = 1;
    crc8_rohc_parameters.reflect_output = 1;
    crc8_wcdma_parameters.initial_value = 0x00;
    crc8_wcdma_parameters.output_xor = 0x00;
    crc8_wcdma_parameters.polynomial = 0x9B;
    crc8_wcdma_parameters.reflect_input = 1;
    crc8_wcdma_parameters.reflect_output = 1;

    printf(
        "CRC-8:              0x%02" PRIX8 "\n",
        crc8(data, length, &crc8_plain_parameters)
    );
    printf(
        "CRC-8/CDMA2000:     0x%02" PRIX8 "\n",
        crc8(data, length, &crc8_cdma2000_parameters)
    );
    printf(
        "CRC-8/DARC:         0x%02" PRIX8 "\n",
        crc8(data, length, &crc8_darc_parameters)
    );
    printf(
        "CRC-8/DVB-S2:       0x%02" PRIX8 "\n",
        crc8(data, length, &crc8_dvbs2_parameters)
    );
    printf(
        "CRC-8/EBU:          0x%02" PRIX8 "\n",
        crc8(data, length, &crc8_ebu_parameters)
    );
    printf(
        "CRC-8/I-CODE:       0x%02" PRIX8 "\n",
        crc8(data, length, &crc8_icode_parameters)
    );
    printf(
        "CRC-8/ITU:          0x%02" PRIX8 "\n",
        crc8(data, length, &crc8_itu_parameters)
    );
    printf(
        "CRC-8/MAXIM:        0x%02" PRIX8 "\n",
        crc8(data, length, &crc8_maxim_parameters)
    );
    printf(
        "CRC-8/ROHC:         0x%02" PRIX8 "\n",
        crc8(data, length, &crc8_rohc_parameters)
    );
    printf(
        "CRC-8/WCDMA:        0x%02" PRIX8 "\n",
        crc8(data, length, &crc8_wcdma_parameters)
    );
}

static void crc16_test(const char* data) {
    const size_t length = strlen(data);
    crc16_parameters crc16_arc_parameters;
    crc16_parameters crc16_augccitt_parameters;
    crc16_parameters crc16_buypass_parameters;
    crc16_parameters crc16_ccittfalse_parameters;
    crc16_parameters crc16_cdma2000_parameters;
    crc16_parameters crc16_dds110_parameters;
    crc16_parameters crc16_dectr_parameters;
    crc16_parameters crc16_dectx_parameters;
    crc16_parameters crc16_dnp_parameters;
    crc16_parameters crc16_en13757_parameters;
    crc16_parameters crc16_genibus_parameters;
    crc16_parameters crc16_kermit_parameters;
    crc16_parameters crc16_maxim_parameters;
    crc16_parameters crc16_mcrf4xx_parameters;
    crc16_parameters crc16_modbus_parameters;
    crc16_parameters crc16_riello_parameters;
    crc16_parameters crc16_t10dif_parameters;
    crc16_parameters crc16_teledisk_parameters;
    crc16_parameters crc16_tms37157_parameters;
    crc16_parameters crc16_usb_parameters;
    crc16_parameters crc16_x25_parameters;
    crc16_parameters crc16_xmodem_parameters;
    crc16_parameters crc16_crca_parameters;

    crc16_arc_parameters.initial_value = 0x0000;
    crc16_arc_parameters.output_xor = 0x0000;
    crc16_arc_parameters.polynomial = 0x8005;
    crc16_arc_parameters.reflect_input = 1;
    crc16_arc_parameters.reflect_output = 1;
    crc16_augccitt_parameters.initial_value = 0x1D0F;
    crc16_augccitt_parameters.output_xor = 0x0000;
    crc16_augccitt_parameters.polynomial = 0x1021;
    crc16_augccitt_parameters.reflect_input = 0;
    crc16_augccitt_parameters.reflect_output = 0;
    crc16_buypass_parameters.initial_value = 0x0000;
    crc16_buypass_parameters.output_xor = 0x0000;
    crc16_buypass_parameters.polynomial = 0x8005;
    crc16_buypass_parameters.reflect_input = 0;
    crc16_buypass_parameters.reflect_output = 0;
    crc16_ccittfalse_parameters.initial_value = 0xFFFF;
    crc16_ccittfalse_parameters.output_xor = 0x0000;
    crc16_ccittfalse_parameters.polynomial = 0x1021;
    crc16_ccittfalse_parameters.reflect_input = 0;
    crc16_ccittfalse_parameters.reflect_output = 0;
    crc16_cdma2000_parameters.initial_value = 0xFFFF;
    crc16_cdma2000_parameters.output_xor = 0x0000;
    crc16_cdma2000_parameters.polynomial = 0xC867;
    crc16_cdma2000_parameters.reflect_input = 0;
    crc16_cdma2000_parameters.reflect_output = 0;
    crc16_dds110_parameters.initial_value = 0x800D;
    crc16_dds110_parameters.output_xor = 0x0000;
    crc16_dds110_parameters.polynomial = 0x8005;
    crc16_dds110_parameters.reflect_input = 0;
    crc16_dds110_parameters.reflect_output = 0;
    crc16_dectr_parameters.initial_value = 0x0000;
    crc16_dectr_parameters.output_xor = 0x0001;
    crc16_dectr_parameters.polynomial = 0x0589;
    crc16_dectr_parameters.reflect_input = 0;
    crc16_dectr_parameters.reflect_output = 0;
    crc16_dectx_parameters.initial_value = 0x0000;
    crc16_dectx_parameters.output_xor = 0x0000;
    crc16_dectx_parameters.polynomial = 0x0589;
    crc16_dectx_parameters.reflect_input = 0;
    crc16_dectx_parameters.reflect_output = 0;
    crc16_dnp_parameters.initial_value = 0x0000;
    crc16_dnp_parameters.output_xor = 0xFFFF;
    crc16_dnp_parameters.polynomial = 0x3D65;
    crc16_dnp_parameters.reflect_input = 1;
    crc16_dnp_parameters.reflect_output = 1;
    crc16_en13757_parameters.initial_value = 0x0000;
    crc16_en13757_parameters.output_xor = 0xFFFF;
    crc16_en13757_parameters.polynomial = 0x3D65;
    crc16_en13757_parameters.reflect_input = 0;
    crc16_en13757_parameters.reflect_output = 0;
    crc16_genibus_parameters.initial_value = 0xFFFF;
    crc16_genibus_parameters.output_xor = 0xFFFF;
    crc16_genibus_parameters.polynomial = 0x1021;
    crc16_genibus_parameters.reflect_input = 0;
    crc16_genibus_parameters.reflect_output = 0;
    crc16_kermit_parameters.initial_value = 0x0000;
    crc16_kermit_parameters.output_xor = 0x0000;
    crc16_kermit_parameters.polynomial = 0x1021;
    crc16_kermit_parameters.reflect_input = 1;
    crc16_kermit_parameters.reflect_output = 1;
    crc16_maxim_parameters.initial_value = 0x0000;
    crc16_maxim_parameters.output_xor = 0xFFFF;
    crc16_maxim_parameters.polynomial = 0x8005;
    crc16_maxim_parameters.reflect_input = 1;
    crc16_maxim_parameters.reflect_output = 1;
    crc16_mcrf4xx_parameters.initial_value = 0xFFFF;
    crc16_mcrf4xx_parameters.output_xor = 0x0000;
    crc16_mcrf4xx_parameters.polynomial = 0x1021;
    crc16_mcrf4xx_parameters.reflect_input = 1;
    crc16_mcrf4xx_parameters.reflect_output = 1;
    crc16_modbus_parameters.initial_value = 0xFFFF;
    crc16_modbus_parameters.output_xor = 0x0000;
    crc16_modbus_parameters.polynomial = 0x8005;
    crc16_modbus_parameters.reflect_input = 1;
    crc16_modbus_parameters.reflect_output = 1;
    crc16_riello_parameters.initial_value = 0xB2AA;
    crc16_riello_parameters.output_xor = 0x0000;
    crc16_riello_parameters.polynomial = 0x1021;
    crc16_riello_parameters.reflect_input = 1;
    crc16_riello_parameters.reflect_output = 1;
    crc16_t10dif_parameters.initial_value = 0x0000;
    crc16_t10dif_parameters.output_xor = 0x0000;
    crc16_t10dif_parameters.polynomial = 0x8BB7;
    crc16_t10dif_parameters.reflect_input = 0;
    crc16_t10dif_parameters.reflect_output = 0;
    crc16_teledisk_parameters.initial_value = 0x0000;
    crc16_teledisk_parameters.output_xor = 0x0000;
    crc16_teledisk_parameters.polynomial = 0xA097;
    crc16_teledisk_parameters.reflect_input = 0;
    crc16_teledisk_parameters.reflect_output = 0;
    crc16_tms37157_parameters.initial_value = 0x89EC;
    crc16_tms37157_parameters.output_xor = 0x0000;
    crc16_tms37157_parameters.polynomial = 0x1021;
    crc16_tms37157_parameters.reflect_input = 1;
    crc16_tms37157_parameters.reflect_output = 1;
    crc16_usb_parameters.initial_value = 0xFFFF;
    crc16_usb_parameters.output_xor = 0xFFFF;
    crc16_usb_parameters.polynomial = 0x8005;
    crc16_usb_parameters.reflect_input = 1;
    crc16_usb_parameters.reflect_output = 1;
    crc16_x25_parameters.initial_value = 0xFFFF;
    crc16_x25_parameters.output_xor = 0xFFFF;
    crc16_x25_parameters.polynomial = 0x1021;
    crc16_x25_parameters.reflect_input = 1;
    crc16_x25_parameters.reflect_output = 1;
    crc16_xmodem_parameters.initial_value = 0x0000;
    crc16_xmodem_parameters.output_xor = 0x0000;
    crc16_xmodem_parameters.polynomial = 0x1021;
    crc16_xmodem_parameters.reflect_input = 0;
    crc16_xmodem_parameters.reflect_output = 0;
    crc16_crca_parameters.initial_value = 0xC6C6;
    crc16_crca_parameters.output_xor = 0x0000;
    crc16_crca_parameters.polynomial = 0x1021;
    crc16_crca_parameters.reflect_input = 1;
    crc16_crca_parameters.reflect_output = 1;

    printf(
        "CRC-16/ARC:         0x%04" PRIX16 "\n",
        crc16(data, length, &crc16_arc_parameters)
    );
    printf(
        "CRC-16/AUG-CCITT:   0x%04" PRIX16 "\n",
        crc16(data, length, &crc16_augccitt_parameters)
    );
    printf(
        "CRC-16/BUYPASS:     0x%04" PRIX16 "\n",
        crc16(data, length, &crc16_buypass_parameters)
    );
    printf(
        "CRC-16/CCITT-FALSE: 0x%04" PRIX16 "\n",
        crc16(data, length, &crc16_ccittfalse_parameters)
    );
    printf(
        "CRC-16/CDMA2000:    0x%04" PRIX16 "\n",
        crc16(data, length, &crc16_cdma2000_parameters)
    );
    printf(
        "CRC-16/DDS-110:     0x%04" PRIX16 "\n",
        crc16(data, length, &crc16_dds110_parameters)
    );
    printf(
        "CRC-16/DECT-R:      0x%04" PRIX16 "\n",
        crc16(data, length, &crc16_dectr_parameters)
    );
    printf(
        "CRC-16/DECT-X:      0x%04" PRIX16 "\n",
        crc16(data, length, &crc16_dectx_parameters)
    );
    printf(
        "CRC-16/DNP:         0x%04" PRIX16 "\n",
        crc16(data, length, &crc16_dnp_parameters)
    );
    printf(
        "CRC-16/EN-13757:    0x%04" PRIX16 "\n",
        crc16(data, length, &crc16_en13757_parameters)
    );
    printf(
        "CRC-16/GENIBUS:     0x%04" PRIX16 "\n",
        crc16(data, length, &crc16_genibus_parameters)
    );
    printf(
        "CRC-16/KERMIT:      0x%04" PRIX16 "\n",
        crc16(data, length, &crc16_kermit_parameters)
    );
    printf(
        "CRC-16/MAXIM:       0x%04" PRIX16 "\n",
        crc16(data, length, &crc16_maxim_parameters)
    );
    printf(
        "CRC-16/MCRF4XX:     0x%04" PRIX16 "\n",
        crc16(data, length, &crc16_mcrf4xx_parameters)
    );
    printf(
        "CRC-16/MODBUS:      0x%04" PRIX16 "\n",
        crc16(data, length, &crc16_modbus_parameters)
    );
    printf(
        "CRC-16/RIELLO:      0x%04" PRIX16 "\n",
        crc16(data, length, &crc16_riello_parameters)
    );
    printf(
        "CRC-16/T10-DIF:     0x%04" PRIX16 "\n",
        crc16(data, length, &crc16_t10dif_parameters)
    );
    printf(
        "CRC-16/TELEDISK:    0x%04" PRIX16 "\n",
        crc16(data, length, &crc16_teledisk_parameters)
    );
    printf(
        "CRC-16/TMS37157:    0x%04" PRIX16 "\n",
        crc16(data, length, &crc16_tms37157_parameters)
    );
    printf(
        "CRC-16/USB:         0x%04" PRIX16 "\n",
        crc16(data, length, &crc16_usb_parameters)
    );
    printf(
        "CRC-16/X-25:        0x%04" PRIX16 "\n",
        crc16(data, length, &crc16_x25_parameters)
    );
    printf(
        "CRC-16/XMODEM:      0x%04" PRIX16 "\n",
        crc16(data, length, &crc16_xmodem_parameters)
    );
    printf(
        "CRC-A:              0x%04" PRIX16 "\n",
        crc16(data, length, &crc16_crca_parameters)
    );
}

static void crc32_test(const char *data) {
    const size_t length = strlen(data);
    crc32_parameters crc32_plain_parameters;
    crc32_parameters crc32_bzip2_parameters;
    crc32_parameters crc32_jamcrc_parameters;
    crc32_parameters crc32_mpeg2_parameters;
    crc32_parameters crc32_posix_parameters;
    crc32_parameters crc32_sata_parameters;
    crc32_parameters crc32_xfer_parameters;
    crc32_parameters crc32_crc32c_parameters;
    crc32_parameters crc32_crc32d_parameters;
    crc32_parameters crc32_crc32q_parameters;

    crc32_plain_parameters.initial_value = 0xFFFFFFFF;
    crc32_plain_parameters.output_xor = 0xFFFFFFFF;
    crc32_plain_parameters.polynomial = 0x04C11DB7;
    crc32_plain_parameters.reflect_input = 1;
    crc32_plain_parameters.reflect_output = 1;
    crc32_bzip2_parameters.initial_value = 0xFFFFFFFF;
    crc32_bzip2_parameters.output_xor = 0xFFFFFFFF;
    crc32_bzip2_parameters.polynomial = 0x04C11DB7;
    crc32_bzip2_parameters.reflect_input = 0;
    crc32_bzip2_parameters.reflect_output = 0;
    crc32_jamcrc_parameters.initial_value = 0xFFFFFFFF;
    crc32_jamcrc_parameters.output_xor = 0x00000000;
    crc32_jamcrc_parameters.polynomial = 0x04C11DB7;
    crc32_jamcrc_parameters.reflect_input = 1;
    crc32_jamcrc_parameters.reflect_output = 1;
    crc32_mpeg2_parameters.initial_value = 0xFFFFFFFF;
    crc32_mpeg2_parameters.output_xor = 0x00000000;
    crc32_mpeg2_parameters.polynomial = 0x04C11DB7;
    crc32_mpeg2_parameters.reflect_input = 0;
    crc32_mpeg2_parameters.reflect_output = 0;
    crc32_posix_parameters.initial_value = 0x00000000;
    crc32_posix_parameters.output_xor = 0xFFFFFFFF;
    crc32_posix_parameters.polynomial = 0x04C11DB7;
    crc32_posix_parameters.reflect_input = 0;
    crc32_posix_parameters.reflect_output = 0;
    crc32_sata_parameters.initial_value = 0x52325032;
    crc32_sata_parameters.output_xor = 0x00000000;
    crc32_sata_parameters.polynomial = 0x04C11DB7;
    crc32_sata_parameters.reflect_input = 0;
    crc32_sata_parameters.reflect_output = 0;
    crc32_xfer_parameters.initial_value = 0x00000000;
    crc32_xfer_parameters.output_xor = 0x00000000;
    crc32_xfer_parameters.polynomial = 0x000000AF;
    crc32_xfer_parameters.reflect_input = 0;
    crc32_xfer_parameters.reflect_output = 0;
    crc32_crc32c_parameters.initial_value = 0xFFFFFFFF;
    crc32_crc32c_parameters.output_xor = 0xFFFFFFFF;
    crc32_crc32c_parameters.polynomial = 0x1EDC6F41;
    crc32_crc32c_parameters.reflect_input = 1;
    crc32_crc32c_parameters.reflect_output = 1;
    crc32_crc32d_parameters.initial_value = 0xFFFFFFFF;
    crc32_crc32d_parameters.output_xor = 0xFFFFFFFF;
    crc32_crc32d_parameters.polynomial = 0xA833982B;
    crc32_crc32d_parameters.reflect_input = 1;
    crc32_crc32d_parameters.reflect_output = 1;
    crc32_crc32q_parameters.initial_value = 0x00000000;
    crc32_crc32q_parameters.output_xor = 0x00000000;
    crc32_crc32q_parameters.polynomial = 0x814141AB;
    crc32_crc32q_parameters.reflect_input = 0;
    crc32_crc32q_parameters.reflect_output = 0;

    printf(
        "CRC-32:             0x%08" PRIX32 "\n",
        crc32(data, length, &crc32_plain_parameters)
    );
    printf(
        "CRC-32/BZIP2:       0x%08" PRIX32 "\n",
        crc32(data, length, &crc32_bzip2_parameters)
    );
    printf(
        "CRC-32/JAMCRC:      0x%08" PRIX32 "\n",
        crc32(data, length, &crc32_jamcrc_parameters)
    );
    printf(
        "CRC-32/MPEG-2:      0x%08" PRIX32 "\n",
        crc32(data, length, &crc32_mpeg2_parameters)
    );
    printf(
        "CRC-32/POSIX:       0x%08" PRIX32 "\n",
        crc32(data, length, &crc32_posix_parameters)
    );
    printf(
        "CRC-32/SATA:        0x%08" PRIX32 "\n",
        crc32(data, length, &crc32_sata_parameters)
    );
    printf(
        "CRC-32/XFER:        0x%08" PRIX32 "\n",
        crc32(data, length, &crc32_xfer_parameters)
    );
    printf(
        "CRC-32C:            0x%08" PRIX32 "\n",
        crc32(data, length, &crc32_crc32c_parameters)
    );
    printf(
        "CRC-32D:            0x%08" PRIX32 "\n",
        crc32(data, length, &crc32_crc32d_parameters)
    );
    printf(
        "CRC-32Q:            0x%08" PRIX32 "\n",
        crc32(data, length, &crc32_crc32q_parameters)
    );
}

static void crc64_test(const char *data) {
    const size_t length = strlen(data);
    crc64_parameters crc64_ecma182_parameters;
    crc64_parameters crc64_goiso_parameters;
    crc64_parameters crc64_ms_parameters;
    crc64_parameters crc64_redis_parameters;
    crc64_parameters crc64_we_parameters;
    crc64_parameters crc64_xz_parameters;

    crc64_ecma182_parameters.initial_value = 0x0000000000000000;
    crc64_ecma182_parameters.output_xor = 0x0000000000000000;
    crc64_ecma182_parameters.polynomial = 0x42F0E1EBA9EA3693;
    crc64_ecma182_parameters.reflect_input = 0;
    crc64_ecma182_parameters.reflect_output = 0;
    crc64_goiso_parameters.initial_value = 0xFFFFFFFFFFFFFFFF;
    crc64_goiso_parameters.output_xor = 0xFFFFFFFFFFFFFFFF;
    crc64_goiso_parameters.polynomial = 0x000000000000001B;
    crc64_goiso_parameters.reflect_input = 1;
    crc64_goiso_parameters.reflect_output = 1;
    crc64_ms_parameters.initial_value = 0xFFFFFFFFFFFFFFFF;
    crc64_ms_parameters.output_xor = 0x0000000000000000;
    crc64_ms_parameters.polynomial = 0x259C84CBA6426349;
    crc64_ms_parameters.reflect_input = 1;
    crc64_ms_parameters.reflect_output = 1;
    crc64_redis_parameters.initial_value = 0x0000000000000000;
    crc64_redis_parameters.output_xor = 0x0000000000000000;
    crc64_redis_parameters.polynomial = 0xad93d23594c935a9;
    crc64_redis_parameters.reflect_input = 1;
    crc64_redis_parameters.reflect_output = 1;
    crc64_we_parameters.initial_value = 0xffffffffffffffff;
    crc64_we_parameters.output_xor = 0xffffffffffffffff;
    crc64_we_parameters.polynomial = 0x42f0e1eba9ea3693;
    crc64_we_parameters.reflect_input = 0;
    crc64_we_parameters.reflect_output = 0;
    crc64_xz_parameters.initial_value = 0xffffffffffffffff;
    crc64_xz_parameters.output_xor = 0xffffffffffffffff;
    crc64_xz_parameters.polynomial = 0x42f0e1eba9ea3693;
    crc64_xz_parameters.reflect_input = 1;
    crc64_xz_parameters.reflect_output = 1;

    printf(
        "CRC-64/ECMA-182:    0x%016" PRIX64 "\n",
        crc64(data, length, &crc64_ecma182_parameters)
    );
    printf(
        "CRC-64/GO-ISO:      0x%016" PRIX64 "\n",
        crc64(data, length, &crc64_goiso_parameters)
    );
    printf(
        "CRC-64/MS:          0x%016" PRIX64 "\n",
        crc64(data, length, &crc64_ms_parameters)
    );
    printf(
        "CRC-64/REDIS:       0x%016" PRIX64 "\n",
        crc64(data, length, &crc64_redis_parameters)
    );
    printf(
        "CRC-64/WE:          0x%016" PRIX64 "\n",
        crc64(data, length, &crc64_we_parameters)
    );
    printf(
        "CRC-64/XZ:          0x%016" PRIX64 "\n",
        crc64(data, length, &crc64_xz_parameters)
    );
}

int main(void) {
    const char *data = "123456789";
    printf("Plain Text:         \"%s\"\n", data);
    crc8_test(data);
    crc16_test(data);
    crc32_test(data);
    crc64_test(data);
    return 0;
}
