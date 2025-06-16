from dataclasses import dataclass

class LutWisunFan1v0:

    MHZ_TO_HZ = 1_000_000

    @dataclass
    class BitrateModulation:
        phy_mode_id_base : int
        modulation_index : float
        bitrates: list[int]

    @dataclass
    class FrequencyChannel:
        total_nb_channel: int
        freq_band_start: float
        freq_band_end: float

    bitrate_modulation_lut: dict[str, BitrateModulation] = {
        'Mode1a' : BitrateModulation(
            phy_mode_id_base=0x01,
            modulation_index=0.5,
            bitrates=[50_000]
        ),
        'Mode1b' : BitrateModulation(
            phy_mode_id_base=0x02,
            modulation_index=1.0,
            bitrates=[50_000]
        ),
        'Mode2a': BitrateModulation(
            phy_mode_id_base=0x03,
            modulation_index=0.5,
            bitrates=[100_000]
        ),
        'Mode2b': BitrateModulation(
            phy_mode_id_base=0x04,
            modulation_index=1.0,
            bitrates=[100_000]
        ),
        'Mode3': BitrateModulation(
            phy_mode_id_base=0x05,
            modulation_index=0.5,
            bitrates=[150_000]
        ),
        'Mode4a': BitrateModulation(
            phy_mode_id_base=0x06,
            modulation_index=0.5,
            bitrates=[200_000]
        ),
        'Mode4b': BitrateModulation(
            phy_mode_id_base=0x07,
            modulation_index=1.0,
            bitrates=[200_000]
        ),
        'Mode5': BitrateModulation(
            phy_mode_id_base=0x08,
            modulation_index=0.5,
            bitrates=[300_000]
        ),
    }

    frequency_channel_lut: dict[str,FrequencyChannel] = {
        'AU_NZ_1': FrequencyChannel(
            total_nb_channel=64,
            freq_band_start=915,
            freq_band_end=928
        ),
        'AU_NZ_2': FrequencyChannel(
            total_nb_channel=32,
            freq_band_start=915,
            freq_band_end=928
        ),
        'BZ_1': FrequencyChannel(
            total_nb_channel=129,
            freq_band_start=902,
            freq_band_end=928
        ),
        'BZ_2': FrequencyChannel(
            total_nb_channel=64,
            freq_band_start=902,
            freq_band_end=928
        ),
        'BZ_3': FrequencyChannel(
            total_nb_channel=42,
            freq_band_start=902,
            freq_band_end=928
        ),
        'CN_1': FrequencyChannel(
            total_nb_channel=199,
            freq_band_start=470,
            freq_band_end=510
        ),
        'CN_2': FrequencyChannel( # TODO: unnecessary only here, because broke etest on tools side
            total_nb_channel=42,
            freq_band_start=42,
            freq_band_end=42
        ),
        'CN_3': FrequencyChannel( # TODO: unnecessary only here, because broke etest on tools side
            total_nb_channel=42,
            freq_band_start=42,
            freq_band_end=42
        ),
        'CN_4': FrequencyChannel(
            total_nb_channel=16,
            freq_band_start=920.5,
            freq_band_end=924.5
        ),
        'EU_1': FrequencyChannel(
            total_nb_channel=69,
            freq_band_start=863,
            freq_band_end=870
        ),
        'EU_2': FrequencyChannel(
            total_nb_channel=35,
            freq_band_start=863,
            freq_band_end=870
        ),
        'EU_3': FrequencyChannel(
            total_nb_channel=55,
            freq_band_start=870,
            freq_band_end=876
        ),
        'EU_4': FrequencyChannel(
            total_nb_channel=27,
            freq_band_start=870,
            freq_band_end=876
        ),
        'HK_1': FrequencyChannel(
            total_nb_channel=24,
            freq_band_start=920,
            freq_band_end=925
        ),
        'HK_2': FrequencyChannel(
            total_nb_channel=12,
            freq_band_start=920,
            freq_band_end=925
        ),
        'IN_1': FrequencyChannel(
            total_nb_channel=29,
            freq_band_start=865,
            freq_band_end=867
        ),
        'IN_2': FrequencyChannel(
            total_nb_channel=15,
            freq_band_start=865,
            freq_band_end=867
        ),
        'JP_1': FrequencyChannel(
            total_nb_channel=38,
            freq_band_start=920,
            freq_band_end=928
        ),
        'JP_2': FrequencyChannel(
            total_nb_channel=18,
            freq_band_start=920,
            freq_band_end=928
        ),
        'JP_3': FrequencyChannel(
            total_nb_channel=12,
            freq_band_start=920,
            freq_band_end=928
        ),
        'KR_1': FrequencyChannel(
            total_nb_channel=32,
            freq_band_start=917,
            freq_band_end=923.5
        ),
        'KR_2': FrequencyChannel(
            total_nb_channel=16,
            freq_band_start=917,
            freq_band_end=923.5
        ),
        'MX_1': FrequencyChannel(
            total_nb_channel=129,
            freq_band_start=902,
            freq_band_end=928
        ),
        'MX_2': FrequencyChannel(
            total_nb_channel=64,
            freq_band_start=902,
            freq_band_end=928
        ),
        'MY_1': FrequencyChannel(
            total_nb_channel=19,
            freq_band_start=919,
            freq_band_end=923
        ),
        'MY_2': FrequencyChannel(
            total_nb_channel=10,
            freq_band_start=919,
            freq_band_end=923
        ),
        'NA_1': FrequencyChannel(
            total_nb_channel=129,
            freq_band_start=902,
            freq_band_end=928
        ),
        'NA_2': FrequencyChannel(
            total_nb_channel=64,
            freq_band_start=902,
            freq_band_end=928
        ),
        'NA_3': FrequencyChannel(
            total_nb_channel=42,
            freq_band_start=902,
            freq_band_end=928
        ),
        'PH_1': FrequencyChannel(
            total_nb_channel=64,
            freq_band_start=915,
            freq_band_end=918
        ),
        'PH_2': FrequencyChannel(
            total_nb_channel=32,
            freq_band_start=915,
            freq_band_end=918
        ),
        'SG_1': FrequencyChannel(
            total_nb_channel=29,
            freq_band_start=866,
            freq_band_end=869
        ),
        'SG_2': FrequencyChannel(
            total_nb_channel=15,
            freq_band_start=866,
            freq_band_end=869
        ),
        'SG_3': FrequencyChannel(
            total_nb_channel=7,
            freq_band_start=866,
            freq_band_end=869
        ),
        'SG_4': FrequencyChannel(
            total_nb_channel=24,
            freq_band_start=920,
            freq_band_end=925
        ),
        'SG_5': FrequencyChannel(
            total_nb_channel=12,
            freq_band_start=920,
            freq_band_end=925
        ),
        'TH_1': FrequencyChannel(
            total_nb_channel=24,
            freq_band_start=920,
            freq_band_end=925
        ),
        'TH_2': FrequencyChannel(
            total_nb_channel=12,
            freq_band_start=920,
            freq_band_end=925
        ),
        'VN_1': FrequencyChannel(
            total_nb_channel=24,
            freq_band_start=920,
            freq_band_end=925
        ),
        'VN_2': FrequencyChannel(
            total_nb_channel=12,
            freq_band_start=920,
            freq_band_end=925
        ),
    }

    @staticmethod
    def __check_bitrate_key_exist(mode: str) -> bool:
        if mode in LutWisunFan1v0.bitrate_modulation_lut.keys():
            return True

        return False

    @staticmethod
    def __generate_frequency_key(reg_abbrev: str, operating_class: int) -> str | None:
        key = f"{reg_abbrev}_{operating_class}"
        if key not in LutWisunFan1v0.frequency_channel_lut.keys():
            return

        return key

    @staticmethod
    def get_modulation(mode: str) -> str | None:
        if not LutWisunFan1v0.__check_bitrate_key_exist(mode):
            return None

        return 'FSK'

    @staticmethod
    def get_modulation_index(mode: str) -> float | None:
        if not LutWisunFan1v0.__check_bitrate_key_exist(mode):
            return None

        modulation_index = LutWisunFan1v0.bitrate_modulation_lut[mode].modulation_index
        return  modulation_index

    @staticmethod
    def get_bitrates(mode: str) -> list[int] | None:
        if not LutWisunFan1v0.__check_bitrate_key_exist(mode):
            return None

        bitrates = LutWisunFan1v0.bitrate_modulation_lut[mode].bitrates
        return bitrates

    @staticmethod
    def get_mcs_restrictions(mode: str) -> None:
        """In fan1v0 mcs_rerstriction not exist, it's connected to OFDM and fan1v0 doesn't have OFDM option.
        Keep it to be consistent with fan1v1."""
        return None

    @staticmethod
    def get_freq_band_start(reg_abbrev: str, operating_class: int) -> int | None:
        key = LutWisunFan1v0.__generate_frequency_key(reg_abbrev, operating_class)
        if key is None:
            return

        freq_band_start = int(LutWisunFan1v0.frequency_channel_lut[key].freq_band_start * LutWisunFan1v0.MHZ_TO_HZ)
        return freq_band_start

    @staticmethod
    def get_freq_band_end(reg_abbrev: str, operating_class: int) -> int | None:
        key = LutWisunFan1v0.__generate_frequency_key(reg_abbrev, operating_class)
        if key is None:
            return

        freq_band_end = int(LutWisunFan1v0.frequency_channel_lut[key].freq_band_end * LutWisunFan1v0.MHZ_TO_HZ)
        return freq_band_end

    @staticmethod
    def get_start_channel_number(reg_abbrev: str, operating_class: int) -> int | None:
        key = LutWisunFan1v0.__generate_frequency_key(reg_abbrev, operating_class)
        if key is None:
            return

        start_channel_number = 0
        return start_channel_number

    @staticmethod
    def get_end_channel_number(reg_abbrev: str, operating_class: int) -> int | None:
        key = LutWisunFan1v0.__generate_frequency_key(reg_abbrev, operating_class)
        if key is None:
            return

        end_channel_number = LutWisunFan1v0.frequency_channel_lut[key].total_nb_channel - 1
        return end_channel_number