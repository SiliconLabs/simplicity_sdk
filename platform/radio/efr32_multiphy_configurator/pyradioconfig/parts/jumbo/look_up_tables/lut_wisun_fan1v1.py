from pycalcmodel.core.model import ModelRoot


class LutWisunFan1v1:
    MHZ_TO_HZ = 1_000_000

    bitrate_modulation_lut: dict[str, dict[str, str | list[int | float] | float | None]] = {
        '1a': {
            'modulation': 'FSK',
            'modulation_index': 0.5,
            'bitrates': [50_000],
            'restrictions': None
        },

        '1b': {
            'modulation': 'FSK',
            'modulation_index': 1.0,
            'bitrates': [50_000],
            'restrictions': None
        },

        '2a': {
            'modulation': 'FSK',
            'modulation_index': 0.5,
            'bitrates': [100_000],
            'restrictions': None
        },

        '2b': {
            'modulation': 'FSK',
            'modulation_index': 1.0,
            'bitrates': [100_000],
            'restrictions': None
        },

        '3': {
            'modulation': 'FSK',
            'modulation_index': 0.5,
            'bitrates': [150_000],
            'restrictions': None
        },

        '4a': {
            'modulation': 'FSK',
            'modulation_index': 0.5,
            'bitrates': [200_000],
            'restrictions': None
        },

        '4b': {
            'modulation': 'FSK',
            'modulation_index': 1.0,
            'bitrates': [200_000],
            'restrictions': None
        },

        '5': {
            'modulation': 'FSK',
            'modulation_index': 0.5,
            'bitrates': [300_000],
            'restrictions': None
        },

        'opt1': {
            'modulation': 'OFDM',
            'modulation_index': None,
            'bitrates': [100_000, 200_000, 400_000, 800_000, 1200_000, 1600_000, 2400_000, 3600_000],
            'restrictions': [2, 3, 4, 5, 6, 7]
        },

        'opt2': {
            'modulation': 'OFDM',
            'modulation_index': None,
            'bitrates': [50_000, 100_000, 200_000, 400_000, 600_000, 800_000, 1200_000, 1800_000],
            'restrictions': [3, 4, 5, 6, 7]
        },

        'opt3': {
            'modulation': 'OFDM',
            'modulation_index': None,
            'bitrates': [25_000, 50_000, 100_000, 200_000, 300_000, 400_000, 600_000, 900_000],
            'restrictions': [4, 5, 6, 7]
        },

        'opt4': {
            'modulation': 'OFDM',
            'modulation_index': None,
            'bitrates': [12_500, 25_000, 50_000, 100_000, 150_000, 200_000, 300_000, 450_000],
            'restrictions': [4, 5, 6, 7]
        },
    }

    frequency_channel_lut: dict[str, dict[str, int | float]] = {
        'AU_NZ_48': {
            'freq_band_start': 915,
            'freq_band_end': 928,
            'total_channel_number': 64,
        },

        'AU_NZ_49': {
            'freq_band_start': 915,
            'freq_band_end': 928,
            'total_channel_number': 32,
        },

        'BZ_1': {
            'freq_band_start': 902,
            'freq_band_end': 928,
            'total_channel_number': 129,
        },

        'BZ_2': {
            'freq_band_start': 902,
            'freq_band_end': 928,
            'total_channel_number': 64,
        },

        'BZ_3': {
            'freq_band_start': 902,
            'freq_band_end': 928,
            'total_channel_number': 42,
        },

        'BZ_4': {
            'freq_band_start': 902,
            'freq_band_end': 928,
            'total_channel_number': 32,
        },

        'BZ_5': {
            'freq_band_start': 902,
            'freq_band_end': 928,
            'total_channel_number': 21,
        },

        'CN_128': {
            'freq_band_start': 920.5,
            'freq_band_end': 924.5,
            'total_channel_number': 16,
        },

        'CN_160': {
            'freq_band_start': 470,
            'freq_band_end': 510,
            'total_channel_number': 199,
        },

        'EU_32': {
            'freq_band_start': 863,
            'freq_band_end': 870,
            'total_channel_number': 69,
        },

        'EU_33': {
            'freq_band_start': 863,
            'freq_band_end': 870,
            'total_channel_number': 35,
        },

        'EU_34': {
            'freq_band_start': 870,
            'freq_band_end': 876,
            'total_channel_number': 55,
        },

        'EU_35': {
            'freq_band_start': 870,
            'freq_band_end': 876,
            'total_channel_number': 27,
        },

        'EU_36': {
            'freq_band_start': 863,
            'freq_band_end': 876,
            'total_channel_number': 125,
        },

        'EU_37': {
            'freq_band_start': 863,
            'freq_band_end': 876,
            'total_channel_number': 62,
        },

        'HK_64': {
            'freq_band_start': 920,
            'freq_band_end': 925,
            'total_channel_number': 24,
        },

        'HK_65': {
            'freq_band_start': 920,
            'freq_band_end': 925,
            'total_channel_number': 12,
        },

        'IN_39': {
            'freq_band_start': 865,
            'freq_band_end': 867,
            'total_channel_number': 29,
        },

        'IN_40': {
            'freq_band_start': 865,
            'freq_band_end': 867,
            'total_channel_number': 15,
        },

        'JP_21': {
            'freq_band_start': 920,
            'freq_band_end': 928,
            'total_channel_number': 38,
        },

        'JP_22': {
            'freq_band_start': 920,
            'freq_band_end': 928,
            'total_channel_number': 18,
        },

        'JP_23': {
            'freq_band_start': 920,
            'freq_band_end': 928,
            'total_channel_number': 12,
        },

        'JP_24': {
            'freq_band_start': 920,
            'freq_band_end': 928,
            'total_channel_number': 24,
        },

        'KR_96': {
            'freq_band_start': 917,
            'freq_band_end': 923.5,
            'total_channel_number': 32,
        },

        'KR_97': {
            'freq_band_start': 917,
            'freq_band_end': 923.5,
            'total_channel_number': 16,
        },

        'MX_1': {
            'freq_band_start': 902,
            'freq_band_end': 928,
            'total_channel_number': 129,
        },

        'MX_2': {
            'freq_band_start': 902,
            'freq_band_end': 928,
            'total_channel_number': 64,
        },

        'MY_80': {
            'freq_band_start': 919,
            'freq_band_end': 923,
            'total_channel_number': 19,
        },

        'MY_81': {
            'freq_band_start': 919,
            'freq_band_end': 923,
            'total_channel_number': 10,
        },

        'NA_1': {
            'freq_band_start': 902,
            'freq_band_end': 928,
            'total_channel_number': 129,
        },

        'NA_2': {
            'freq_band_start': 902,
            'freq_band_end': 928,
            'total_channel_number': 64,
        },

        'NA_3': {
            'freq_band_start': 902,
            'freq_band_end': 928,
            'total_channel_number': 42,
        },

        'NA_4': {
            'freq_band_start': 902,
            'freq_band_end': 928,
            'total_channel_number': 32,
        },

        'NA_5': {
            'freq_band_start': 902,
            'freq_band_end': 928,
            'total_channel_number': 21,
        },

        'PH_48': {
            'freq_band_start': 915,
            'freq_band_end': 918,
            'total_channel_number': 64,
        },

        'PH_49': {
            'freq_band_start': 915,
            'freq_band_end': 918,
            'total_channel_number': 32,
        },

        'SG_41': {
            'freq_band_start': 866,
            'freq_band_end': 869,
            'total_channel_number': 29,
        },

        'SG_42': {
            'freq_band_start': 866,
            'freq_band_end': 869,
            'total_channel_number': 15,
        },

        'SG_43': {
            'freq_band_start': 866,
            'freq_band_end': 869,
            'total_channel_number': 7,
        },

        'SG_64': {
            'freq_band_start': 920,
            'freq_band_end': 925,
            'total_channel_number': 24,
        },

        'SG_65': {
            'freq_band_start': 920,
            'freq_band_end': 925,
            'total_channel_number': 12,
        },

        'TH_64': {
            'freq_band_start': 920,
            'freq_band_end': 925,
            'total_channel_number': 24
        },

        'TH_65': {
            'freq_band_start': 920,
            'freq_band_end': 925,
            'total_channel_number': 12,
        },

        'VN_64': {
            'freq_band_start': 920,
            'freq_band_end': 925,
            'total_channel_number': 24,
        },

        'VN_65': {
            'freq_band_start': 920,
            'freq_band_end': 925,
            'total_channel_number': 12,
        },

        'WW_112': {
            'freq_band_start': 2400,
            'freq_band_end': 2483.5,
            'total_channel_number': 416,
        },

        'WW_113': {
            'freq_band_start': 2400,
            'freq_band_end': 2483.5,
            'total_channel_number': 207,
        },

    }


    @staticmethod
    def __generate_bitrate_key(code: int) -> str | None:
        if code == 0x01 or code == 0x11:
            return '1a'
        if code == 0x02 or code == 0x12:
            return '1b'
        if code == 0x03 or code == 0x13:
            return '2a'
        if code == 0x04 or code == 0x14:
            return '2b'
        if code == 0x05 or code == 0x15:
            return '3'
        if code == 0x06 or code == 0x16:
            return '4a'
        if code == 0x07 or code == 0x17:
            return '4b'
        if code == 0x08 or code == 0x18:
            return '5'

        if code >= 0x50 :
            return 'opt4'
        elif code >= 0x40:
            return 'opt3'
        elif code >= 0x30:
            return 'opt2'
        elif code >= 0x20:
            return 'opt1'

        return None

    @staticmethod
    def __generate_frequency_key(reg_domain: str, channel_plan_id: int) -> str | None:
        key = f"{reg_domain}_{channel_plan_id}"
        if key not in LutWisunFan1v1.frequency_channel_lut.keys():
            return

        return key

    @staticmethod
    def get_modulation(code: int) -> str | None:
        key = LutWisunFan1v1.__generate_bitrate_key(code)
        if key is None:
            return

        modulation = LutWisunFan1v1.bitrate_modulation_lut[key]['modulation']
        return modulation

    @staticmethod
    def get_modulation_index(code: int) -> float | None:
        key = LutWisunFan1v1.__generate_bitrate_key(code)
        if key is None:
            return

        modulation_index = LutWisunFan1v1.bitrate_modulation_lut[key]['modulation_index']
        return modulation_index

    @staticmethod
    def get_bitrates(code: int) -> list[int] | None:
        key = LutWisunFan1v1.__generate_bitrate_key(code)
        if key is None:
            return

        bitrates = LutWisunFan1v1.bitrate_modulation_lut[key]['bitrates']
        return bitrates

    @staticmethod
    def get_mcs_restrictions(code: int) -> list[int] | None:
        key = LutWisunFan1v1.__generate_bitrate_key(code)
        if key is None:
            return

        restrictions = LutWisunFan1v1.bitrate_modulation_lut[key]['restrictions']
        return restrictions

    @staticmethod
    def get_freq_band_start(reg_domain: str, channel_plan_id: int) -> int | None:
        key = LutWisunFan1v1.__generate_frequency_key(reg_domain, channel_plan_id)
        if key is None:
            return

        freq_band_start = int(LutWisunFan1v1.frequency_channel_lut[key]['freq_band_start'] * LutWisunFan1v1.MHZ_TO_HZ)
        return freq_band_start

    @staticmethod
    def get_freq_band_end(reg_domain: str, channel_plan_id: int) -> int | None:
        key = LutWisunFan1v1.__generate_frequency_key(reg_domain, channel_plan_id)
        if key is None:
            return

        freq_band_end = int(LutWisunFan1v1.frequency_channel_lut[key]['freq_band_end'] * LutWisunFan1v1.MHZ_TO_HZ)
        return freq_band_end

    @staticmethod
    def __get_right_phy_mode_id(phy_mode_id: int) -> int:
        if phy_mode_id < 0x20:
            return phy_mode_id

        if phy_mode_id > 0x50:
            return 0x50
        if phy_mode_id > 0x40:
            return  0x40
        if phy_mode_id > 0x30:
            return 0x30
        if phy_mode_id > 0x20:
            return 0x20

    @staticmethod
    def get_start_channel_number(reg_domain: str, channel_plan_id: int, phy_mode_id: int) -> int | None:
        key = LutWisunFan1v1.__generate_frequency_key(reg_domain, channel_plan_id)
        if key is None:
            return

        real_phy_mode = LutWisunFan1v1.__get_right_phy_mode_id(phy_mode_id)
        start_channel_number = real_phy_mode * 256
        return start_channel_number

    @staticmethod
    def get_end_channel_number(reg_domain: str, channel_plan_id: int, phy_mode_id: int) -> int | None:
        key = LutWisunFan1v1.__generate_frequency_key(reg_domain, channel_plan_id)
        if key is None:
            return

        start_channel_number = LutWisunFan1v1.get_start_channel_number(reg_domain, channel_plan_id, phy_mode_id)
        if start_channel_number is None:
            return

        total_channel_number = LutWisunFan1v1.frequency_channel_lut[key]['total_channel_number']
        end_channel_number = start_channel_number + total_channel_number - 1

        return end_channel_number

    @staticmethod
    def get_group(reg_domain: str, channel_plan_id: int, model: ModelRoot):
        key = LutWisunFan1v1.__generate_frequency_key(reg_domain, channel_plan_id)
        if key is None:
            return

        if reg_domain == 'AU_NZ':
            return model.vars.meta_group.var_enum.AU_NZ
        if reg_domain == 'BZ':
            return model.vars.meta_group.var_enum.BZ
        if reg_domain == 'HK':
            return model.vars.meta_group.var_enum.HK
        if reg_domain == 'IN':
            return model.vars.meta_group.var_enum.IN
        if reg_domain == 'JP':
            return model.vars.meta_group.var_enum.JP
        if reg_domain == 'KR':
            return model.vars.meta_group.var_enum.KR
        if reg_domain == 'MX':
            return model.vars.meta_group.var_enum.MX
        if reg_domain == 'MY':
            return model.vars.meta_group.var_enum.MY
        if reg_domain == 'NA':
            return model.vars.meta_group.var_enum.NA
        if reg_domain == 'PH':
            return model.vars.meta_group.var_enum.PH
        if reg_domain == 'TH':
            return model.vars.meta_group.var_enum.TH
        if reg_domain == 'VN':
            return model.vars.meta_group.var_enum.VN
        if reg_domain == 'WW':
            return model.vars.meta_group.var_enum.WW

        if reg_domain == 'CN':
            if channel_plan_id in [160]:
                return model.vars.meta_group.var_enum.CN1
            if channel_plan_id in [128]:
                return model.vars.meta_group.var_enum.CN2

        if reg_domain == 'EU':
            if channel_plan_id in [32,33]:
                return model.vars.meta_group.var_enum.EU1
            if channel_plan_id in [34,35]:
                return model.vars.meta_group.var_enum.EU2
            if channel_plan_id in [36,37]:
                return model.vars.meta_group.var_enum.EU3

        if reg_domain == 'SG':
            if channel_plan_id in [41,42,43]:
                return model.vars.meta_group.var_enum.SG1
            if channel_plan_id in [64,65]:
                return model.vars.meta_group.var_enum.SG2

        return
