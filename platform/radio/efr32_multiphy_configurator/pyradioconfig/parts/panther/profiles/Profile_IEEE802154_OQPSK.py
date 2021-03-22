from pyradioconfig.parts.common.profiles.panther_regs import build_modem_regs_panther
from pyradioconfig.calculator_model_framework.interfaces.iprofile import IProfile
from pyradioconfig.parts.common.utils.units_multiplier import UnitsMultiplier
from pyradioconfig.parts.common.profiles.profile_common import buildCrcOutputs, buildFecOutputs, buildFrameOutputs, \
    buildWhiteOutputs, build_ircal_sw_vars
from pyradioconfig.parts.common.profiles.profile_modem import buildModemInfoOutputs
from pycalcmodel.core.output import ModelOutput, ModelOutputType
from py_2_and_3_compatibility import *


class ProfileIEEE802154OQPSKPanther(IProfile):

    def __init__(self):
        super().__init__()
        self._profileName = "IEEE802154OQPSK"
        self._readable_name = "IEEE802154 OQPSK Profile"
        self._category = ""
        self._description = "Profile used for IEEE802154 OQPSK phys"
        self._default = False
        self._activation_logic = ""
        self._family = "panther"

    def buildProfileModel(self, model):

        # Build profile object and append it to the model
        profile = self._makeProfile(model)

        # Build inputs
        self.build_required_profile_inputs(model, profile)
        self.build_optional_profile_inputs(model, profile)
        self.build_advanced_profile_inputs(model, profile)
        self.build_hidden_profile_inputs(model, profile)

        # Build outputs
        buildFrameOutputs(model, profile)
        buildCrcOutputs(model, profile)
        buildWhiteOutputs(model, profile)
        buildFecOutputs(model, profile)
        self.build_register_profile_outputs(model, profile)
        self.build_variable_profile_outputs(model, profile)
        self.build_info_profile_outputs(model, profile)

    def build_required_profile_inputs(self, model, profile):
        IProfile.make_required_input(profile, model.vars.xtal_frequency_hz, "crystal",
                                     readable_name="Crystal Frequency", value_limit_min=38000000,
                                     value_limit_max=40000000, units_multiplier=UnitsMultiplier.MEGA)

    def build_optional_profile_inputs(self, model, profile):
        # No optional inputs for this profile
        pass

    def build_advanced_profile_inputs(self, model, profile):
        IProfile.make_linked_io(profile, model.vars.base_frequency_hz, "operational_frequency",
                                     readable_name="Base Channel Frequency", value_limit_min=100000000,
                                     value_limit_max=2500000000, units_multiplier=UnitsMultiplier.MEGA)

        self.make_linked_io(profile, model.vars.zigbee_feature, 'Advanced', readable_name="Zigbee PHY Feature Selection")
        self.make_linked_io(profile, model.vars.rx_sync_delay_ns, 'Advanced', readable_name="Receive chain delay ns", value_limit_min=0, value_limit_max=10000)
        self.make_linked_io(profile, model.vars.rx_eof_delay_ns, 'Advanced', readable_name="Receive end of frame delay ns", value_limit_min=0, value_limit_max=10000)
        pass

    def build_hidden_profile_inputs(self, model, profile):
        # Hidden input needed for rail_test generation
        self.make_hidden_input(profile, model.vars.channel_spacing_hz, "operational_frequency",
                                     readable_name="Channel Spacing", value_limit_min=0,
                                     value_limit_max=10000000,
                                     units_multiplier=UnitsMultiplier.KILO)

        # Hidden inputs to allow for fixed frame length testing
        self.make_hidden_input(profile, model.vars.frame_length_type, 'frame_general',
                               readable_name="Frame Length Algorithm")
        self.make_hidden_input(profile, model.vars.fixed_length_size, category='frame_fixed_length',
                               readable_name="Fixed Payload Size", value_limit_min=0, value_limit_max=0x7fffffff)

        # Hidden input for changing syncword
        self.make_hidden_input(profile, model.vars.syncword_0, "syncword", readable_name="Sync Word 0",
                                     value_limit_min=long(0), value_limit_max=long(0xffffffff))
        self.make_hidden_input(profile, model.vars.syncword_1, "syncword", readable_name="Sync Word 1",
                                     value_limit_min=long(0), value_limit_max=long(0xffffffff))

    def build_register_profile_outputs(self, model, profile):
        build_modem_regs_panther(model, profile)

    def build_variable_profile_outputs(self, model, profile):
        self.buildRailOutputs(model, profile)
        build_ircal_sw_vars(model, profile)

    def build_info_profile_outputs(self, model, profile):
        buildModemInfoOutputs(model, profile)
        profile.outputs.append(ModelOutput(model.vars.afc_period, '', ModelOutputType.INFO, readable_name="Frequency Offset Compensation (AFC) Period"))
        profile.outputs.append(ModelOutput(model.vars.afc_step_scale, '', ModelOutputType.INFO, readable_name="Frequency Offset Compensation (AFC) Step Scale"))
        profile.outputs.append(ModelOutput(model.vars.agc_hysteresis, '', ModelOutputType.INFO, readable_name="AGC Hysteresis"))
        profile.outputs.append(ModelOutput(model.vars.agc_period, '', ModelOutputType.INFO, readable_name="AGC Period"))
        profile.outputs.append(ModelOutput(model.vars.agc_power_target, '', ModelOutputType.INFO, readable_name="AGC Power Target"))
        profile.outputs.append(ModelOutput(model.vars.agc_scheme, '', ModelOutputType.INFO, readable_name="AGC backoff scheme"))
        profile.outputs.append(ModelOutput(model.vars.agc_settling_delay, '', ModelOutputType.INFO, readable_name="AGC Settling Delay"))
        profile.outputs.append(ModelOutput(model.vars.agc_speed, '', ModelOutputType.INFO, readable_name="AGC Speed"))
        profile.outputs.append(ModelOutput(model.vars.bandwidth_hz, '', ModelOutputType.INFO, readable_name="Acquisition Channel Bandwidth"))
        profile.outputs.append(ModelOutput(model.vars.errors_in_timing_window, '', ModelOutputType.INFO, readable_name="Number of Errors Allowed in a Timing Window"))
        profile.outputs.append(ModelOutput(model.vars.etsi_cat1_compatible, '', ModelOutputType.INFO, readable_name="ETSI Category 1 Compatibility"))
        profile.outputs.append(ModelOutput(model.vars.firstframe_bitsperword, '', ModelOutputType.INFO, readable_name="Length of the First Word"))
        profile.outputs.append(ModelOutput(model.vars.freq_offset_hz, '', ModelOutputType.INFO, readable_name="Frequency Offset Compensation (AFC) Limit"))
        profile.outputs.append(ModelOutput(model.vars.frequency_comp_mode, '', ModelOutputType.INFO, readable_name="Frequency Compensation Mode"))
        profile.outputs.append(ModelOutput(model.vars.frequency_offset_period, '', ModelOutputType.INFO, readable_name="Frequency Offset Period"))
        profile.outputs.append(ModelOutput(model.vars.if_frequency_hz, '', ModelOutputType.INFO, readable_name="IF Frequency"))
        profile.outputs.append(ModelOutput(model.vars.ircal_power_level, '', ModelOutputType.INFO, readable_name="IR cal power level (amplitude)"))
        profile.outputs.append(ModelOutput(model.vars.ircal_rxtx_path_common, '', ModelOutputType.INFO, readable_name="Common RX/TX circuit"))
        profile.outputs.append(ModelOutput(model.vars.lo_injection_side, '', ModelOutputType.INFO, readable_name="Injection side"))
        profile.outputs.append(ModelOutput(model.vars.number_of_timing_windows, '', ModelOutputType.INFO, readable_name="Number of Timing Windows to Detect"))
        profile.outputs.append(ModelOutput(model.vars.pll_bandwidth_rx, '', ModelOutputType.INFO, readable_name="PLL Bandwidth in RX mode"))
        profile.outputs.append(ModelOutput(model.vars.pll_bandwidth_tx, '', ModelOutputType.INFO, readable_name="PLL Bandwidth in TX mode"))
        profile.outputs.append(ModelOutput(model.vars.src_disable, '', ModelOutputType.INFO, readable_name="SRC Operation"))
        profile.outputs.append(ModelOutput(model.vars.symbols_in_timing_window, '', ModelOutputType.INFO, readable_name="Number of Symbols in Timing Window"))
        profile.outputs.append(ModelOutput(model.vars.target_osr, '', ModelOutputType.INFO, readable_name="Target oversampling rate"))
        profile.outputs.append(ModelOutput(model.vars.timing_detection_threshold, '', ModelOutputType.INFO, readable_name="Timing Detection Threshold"))
        profile.outputs.append(ModelOutput(model.vars.timing_resync_period, '', ModelOutputType.INFO, readable_name="Timing Resync Period"))
        profile.outputs.append(ModelOutput(model.vars.timing_sample_threshold, '', ModelOutputType.INFO, readable_name="Timing Samples Threshold"))
        profile.outputs.append(ModelOutput(model.vars.var_length_loc, '', ModelOutputType.INFO, readable_name="Byte position of dynamic length byte"))
        profile.outputs.append(ModelOutput(model.vars.rssi_period, '', ModelOutputType.INFO, readable_name="RSSI Update Period"))
        profile.outputs.append(ModelOutput(model.vars.antdivmode, '', ModelOutputType.INFO, readable_name="Antenna diversity mode"))
        profile.outputs.append(ModelOutput(model.vars.antdivrepeatdis, '', ModelOutputType.INFO, readable_name="Diversity Select-Best repeat"))

    def buildRailOutputs(self, model, profile):
        profile.outputs.append(ModelOutput(model.vars.frequency_offset_factor, '', ModelOutputType.RAIL_CONFIG, readable_name='Frequency Offset Factor'))
        profile.outputs.append(ModelOutput(model.vars.frequency_offset_factor_fxp, '', ModelOutputType.RAIL_CONFIG, readable_name='Frequency Offset Factor FXP'))
        profile.outputs.append(ModelOutput(model.vars.dynamic_slicer_enabled, '', ModelOutputType.RAIL_CONFIG, readable_name='Dynamic Slicer Feature Enabled'))
        profile.outputs.append(ModelOutput(model.vars.dynamic_slicer_threshold_values, '', ModelOutputType.RAIL_CONFIG, readable_name='Dynamic Slicer Threshold Values'))
        profile.outputs.append(ModelOutput(model.vars.dynamic_slicer_level_values, '', ModelOutputType.RAIL_CONFIG, readable_name='Dynamic Slicer Level Values'))
        profile.outputs.append(ModelOutput(model.vars.src1_calcDenominator, '', ModelOutputType.RAIL_CONFIG, readable_name='SRC1 Helper Calculation'))
        profile.outputs.append(ModelOutput(model.vars.src2_calcDenominator, '', ModelOutputType.RAIL_CONFIG, readable_name='SRC2 Helper Calculation'))
        profile.outputs.append(ModelOutput(model.vars.tx_baud_rate_actual, '', ModelOutputType.RAIL_CONFIG, readable_name='TX Baud Rate'))
        profile.outputs.append(ModelOutput(model.vars.baud_per_symbol_actual, '', ModelOutputType.RAIL_CONFIG, readable_name='Number of baud to transmit 1 symbol'))
        profile.outputs.append(ModelOutput(model.vars.bits_per_symbol_actual, '', ModelOutputType.RAIL_CONFIG, readable_name='Number of bits contained in 1 symbol'))
        profile.outputs.append(ModelOutput(model.vars.rx_ch_hopping_order_num, '', ModelOutputType.RAIL_CONFIG, readable_name='For receive scanning PHYs: order of PHY in scanning sequence'))
        profile.outputs.append(ModelOutput(model.vars.rx_ch_hopping_mode, '', ModelOutputType.RAIL_CONFIG, readable_name='For receive scanning PHYs: event to trigger a hop to next PHY'))
        profile.outputs.append(ModelOutput(model.vars.rx_ch_hopping_delay_usec, '', ModelOutputType.RAIL_CONFIG,readable_name='For receive scanning PHYs: delay in microseconds to look for RX on a particular PHY'))
        profile.outputs.append(ModelOutput(model.vars.div_antdivmode, '', ModelOutputType.RAIL_CONFIG, readable_name='Antenna diversity mode'))
        profile.outputs.append(ModelOutput(model.vars.div_antdivrepeatdis, '', ModelOutputType.RAIL_CONFIG, readable_name='Disable repeated measurement of first antenna when Select-Best algorithm is used'))
        profile.outputs.append(ModelOutput(model.vars.stack_info, '', ModelOutputType.RAIL_CONFIG, readable_name='Stack information containing protocol and PHY IDs'))
        profile.outputs.append(ModelOutput(model.vars.tx_eof_delay_ns, '', ModelOutputType.RAIL_CONFIG, readable_name='Time from end of frame on-air to TX EOF timestamp'))
        profile.outputs.append(ModelOutput(model.vars.rssi_adjust_db, '', ModelOutputType.RAIL_CONFIG, readable_name='RSSI compensation value calculated from decimation and digital gains'))
        profile.outputs.append(ModelOutput(model.vars.tx_sync_delay_ns, '', ModelOutputType.RAIL_CONFIG, readable_name='Time needed from TXEN to start of preamble on-air'))

    def profile_calculate(self, model):
        model.vars.protocol_id.value_forced = model.vars.protocol_id.var_enum.Zigbee
        self._build_shr_settings(model)
        self._build_frame_settings(model)
        self._build_chip_rate_settings(model)
        self._build_modulation_settings(model)
        self._build_feature_settings(model)
        self._build_modem_settings(model)

    def _build_shr_settings(self, model):
        model.vars.preamble_length.value_forced = 32
        model.vars.preamble_pattern.value_forced = 0
        model.vars.preamble_pattern_len.value_forced = 4

        model.vars.syncword_0.value = 0xe5
        model.vars.syncword_1.value = 0x0
        model.vars.syncword_length.value_forced = 8

    def _build_chip_rate_settings(self, model):
        base_freq_Hz = model.profile.inputs.base_frequency_hz.var_value

        if 2360e6 <= base_freq_Hz <= 2400e6 or 2400e6 <= base_freq_Hz <= 2500.0e6:
            model.vars.bitrate.value_forced = 250000
            model.vars.channel_spacing_hz.value_forced = 5000000
            model.vars.deviation.value_forced = 500000

            model.vars.dsss_chipping_code.value_forced = 0x744AC39B
            model.vars.dsss_len.value_forced = 32
            model.vars.dsss_spreading_factor.value_forced = 8
        elif 868e6 <= base_freq_Hz <= 868.6e6:
            model.vars.bitrate.value_forced = 100000
            model.vars.channel_spacing_hz.value_forced = 2000000
            model.vars.deviation.value_forced = 100000

            model.vars.dsss_chipping_code.value_forced = 0xA47C
            model.vars.dsss_len.value_forced = 16
            model.vars.dsss_spreading_factor.value_forced = 4
        else:
            model.vars.bitrate.value_forced = 250000
            model.vars.channel_spacing_hz.value_forced = 2000000
            model.vars.deviation.value_forced = 250000

            model.vars.dsss_chipping_code.value_forced = 0xA47C
            model.vars.dsss_len.value_forced = 16
            model.vars.dsss_spreading_factor.value_forced = 4

    def _build_frame_settings(self, model):
        base_freq_Hz = model.profile.inputs.base_frequency_hz.var_value

        # Packet Inputs
        model.vars.frame_coding.value_forced = model.vars.frame_coding.var_enum.NONE
        model.vars.frame_bitendian.value_forced = model.vars.frame_bitendian.var_enum.LSB_FIRST
        model.vars.frame_length_type.value_forced = model.vars.frame_length_type.var_enum.VARIABLE_LENGTH
        model.vars.payload_white_en.value_forced = False
        model.vars.payload_crc_en.value_forced = True
        model.vars.white_poly.value_forced = model.vars.white_poly.var_enum.NONE
        model.vars.white_seed.value_forced = 65535
        model.vars.white_output_bit.value_forced = 0
        model.vars.fec_en.value_forced = model.vars.fec_en.var_enum.NONE

        # Variable length includes header
        model.vars.header_en.value_forced = True
        model.vars.header_size.value_forced = 1
        model.vars.header_calc_crc.value_forced = False
        model.vars.header_white_en.value_forced = False

        model.vars.var_length_numbits.value_forced = 8
        model.vars.var_length_byteendian.value_forced = model.vars.var_length_byteendian.var_enum.LSB_FIRST
        model.vars.var_length_bitendian.value_forced = model.vars.var_length_bitendian.var_enum.LSB_FIRST
        model.vars.var_length_shift.value_forced = 0
        model.vars.var_length_adjust.value_forced = 0

        if 2360e6 <= base_freq_Hz <= 2400e6 or 2400e6 <= base_freq_Hz <= 2483.5e6:
            model.vars.var_length_minlength.value_forced = 4
        else:
            model.vars.var_length_minlength.value_forced = 5
        model.vars.var_length_maxlength.value_forced = 0x7F
        model.vars.var_length_includecrc.value_forced = True

        # CRC Inputs
        model.vars.crc_poly.value_forced = model.vars.crc_poly.var_enum.CCITT_16
        model.vars.crc_seed.value_forced = 0
        model.vars.crc_input_order.value_forced = model.vars.crc_input_order.var_enum.LSB_FIRST
        model.vars.crc_bit_endian.value_forced = model.vars.crc_bit_endian.var_enum.MSB_FIRST
        model.vars.crc_byte_endian.value_forced = model.vars.crc_byte_endian.var_enum.MSB_FIRST
        model.vars.crc_pad_input.value_forced = False
        model.vars.crc_invert.value_forced = False

        model.vars.syncword_tx_skip.value_forced = False
        model.vars.asynchronous_rx_enable.value_forced = False
        model.vars.test_ber.value_forced = False

        model.vars.fixed_length_size.value_forced = 1
        model.vars.frame_type_0_filter.value_forced = False
        model.vars.frame_type_0_length.value_forced = 16
        model.vars.frame_type_0_valid.value_forced = True
        model.vars.frame_type_1_filter.value_forced = False
        model.vars.frame_type_1_length.value_forced = 16
        model.vars.frame_type_1_valid.value_forced = True
        model.vars.frame_type_2_filter.value_forced = False
        model.vars.frame_type_2_length.value_forced = 16
        model.vars.frame_type_2_valid.value_forced = True
        model.vars.frame_type_3_filter.value_forced = False
        model.vars.frame_type_3_length.value_forced = 16
        model.vars.frame_type_3_valid.value_forced = True
        model.vars.frame_type_4_filter.value_forced = False
        model.vars.frame_type_4_length.value_forced = 16
        model.vars.frame_type_4_valid.value_forced = True
        model.vars.frame_type_5_filter.value_forced = False
        model.vars.frame_type_5_length.value_forced = 16
        model.vars.frame_type_5_valid.value_forced = True
        model.vars.frame_type_6_filter.value_forced = False
        model.vars.frame_type_6_length.value_forced = 16
        model.vars.frame_type_6_valid.value_forced = True
        model.vars.frame_type_7_filter.value_forced = False
        model.vars.frame_type_7_length.value_forced = 16
        model.vars.frame_type_7_valid.value_forced = True
        model.vars.frame_type_bits.value_forced = 3
        model.vars.frame_type_loc.value_forced = 0
        model.vars.frame_type_lsbit.value_forced = 0

    def _build_modulation_settings(self, model):
        model.vars.modulation_type.value_forced = model.vars.modulation_type.var_enum.OQPSK
        model.vars.symbol_encoding.value_forced = model.vars.symbol_encoding.var_enum.DSSS
        model.vars.manchester_mapping.value_forced = model.vars.manchester_mapping.var_enum.Default
        model.vars.fsk_symbol_map.value_forced = model.vars.fsk_symbol_map.var_enum.MAP0
        model.vars.diff_encoding_mode.value_forced = model.vars.diff_encoding_mode.var_enum.DISABLED
        model.vars.shaping_filter.value_forced = model.vars.shaping_filter.var_enum.Custom_OQPSK
        model.vars.shaping_filter_param.value_forced = 0.0

    def _build_feature_settings(self, model):
        if model.profile.inputs.zigbee_feature.var_value == model.vars.zigbee_feature.var_enum.STANDARD or \
                model.profile.inputs.zigbee_feature.var_value == model.vars.zigbee_feature.var_enum.FEM or \
                model.profile.inputs.zigbee_feature.var_value == model.vars.zigbee_feature.var_enum.COHERENT:
            model.vars.demod_select.value_forced = model.vars.demod_select.var_enum.COHERENT
        else:
            model.vars.demod_select.value_forced = model.vars.demod_select.var_enum.LEGACY

        if model.profile.inputs.zigbee_feature.var_value == model.vars.zigbee_feature.var_enum.ANTDIV or \
                model.profile.inputs.zigbee_feature.var_value == model.vars.zigbee_feature.var_enum.ANTDIV_FEM:
            model.vars.antdivmode.value_forced = model.vars.antdivmode.var_enum.ANTSELRSSI
            model.vars.antdivrepeatdis.value_forced = model.vars.antdivrepeatdis.var_enum.NOREPEATFIRST
        else:
            model.vars.antdivmode.value_forced = model.vars.antdivmode.var_enum.DISABLE

        if model.profile.inputs.zigbee_feature.var_value == model.vars.zigbee_feature.var_enum.FCS:
            model.vars.antdivmode.value_forced = model.vars.antdivmode.var_enum.ANTSELFIRST
            model.vars.antdivrepeatdis.value_forced = model.vars.antdivrepeatdis.var_enum.NOREPEATFIRST

    def _build_modem_settings(self, model):
        self._build_agc_fastloop_settings(model)

        if model.profile.inputs.zigbee_feature.var_value == model.vars.zigbee_feature.var_enum.FCS:
            self._build_legacy_settings(model)
            self._build_fast_switch_settings(model)
        elif model.vars.demod_select.value == model.vars.demod_select.var_enum.COHERENT:
            self._build_cohdsa_settings(model)
        else:
            self._build_legacy_settings(model)

    def _build_agc_fastloop_settings(self, model):
        model.vars.AGC_CTRL0_PWRTARGET.value_forced = 245
        model.vars.AGC_CTRL0_MODE.value_forced = 2
        model.vars.AGC_CTRL0_DISCFLOOPADJ.value_forced = 1
        model.vars.AGC_CTRL0_ADCATTENCODE.value_forced = 0
        model.vars.AGC_CTRL0_ADCATTENMODE.value_forced = 0
        model.vars.AGC_CTRL0_DISPNGAINUP.value_forced = 0
        model.vars.AGC_CTRL0_DISPNDWNCOMP.value_forced = 0
        model.vars.AGC_CTRL0_DISRESETCHPWR.value_forced = 0
        model.vars.AGC_CTRL0_AGCRST.value_forced = 0

        model.vars.rssi_period.value_forced = 3
        model.vars.AGC_CTRL1_PWRPERIOD.value_forced = 1
        if model.part_family.lower() in ["panther", "lynx", "leopard"]:
            model.vars.AGC_CTRL1_SUBPERIOD.value_forced = 0
            model.vars.AGC_CTRL1_SUBNUM.value_forced = 0
            model.vars.AGC_CTRL1_SUBDEN.value_forced = 0
            model.vars.AGC_CTRL1_SUBINT.value_forced = 0

        model.vars.AGC_CTRL2_PRSDEBUGEN.value_forced = 0
        model.vars.AGC_CTRL2_DMASEL.value_forced = 0
        model.vars.AGC_CTRL2_SAFEMODE.value_forced = 0
        model.vars.AGC_CTRL2_SAFEMODETHD.value_forced = 3
        model.vars.AGC_CTRL2_REHICNTTHD.value_forced = 7
        model.vars.AGC_CTRL2_RELOTHD.value_forced = 4
        model.vars.AGC_CTRL2_RELBYCHPWR.value_forced = 3
        model.vars.AGC_CTRL2_RELTARGETPWR.value_forced = 236
        model.vars.AGC_CTRL2_DISRFPKD.value_forced = 0

        model.vars.AGC_CTRL3_RFPKDDEB.value_forced = 1
        model.vars.AGC_CTRL3_RFPKDDEBTHD.value_forced = 1
        model.vars.AGC_CTRL3_RFPKDDEBPRD.value_forced = 40
        model.vars.AGC_CTRL3_RFPKDDEBRST.value_forced = 10
        model.vars.AGC_CTRL3_IFPKDDEB.value_forced = 1
        model.vars.AGC_CTRL3_IFPKDDEBTHD.value_forced = 1
        model.vars.AGC_CTRL3_IFPKDDEBPRD.value_forced = 40
        model.vars.AGC_CTRL3_IFPKDDEBRST.value_forced = 10

        if model.part_family.lower() in ["panther", "lynx", "leopard"]:
            model.vars.AGC_AGCPERIOD_PERIODHI.value_forced = 14
            model.vars.AGC_AGCPERIOD_PERIODLO.value_forced = 45
            model.vars.AGC_AGCPERIOD_MAXHICNTTHD.value_forced = 9
            model.vars.AGC_AGCPERIOD_SETTLETIMEIF.value_forced = 6
            model.vars.AGC_AGCPERIOD_SETTLETIMERF.value_forced = 14
            model.vars.AGC_GAINSTEPLIM_CFLOOPSTEPMAX.value_forced = 4
            model.vars.AGC_GAINSTEPLIM_CFLOOPDEL.value_forced = 0
            model.vars.AGC_GAINSTEPLIM_HYST.value_forced = 3
            model.vars.AGC_GAINSTEPLIM_MAXPWRVAR.value_forced = 0
            model.vars.AGC_GAINSTEPLIM_TRANRSTAGC.value_forced = 0
            model.vars.AGC_GAINRANGE_LNAINDEXBORDER.value_forced = 7
            model.vars.AGC_GAINRANGE_PGAINDEXBORDER.value_forced = 8
        model.vars.AGC_GAINRANGE_GAININCSTEP.value_forced = 1
        model.vars.AGC_GAINRANGE_LATCHEDHISTEP.value_forced = 0
        model.vars.AGC_GAINRANGE_PNGAINSTEP.value_forced = 4
        model.vars.AGC_GAINRANGE_HIPWRTHD.value_forced = 3

        model.vars.AGC_MANGAIN_MANGAINEN.value_forced = 0
        model.vars.AGC_MANGAIN_MANGAINIFPGA.value_forced = 7
        model.vars.AGC_MANGAIN_MANGAINLNA.value_forced = 1
        model.vars.AGC_MANGAIN_MANGAINPN.value_forced = 1

        if model.part_family.lower() in ["panther", "lynx", "leopard"]:
            model.vars.AGC_HICNTREGION_HICNTREGION0.value_forced = 4
            model.vars.AGC_HICNTREGION_HICNTREGION1.value_forced = 5
            model.vars.AGC_HICNTREGION_HICNTREGION2.value_forced = 6
            model.vars.AGC_HICNTREGION_HICNTREGION3.value_forced = 7
            model.vars.AGC_HICNTREGION_HICNTREGION4.value_forced = 8

        model.vars.AGC_STEPDWN_STEPDWN0.value_forced = 1
        model.vars.AGC_STEPDWN_STEPDWN1.value_forced = 2
        model.vars.AGC_STEPDWN_STEPDWN2.value_forced = 3
        model.vars.AGC_STEPDWN_STEPDWN3.value_forced = 3
        model.vars.AGC_STEPDWN_STEPDWN4.value_forced = 3
        model.vars.AGC_STEPDWN_STEPDWN5.value_forced = 5

        model.vars.AGC_RSSISTEPTHR_DEMODRESTARTPER.value_forced = 0
        model.vars.AGC_RSSISTEPTHR_DEMODRESTARTTHR.value_forced = 0
        model.vars.AGC_RSSISTEPTHR_POSSTEPTHR.value_forced = 0

        if model.part_family.lower() in ["panther", "lynx", "leopard"]:
            ##### RF peak detector threshold configuration
            model.vars.RAC_PGACTRL_LNAMIXRFPKDTHRESHSEL.value_forced = 2

        if model.part_family.lower() not in ["rainier"]:
            ##### IF peak detector configuration
            # enable I/Q latch
            model.vars.RAC_PGACTRL_PGAENLATCHI.value_forced = 1
            model.vars.RAC_PGACTRL_PGAENLATCHQ.value_forced = 1
            # set IF peak detector threshold
            model.vars.RAC_PGACTRL_PGATHRPKDHISEL.value_forced = 5
            model.vars.RAC_PGACTRL_PGATHRPKDLOSEL.value_forced = 1

    def _build_cohdsa_settings(self, model):
        model.vars.baudrate_tol_ppm.value_forced = 0
        model.vars.tx_xtal_error_ppm.value_forced = 0
        model.vars.rx_xtal_error_ppm.value_forced = 0

        model.vars.bandwidth_hz.value_forced = 2524800
        model.vars.if_frequency_hz.value_forced = 1370000

        model.vars.frequency_comp_mode.value_forced = model.vars.frequency_comp_mode.var_enum.DISABLED
        model.vars.pll_bandwidth_tx.value_forced = model.vars.pll_bandwidth_tx.var_enum.BW_1500KHz
        model.vars.pll_bandwidth_rx.value_forced = model.vars.pll_bandwidth_rx.var_enum.BW_250KHz

        model.vars.number_of_timing_windows.value_forced = 7
        model.vars.symbols_in_timing_window.value_forced = 12
        model.vars.timing_detection_threshold.value_forced = 35
        model.vars.timing_sample_threshold.value_forced = 0

        model.vars.target_osr.value_forced = 5  # Calc SRC

        model.vars.AGC_GAINRANGE_BOOSTLNA.value_forced = 1
        model.vars.AGC_GAINRANGE_LNABWADJ.value_forced = 0
        model.vars.AGC_CTRL0_PWRTARGET.value_forced = 20
        model.vars.AGC_CTRL1_PWRPERIOD.value_forced = 4
        model.vars.FRC_AUTOCG_AUTOCGEN.value_forced = 7
        model.vars.MODEM_AFC_AFCRXCLR.value_forced = 1
        model.vars.MODEM_AFC_AFCSCALEM.value_forced = 3
        model.vars.MODEM_AFCADJLIM_AFCADJLIM.value_forced = 2750
        model.vars.MODEM_CGCLKSTOP_FORCEOFF.value_forced = 0x1E00  # 9,10,11,12
        model.vars.MODEM_COH0_COHCHPWRTH0.value_forced = 33
        model.vars.MODEM_COH0_COHCHPWRTH1.value_forced = 40
        model.vars.MODEM_COH0_COHCHPWRTH2.value_forced = 100
        model.vars.MODEM_COH0_COHDYNAMICBBSSEN.value_forced = 1
        model.vars.MODEM_COH0_COHDYNAMICPRETHRESH.value_forced = 1
        model.vars.MODEM_COH0_COHDYNAMICSYNCTHRESH.value_forced = 1
        model.vars.MODEM_COH1_SYNCTHRESH0.value_forced = 20
        model.vars.MODEM_COH1_SYNCTHRESH1.value_forced = 23
        model.vars.MODEM_COH1_SYNCTHRESH2.value_forced = 26
        model.vars.MODEM_COH2_DSAPEAKCHPWRTH.value_forced = 200
        model.vars.MODEM_COH2_FIXEDCDTHFORIIR.value_forced = 105
        model.vars.MODEM_COH2_SYNCTHRESHDELTA1.value_forced = 2
        model.vars.MODEM_COH2_SYNCTHRESHDELTA2.value_forced = 4
        model.vars.MODEM_COH3_CDSS.value_forced = 4
        model.vars.MODEM_COH3_COHDSAADDWNDSIZE.value_forced = 80
        model.vars.MODEM_COH3_COHDSAEN.value_forced = 1
        model.vars.MODEM_COH3_DSAPEAKINDLEN.value_forced = 4
        model.vars.MODEM_COH3_DYNIIRCOEFOPTION.value_forced = 3
        model.vars.MODEM_COH3_LOGICBASEDCOHDEMODGATE.value_forced = 1
        model.vars.MODEM_COH3_PEAKCHKTIMOUT.value_forced = 18
        model.vars.MODEM_CTRL1_PHASEDEMOD.value_forced = 2
        model.vars.MODEM_CTRL2_DATAFILTER.value_forced = 7
        model.vars.MODEM_CTRL2_SQITHRESH.value_forced = 200
        model.vars.MODEM_CTRL5_BBSS.value_forced = 6
        model.vars.MODEM_CTRL5_DSSSCTD.value_forced = 1
        model.vars.MODEM_CTRL5_FOEPREAVG.value_forced = 7
        model.vars.MODEM_CTRL5_LINCORR.value_forced = 1
        model.vars.MODEM_CTRL5_POEPER.value_forced = 1
        model.vars.MODEM_CTRL6_ARW.value_forced = 1
        model.vars.MODEM_CTRL6_DSSS3SYMBOLSYNCEN.value_forced = 1
        model.vars.MODEM_CTRL6_PSTIMABORT0.value_forced = 1
        model.vars.MODEM_CTRL6_PSTIMABORT1.value_forced = 1
        model.vars.MODEM_CTRL6_PSTIMABORT2.value_forced = 1
        model.vars.MODEM_CTRL6_RXBRCALCDIS.value_forced = 1
        model.vars.MODEM_CTRL6_TDREW.value_forced = 64
        model.vars.MODEM_CTRL6_TIMTHRESHGAIN.value_forced = 2
        model.vars.MODEM_DIGIGAINCTRL_DIGIGAINEN.value_forced = 1
        model.vars.MODEM_DIGIGAINCTRL_DIGIGAINSEL.value_forced = 20
        model.vars.MODEM_DSACTRL_ARRTHD.value_forced = 4  # Was missed
        model.vars.MODEM_INTAFC_FOEPREAVG0.value_forced = 1
        model.vars.MODEM_INTAFC_FOEPREAVG1.value_forced = 3
        model.vars.MODEM_INTAFC_FOEPREAVG2.value_forced = 5
        model.vars.MODEM_INTAFC_FOEPREAVG3.value_forced = 5
        model.vars.MODEM_LONGRANGE1_AVGWIN.value_forced = 4
        model.vars.MODEM_LONGRANGE1_HYSVAL.value_forced = 3
        model.vars.MODEM_LONGRANGE1_LRTIMEOUTTHD.value_forced = 320
        model.vars.MODEM_LONGRANGE2_LRCHPWRTH1.value_forced = 20
        model.vars.MODEM_LONGRANGE2_LRCHPWRTH2.value_forced = 26
        model.vars.MODEM_LONGRANGE2_LRCHPWRTH3.value_forced = 33
        model.vars.MODEM_LONGRANGE2_LRCHPWRTH4.value_forced = 40
        model.vars.MODEM_LONGRANGE3_LRCHPWRTH5.value_forced = 46
        model.vars.MODEM_LONGRANGE3_LRCHPWRTH6.value_forced = 52
        model.vars.MODEM_LONGRANGE3_LRCHPWRTH7.value_forced = 59
        model.vars.MODEM_LONGRANGE3_LRCHPWRTH8.value_forced = 66
        model.vars.MODEM_LONGRANGE4_LRCHPWRSH1.value_forced = 3
        model.vars.MODEM_LONGRANGE4_LRCHPWRSH2.value_forced = 4
        model.vars.MODEM_LONGRANGE4_LRCHPWRSH3.value_forced = 5
        model.vars.MODEM_LONGRANGE4_LRCHPWRSH4.value_forced = 5
        model.vars.MODEM_LONGRANGE4_LRCHPWRTH10.value_forced = 80
        model.vars.MODEM_LONGRANGE4_LRCHPWRTH9.value_forced = 73
        model.vars.MODEM_LONGRANGE5_LRCHPWRSH10.value_forced = 11
        model.vars.MODEM_LONGRANGE5_LRCHPWRSH11.value_forced = 12
        model.vars.MODEM_LONGRANGE5_LRCHPWRSH5.value_forced = 6
        model.vars.MODEM_LONGRANGE5_LRCHPWRSH6.value_forced = 7
        model.vars.MODEM_LONGRANGE5_LRCHPWRSH7.value_forced = 8
        model.vars.MODEM_LONGRANGE5_LRCHPWRSH8.value_forced = 9
        model.vars.MODEM_LONGRANGE5_LRCHPWRSH9.value_forced = 10
        model.vars.MODEM_LONGRANGE6_LRCHPWRSPIKETH.value_forced = 40
        model.vars.MODEM_LONGRANGE6_LRSPIKETHD.value_forced = 105

        model.vars.MODEM_TXBR_TXBRDEN.value_forced = 105
        model.vars.MODEM_TXBR_TXBRNUM.value_forced = 252

        model.vars.RAC_PGACTRL_LNAMIXRFPKDTHRESHSEL.value_forced = 2
        model.vars.RAC_PGACTRL_LNAMIXRFPKDTHRESHSEL.value_forced = 2
        model.vars.RAC_PGACTRL_PGATHRPKDHISEL.value_forced = 5
        model.vars.RAC_PGACTRL_PGATHRPKDLOSEL.value_forced = 1
        model.vars.RAC_SYNTHCTRL_MMDPOWERBALANCEDISABLE.value_forced = 1
        model.vars.RAC_SYNTHREGCTRL_MMDLDOVREFTRIM.value_forced = 3
        model.vars.RAC_PGACTRL_PGAENLATCHI.value_forced = 1
        model.vars.RAC_PGACTRL_PGAENLATCHQ.value_forced = 1
        model.vars.RFCRC_CTRL_INPUTINV.value_forced = 0
        model.vars.SYNTH_LPFCTRL1CAL_OP1BWCAL.value_forced = 11
        model.vars.SYNTH_LPFCTRL1CAL_OP1COMPCAL.value_forced = 14
        model.vars.SYNTH_LPFCTRL1CAL_RFBVALCAL.value_forced = 0
        model.vars.SYNTH_LPFCTRL1CAL_RPVALCAL.value_forced = 0
        model.vars.SYNTH_LPFCTRL1CAL_RZVALCAL.value_forced = 9
        # Derived empirically
        # https://confluence.silabs.com/display/RAIL/Panther+Weaksymbols+WifiBlocker+Characterization
        model.vars.MODEM_CTRL2_SQITHRESH.value_forced = 56

    def _build_legacy_settings(self, model):
        model.vars.bandwidth_hz.value_forced = 2524800
        model.vars.if_frequency_hz.value_forced = 1370000
        model.vars.baudrate_tol_ppm.value_forced = 4000
        model.vars.tx_xtal_error_ppm.value_forced = 0
        model.vars.rx_xtal_error_ppm.value_forced = 0

        model.vars.frequency_comp_mode.value_forced = model.vars.frequency_comp_mode.var_enum.INTERNAL_ALWAYS_ON
        model.vars.pll_bandwidth_tx.value_forced = model.vars.pll_bandwidth_tx.var_enum.BW_1500KHz
        model.vars.pll_bandwidth_rx.value_forced = model.vars.pll_bandwidth_rx.var_enum.BW_250KHz

        model.vars.timing_detection_threshold.value_forced = 75
        model.vars.timing_resync_period.value_forced = 2
        model.vars.timing_sample_threshold.value_forced = 0

        model.vars.AGC_CTRL1_PWRPERIOD.value_forced = 4
        model.vars.AGC_GAINRANGE_BOOSTLNA.value_forced = 1
        model.vars.AGC_GAINRANGE_LNABWADJ.value_forced = 1
        model.vars.FRC_AUTOCG_AUTOCGEN.value_forced = 7
        model.vars.MODEM_CGCLKSTOP_FORCEOFF.value_forced = 0x1003  # 0, 1, 12
        model.vars.RAC_PGACTRL_LNAMIXRFPKDTHRESHSEL.value_forced = 2
        model.vars.RAC_PGACTRL_PGATHRPKDHISEL.value_forced = 5
        model.vars.RAC_PGACTRL_PGATHRPKDLOSEL.value_forced = 1
        model.vars.RAC_SYNTHCTRL_MMDPOWERBALANCEDISABLE.value_forced = 1
        model.vars.RAC_SYNTHREGCTRL_MMDLDOVREFTRIM.value_forced = 3
        model.vars.RAC_PGACTRL_PGAENLATCHI.value_forced = 1
        model.vars.RAC_PGACTRL_PGAENLATCHQ.value_forced = 1
        model.vars.SYNTH_LPFCTRL1CAL_OP1BWCAL.value_forced = 11
        model.vars.SYNTH_LPFCTRL1CAL_OP1COMPCAL.value_forced = 14
        model.vars.SYNTH_LPFCTRL1CAL_RFBVALCAL.value_forced = 0
        model.vars.SYNTH_LPFCTRL1CAL_RPVALCAL.value_forced = 0
        model.vars.SYNTH_LPFCTRL1CAL_RZVALCAL.value_forced = 9

    def _build_fast_switch_settings(self, model):
        """ Bandwidth """
        model.vars.pll_bandwidth_rx.value_forced = model.vars.pll_bandwidth_rx.var_enum.FASTSWITCH

        # additional registers set in: RFHAL_IEEE802154_ConfigAntdivForRxChSwitching
        model.vars.MODEM_INTAFC_FOEPREAVG2.value_forced = 4
        model.vars.MODEM_INTAFC_FOEPREAVG1.value_forced = 2
        model.vars.MODEM_INTAFC_FOEPREAVG0.value_forced = 1

        model.vars.MODEM_TIMING_TIMINGBASES.value_forced = 2

        model.vars.MODEM_CTRL6_TDREW.value_forced = 0x20
        model.vars.MODEM_CTRL6_PSTIMABORT0.value_forced = 1
        model.vars.MODEM_CTRL6_PSTIMABORT1.value_forced = 1
        model.vars.MODEM_CTRL6_PSTIMABORT2.value_forced = 1
        model.vars.MODEM_CTRL5_LINCORR.value_forced = 0

        model.vars.MODEM_CTRL1_PHASEDEMOD.value_forced = 0
        model.vars.MODEM_TIMING_TIMTHRESH.value_forced = 92
        model.vars.SYNTH_DSMCTRLRX_DITHERDSMOUTPUTRX.value_forced = 3

        # : improve +10 MHz cw blocking performance
        # : MCUW_RADIO_CFG-2298
        model.vars.MODEM_DCCOMP_DCCOMPGEAR.value_forced = 7
