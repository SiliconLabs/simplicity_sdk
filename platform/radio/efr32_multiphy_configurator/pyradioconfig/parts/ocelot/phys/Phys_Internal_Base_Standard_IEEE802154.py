from pyradioconfig.parts.common.phys.phy_common import PHY_COMMON_FRAME_154
from pyradioconfig.parts.lynx.phys.Phys_RAIL_Base_Standard_IEEE802154 import PhysRAILBaseStandardIEEE802154Lynx
from pyradioconfig.parts.common.phys.phy_common import PHY_COMMON_FRAME_INTERNAL
from pyradioconfig.parts.panther.phys.PHY_internal_base import Phy_Internal_Base
from py_2_and_3_compatibility import *


class PhysInternalBaseStandardIEEE802154Ocelot(PhysRAILBaseStandardIEEE802154Lynx):
    # For now, inherit all from Lynx

    # Manual copy/paste of sub-GHz 802.15.4 PHYs from Jumbo
    # These do not inherit from Lynx, as Lynx excludes sub-GHz
    #
    # Useful to copy here as many register overrides do not exist on Series 2

    def IEEE802154_Base(self, phy, model):
        # Inputs
        phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED
        phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP0
        phy.profile_inputs.preamble_pattern.value = 0
        phy.profile_inputs.preamble_pattern_len.value = 4
        phy.profile_inputs.rx_xtal_error_ppm.value = 0
        phy.profile_inputs.symbol_encoding.value = model.vars.symbol_encoding.var_enum.DSSS
        phy.profile_inputs.syncword_0.value = long(0xe5)
        phy.profile_inputs.syncword_1.value = long(0x0)
        phy.profile_inputs.syncword_length.value = 8
        phy.profile_inputs.timing_sample_threshold.value = 0
        phy.profile_inputs.tx_xtal_error_ppm.value = 0
        phy.profile_inputs.xtal_frequency_hz.value = 38400000

        # Add 15.4 Packet Configuration
        PHY_COMMON_FRAME_154(phy, model)

    def PHY_IEEE802154_780MHz_OQPSK(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='IEEE 802.15.4 780MHz OQPSK', phy_name=phy_name)

        self.IEEE802154_Base(phy, model)

        phy.profile_inputs.agc_hysteresis.value = 0
        phy.profile_inputs.agc_power_target.value = -15
        phy.profile_inputs.bandwidth_hz.value = 1262400
        phy.profile_inputs.base_frequency_hz.value =  long(780000000)
        phy.profile_inputs.baudrate_tol_ppm.value = 8000
        phy.profile_inputs.bitrate.value = 250000
        phy.profile_inputs.channel_spacing_hz.value = 2000000
        phy.profile_inputs.deviation.value = 250000
        phy.profile_inputs.dsss_chipping_code.value =  long(0xA47C)
        phy.profile_inputs.dsss_len.value = 16
        phy.profile_inputs.dsss_spreading_factor.value = 4
        phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.INTERNAL_ALWAYS_ON
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.OQPSK
        phy.profile_inputs.number_of_timing_windows.value = 3
        phy.profile_inputs.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_1200KHz
        phy.profile_inputs.preamble_length.value = 32
        phy.profile_inputs.preamble_pattern_len.value = 4
        phy.profile_inputs.rssi_period.value = 7
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Custom_OQPSK
        phy.profile_inputs.shaping_filter_param.value = 0.0
        phy.profile_inputs.syncword_length.value = 8
        phy.profile_inputs.timing_detection_threshold.value = 35

        phy.profile_outputs.AGC_GAINSTEPLIM0_CFLOOPSTEPMAX.override = 4

        return phy

    def IEEE802154_915MHz_OQPSK(self, phy, model):
        #phy.profile_inputs.agc_power_target.value = -15 # : TODO remove
        phy.profile_inputs.bandwidth_hz.value = 1262400
        phy.profile_inputs.base_frequency_hz.value = long(906000000)
        phy.profile_inputs.baudrate_tol_ppm.value = 8000
        phy.profile_inputs.bitrate.value = 250000
        phy.profile_inputs.channel_spacing_hz.value = 2000000
        phy.profile_inputs.deviation.value = 250000
        phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED
        phy.profile_inputs.dsss_chipping_code.value = long(0xA47C)
        phy.profile_inputs.dsss_len.value = 16
        phy.profile_inputs.dsss_spreading_factor.value = 4
        phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP0
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.OQPSK
        phy.profile_inputs.preamble_length.value = 32
        phy.profile_inputs.preamble_pattern.value = 0
        phy.profile_inputs.preamble_pattern_len.value = 4
        #phy.profile_inputs.rssi_period.value = 7 # : TODO remove
        phy.profile_inputs.rx_xtal_error_ppm.value = 25
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Custom_OQPSK
        phy.profile_inputs.shaping_filter_param.value = 0.0
        phy.profile_inputs.symbol_encoding.value = model.vars.symbol_encoding.var_enum.DSSS
        phy.profile_inputs.syncword_0.value = long(0xe5)
        phy.profile_inputs.syncword_1.value = long(0x0)
        phy.profile_inputs.syncword_length.value = 8
        #phy.profile_inputs.timing_detection_threshold.value = 35 # : TODO remove
        #phy.profile_inputs.timing_sample_threshold.value = 0 # : TODO remove
        phy.profile_inputs.tx_xtal_error_ppm.value = 25
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        # Add 15.4 Packet Configuration
        PHY_COMMON_FRAME_154(phy, model)


    def PHY_IEEE802154_915MHz_OQPSK(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='IEEE 802.15.4 915MHz OQPSK', phy_name=phy_name)

        self.IEEE802154_915MHz_OQPSK(phy, model)
        phy.profile_inputs.demod_select.value = model.vars.demod_select.var_enum.LEGACY
        phy.profile_inputs.number_of_timing_windows.value = 3

        return phy

    def PHY_IEEE802154_915MHz_OQPSK_coh(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='IEEE 802.15.4 915MHz OQPSK Coherent Demod', phy_name=phy_name, tags="-FPGA")

        self.IEEE802154_915MHz_OQPSK(phy, model)
        phy.profile_inputs.deviation.value = 250000
        phy.profile_inputs.demod_select.value = model.vars.demod_select.var_enum.COHERENT
        phy.profile_inputs.timing_detection_threshold.value = 35
        #phy.profile_outputs.AGC_AGCPERIOD0_MAXHICNTTHD.override = 13
        #phy.profile_outputs.AGC_AGCPERIOD0_PERIODHI.override = 14
        #phy.profile_outputs.AGC_AGCPERIOD1_PERIODLOW.override = 42
        #phy.profile_outputs.AGC_AGCPERIOD0_SETTLETIMERF.override = 14
        # phy.profile_outputs.AGC_CTRL0_DISPNGAINUP.override = 0
        # phy.profile_outputs.AGC_CTRL0_MODE.override = 2
        phy.profile_outputs.AGC_CTRL0_PWRTARGET.override = 20
        phy.profile_outputs.AGC_CTRL1_RSSIPERIOD.override = 3
        # phy.profile_outputs.AGC_CTRL2_REHICNTTHD.override = 7
        # phy.profile_outputs.AGC_CTRL2_SAFEMODE.override = 0
        # phy.profile_outputs.AGC_CTRL2_SAFEMODETHD.override = 3
        # phy.profile_outputs.AGC_CTRL3_IFPKDDEB.override = 1
        # phy.profile_outputs.AGC_CTRL3_IFPKDDEBPRD.override = 40
        # phy.profile_outputs.AGC_CTRL3_IFPKDDEBRST.override = 10
        # phy.profile_outputs.AGC_CTRL3_IFPKDDEBTHD.override = 1
        # phy.profile_outputs.AGC_CTRL3_RFPKDDEB.override = 0
        # phy.profile_outputs.AGC_CTRL3_RFPKDDEBTHD.override = 1
        # phy.profile_outputs.AGC_CTRL4_PERIODRFPKD.override = 4000
        # phy.profile_outputs.AGC_CTRL4_RFPKDPRDGEAR.override = 4
        # phy.profile_outputs.AGC_CTRL4_RFPKDSEL.override = 1
        # phy.profile_outputs.AGC_CTRL4_RFPKDSYNCSEL.override = 1
        # phy.profile_outputs.AGC_CTRL5_PNUPDISTHD.override = 32
        # phy.profile_outputs.AGC_CTRL5_PNUPRELTHD.override = 4
        # phy.profile_outputs.AGC_CTRL6_SEQRFPKDEN.override = 0
        # phy.profile_outputs.AGC_GAINRANGE_BOOSTLNA.override = 1
        # phy.profile_outputs.AGC_GAINRANGE_HIPWRTHD.override = 3
        # phy.profile_outputs.AGC_GAINRANGE_LATCHEDHISTEP.override = 0
        # phy.profile_outputs.AGC_GAINRANGE_PNGAINSTEP.override = 4
        # phy.profile_outputs.AGC_GAINSTEPLIM0_CFLOOPDEL.override = 50
        # phy.profile_outputs.AGC_GAINSTEPLIM0_HYST.override = 3
        # phy.profile_outputs.AGC_HICNTREGION0_HICNTREGION0.override = 8
        # phy.profile_outputs.AGC_HICNTREGION0_HICNTREGION1.override = 10
        # phy.profile_outputs.AGC_HICNTREGION0_HICNTREGION2.override = 12
        # phy.profile_outputs.AGC_HICNTREGION0_HICNTREGION3.override = 12
        # phy.profile_outputs.AGC_HICNTREGION1_HICNTREGION4.override = 13
        # phy.profile_outputs.AGC_MANGAIN_MANGAINIFPGA.override = 7
        # phy.profile_outputs.AGC_MANGAIN_MANGAINLNA.override = 1
        # phy.profile_outputs.AGC_MANGAIN_MANGAINPN.override = 1
        # phy.profile_outputs.AGC_STEPDWN_STEPDWN2.override = 3
        # phy.profile_outputs.AGC_STEPDWN_STEPDWN4.override = 3
        phy.profile_outputs.MODEM_AFCADJLIM_AFCADJLIM.override = 658
        phy.profile_outputs.MODEM_COH0_COHCHPWRTH1.override = 19
        phy.profile_outputs.MODEM_COH0_COHCHPWRTH2.override = 79
        phy.profile_outputs.MODEM_COH0_COHDYNAMICBBSSEN.override = 1
        phy.profile_outputs.MODEM_COH0_COHDYNAMICPRETHRESH.override = 1
        phy.profile_outputs.MODEM_COH0_COHDYNAMICSYNCTHRESH.override = 0
        phy.profile_outputs.MODEM_COH1_SYNCTHRESH0.override = 31
        phy.profile_outputs.MODEM_COH1_SYNCTHRESH1.override = 21
        phy.profile_outputs.MODEM_COH1_SYNCTHRESH2.override = 21
        phy.profile_outputs.MODEM_COH1_SYNCTHRESH3.override = 46
        phy.profile_outputs.MODEM_COH2_SYNCTHRESHDELTA2.override = 5
        phy.profile_outputs.MODEM_COH3_CDSS.override = 4
        phy.profile_outputs.MODEM_COH3_COHDSAADDWNDSIZE.override = 864 #-160
        phy.profile_outputs.MODEM_COH3_COHDSAEN.override = 1
        phy.profile_outputs.MODEM_COH3_DYNIIRCOEFOPTION.override = 3
        phy.profile_outputs.MODEM_CTRL1_COMPMODE.override = 0
        phy.profile_outputs.MODEM_CTRL1_PHASEDEMOD.override = 2
        phy.profile_outputs.MODEM_CTRL2_DATAFILTER.override = 7
        phy.profile_outputs.MODEM_CTRL5_LINCORR.override = 0
        phy.profile_outputs.MODEM_CTRL5_BBSS.override = 5
        phy.profile_outputs.MODEM_CTRL5_DSSSCTD.override = 1
        phy.profile_outputs.MODEM_CTRL5_FOEPREAVG.override = 7
        phy.profile_outputs.MODEM_CTRL5_POEPER.override = 1
        phy.profile_outputs.MODEM_CTRL6_ARW.override = 1
        phy.profile_outputs.MODEM_CTRL6_PSTIMABORT1.override = 1
        phy.profile_outputs.MODEM_CTRL6_RXBRCALCDIS.override = 1
        phy.profile_outputs.MODEM_CTRL6_TDREW.override = 32
        phy.profile_outputs.MODEM_CTRL6_TIMTHRESHGAIN.override = 1
        phy.profile_outputs.MODEM_CTRL6_PSTIMABORT0.override = 0
        phy.profile_outputs.MODEM_CTRL6_PSTIMABORT2.override = 0
        phy.profile_outputs.MODEM_INTAFC_FOEPREAVG0.override = 1
        phy.profile_outputs.MODEM_INTAFC_FOEPREAVG1.override = 2
        phy.profile_outputs.MODEM_INTAFC_FOEPREAVG2.override = 4
        phy.profile_outputs.MODEM_INTAFC_FOEPREAVG3.override = 4
        phy.profile_outputs.MODEM_LONGRANGE1_AVGWIN.override = 4
        phy.profile_outputs.MODEM_LONGRANGE1_CHPWRACCUDEL.override = 1
        phy.profile_outputs.MODEM_LONGRANGE1_HYSVAL.override = 3
        phy.profile_outputs.MODEM_LONGRANGE1_LRTIMEOUTTHD.override = 160
        phy.profile_outputs.MODEM_LONGRANGE2_LRCHPWRTH1.override = 19
        phy.profile_outputs.MODEM_LONGRANGE2_LRCHPWRTH2.override = 25
        phy.profile_outputs.MODEM_LONGRANGE2_LRCHPWRTH3.override = 31
        phy.profile_outputs.MODEM_LONGRANGE2_LRCHPWRTH4.override = 37
        phy.profile_outputs.MODEM_LONGRANGE3_LRCHPWRTH5.override = 43
        phy.profile_outputs.MODEM_LONGRANGE3_LRCHPWRTH6.override = 49
        phy.profile_outputs.MODEM_LONGRANGE3_LRCHPWRTH7.override = 55
        phy.profile_outputs.MODEM_LONGRANGE3_LRCHPWRTH8.override = 61
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRSH1.override = 3
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRSH2.override = 4
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRSH3.override = 5
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRSH4.override = 6
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRTH10.override = 73
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRTH9.override = 67
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH10.override = 12
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH11.override = 13
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH5.override = 7
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH6.override = 8
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH7.override = 9
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH8.override = 10
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH9.override = 11
        phy.profile_outputs.MODEM_LONGRANGE6_LRCHPWRSPIKETH.override = 127
        phy.profile_outputs.MODEM_LONGRANGE6_LRSPIKETHD.override = 50
        phy.profile_outputs.MODEM_MODINDEX_FREQGAINE.override = 7
        phy.profile_outputs.MODEM_MODINDEX_FREQGAINM.override = 1
        phy.profile_outputs.MODEM_PRE_PREERRORS.override = 15
        #phy.profile_outputs.MODEM_REALTIMCFE_MINCOSTTHD.override = 800
        #phy.profile_outputs.MODEM_REALTIMCFE_RTCFEEN.override = 1
        #phy.profile_outputs.MODEM_REALTIMCFE_RTSCHWIN.override = 10
        #phy.profile_outputs.MODEM_REALTIMCFE_VTAFCFRAME.override = 1
        phy.profile_outputs.MODEM_TIMING_ADDTIMSEQ.override = 6
        phy.profile_outputs.MODEM_TIMING_OFFSUBDEN.override = 5
        phy.profile_outputs.MODEM_TIMING_OFFSUBNUM.override = 7
        phy.profile_outputs.MODEM_TIMING_TIMINGBASES.override = 3
        phy.profile_outputs.MODEM_TIMING_TIMTHRESH.override = 25
        #phy.profile_outputs.MODEM_TRECPMDET_PMACQUINGWIN.override = 1
        #phy.profile_outputs.MODEM_TRECPMDET_PMMINCOSTTHD.override = 120
        #phy.profile_outputs.MODEM_TRECPMDET_PMTIMEOUTSEL.override = 2
        phy.profile_outputs.MODEM_CTRL1_RESYNCPER.override = 1
        phy.profile_outputs.MODEM_LONGRANGE1_PREFILTLEN.override = 1
        phy.profile_outputs.MODEM_PREFILTCOEFF_PREFILTCOEFF.override = 2736235287
        phy.profile_outputs.MODEM_COH3_COHDSACMPLX.override = 0
        phy.profile_outputs.MODEM_SYNCPROPERTIES_STATICSYNCTHRESHEN.override = 1
        phy.profile_outputs.MODEM_SYNCPROPERTIES_STATICSYNCTHRESH.override = 50
        phy.profile_outputs.MODEM_COH0_COHCHPWRLOCK.override = 0
        phy.profile_outputs.MODEM_COH0_COHCHPWRRESTART.override = 1


        return phy

    def PHY_IEEE802154_868MHz_OQPSK(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='IEEE 802.15.4 868MHz OQPSK', phy_name=phy_name)

        self.IEEE802154_915MHz_OQPSK(phy, model)
        phy.profile_inputs.bitrate.value = 100000
        phy.profile_inputs.timing_detection_threshold.value = 35
        phy.profile_inputs.number_of_timing_windows.value = 3
        phy.profile_inputs.deviation.value = 100000
        phy.profile_inputs.dsss_spreading_factor.value = 4
        phy.profile_inputs.dsss_len.value = 16
        phy.profile_inputs.bandwidth_hz.value = 504960
        phy.profile_inputs.demod_select.value = model.vars.demod_select.var_enum.LEGACY

        return phy

    # Owner: Young-Joon Choi
    # Jira Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-161
    def PHY_IEEE802154_868MHz_OQPSK_coh(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='IEEE 802.15.4 868MHz OQPSK Coherent Demod',
                            phy_name=phy_name)

        self.IEEE802154_915MHz_OQPSK(phy, model)
        phy.profile_inputs.demod_select.value = model.vars.demod_select.var_enum.COHERENT
        phy.profile_inputs.base_frequency_hz.value = 868300000
        phy.profile_inputs.bitrate.value = 100000
        phy.profile_inputs.deviation.value = 100000
        phy.profile_inputs.bandwidth_hz.value = 504960

        """ Complex Correlation """
        phy.profile_outputs.MODEM_CTRL6_CPLXCORREN.override = 1
        phy.profile_outputs.MODEM_COH3_COHDSACMPLX.override = 1

        """ Channel Power Accumulator Setting """
        # : Average and delay
        phy.profile_outputs.MODEM_LONGRANGE1_AVGWIN.override = 1
        phy.profile_outputs.MODEM_LONGRANGE1_CHPWRACCUDEL.override = 0

        """ BBSS """
        # : BBSS Channel Power Thresholds
        phy.profile_outputs.MODEM_LONGRANGE2_LRCHPWRTH1.override = 12
        phy.profile_outputs.MODEM_LONGRANGE2_LRCHPWRTH2.override = 18
        phy.profile_outputs.MODEM_LONGRANGE2_LRCHPWRTH3.override = 28
        phy.profile_outputs.MODEM_LONGRANGE2_LRCHPWRTH4.override = 35
        phy.profile_outputs.MODEM_LONGRANGE3_LRCHPWRTH5.override = 42
        phy.profile_outputs.MODEM_LONGRANGE3_LRCHPWRTH6.override = 48
        phy.profile_outputs.MODEM_LONGRANGE3_LRCHPWRTH7.override = 54
        phy.profile_outputs.MODEM_LONGRANGE3_LRCHPWRTH8.override = 60
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRTH9.override = 65
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRTH10.override = 70
        phy.profile_outputs.MODEM_LONGRANGE6_LRCHPWRTH11.override = 78

        # : BBSS lookup table
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRSH1.override = 2
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRSH2.override = 3
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRSH3.override = 4
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRSH4.override = 4
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH5.override = 5
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH6.override = 6
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH7.override = 7
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH8.override = 8
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH9.override = 9
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH10.override = 10
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH11.override = 11
        phy.profile_outputs.MODEM_LONGRANGE6_LRCHPWRSH12.override = 12

        """ Datafilter """
        # : Per's recommended DATAFILTER value is 7; however, with cplxcorren =1, DATAFILTER = 4 gives improved sens.
        phy.profile_outputs.MODEM_CTRL2_DATAFILTER.override = 4

        """ DSA Prefilter """
        phy.profile_outputs.MODEM_PREFILTCOEFF_PREFILTCOEFF.override = 2736235287

        """ Coherent DSA Settings """
        # : Enable coherent demodulator DSA
        phy.profile_outputs.MODEM_COH3_COHDSAEN.override = 1
        phy.profile_outputs.MODEM_COH3_CDSS.override = 4

        # : This threshold determines whether to use fixed or dynamic threshold based on channel power.
        # : > 128, FIXED DSA THRESHOLD ALWAYS
        # : = 0  , DYNAMIC DSA THRESHOLD ALWAYS
        # : 0 < x < 128, hybrid - for low channel power, use fixed threshold. For high channel power, use dynamic.
        phy.profile_outputs.MODEM_LONGRANGE6_LRCHPWRSPIKETH.override = 35

        # : For FIXED DSA mode, this is the correlation threshold
        phy.profile_outputs.MODEM_LONGRANGE6_LRSPIKETHD.override = 120  # : below 130, floor issues

        # : For dynamic DSA threshold, this is the baseline threshold. Threshold will increase in addition to this
        # : baseline value dependent on the channel power.
        phy.profile_outputs.MODEM_COH2_FIXEDCDTHFORIIR.override = 120  # Above 120, blocking degradation
        # : Coefficients for IIR, 0 -> 2^-3, 1 -> 2^-4, 2 -> 2^-5 , 3 -> 2^-6, Higher the value slower averaging.
        phy.profile_outputs.MODEM_COH3_DYNIIRCOEFOPTION.override = 3

        phy.profile_outputs.MODEM_COH3_PEAKCHKTIMOUT.override = 18

        """ Timing Detection Settings """
        # : Offset between DSA and timing windows
        # : Calculate as OSR*no_of_chips_per_sym*m
        phy.profile_outputs.MODEM_COH3_COHDSAADDWNDSIZE.override = 864  #-160 ( 864 = 2^10 - 160 )

        """ AFC """
        # : Controls number of bauds to rewind after fixed window timing detection.
        # : baudrate offset range is dependent on this value. Setting to 32 narrows baudrate offset range
        phy.profile_outputs.MODEM_CTRL6_TDREW.override = 64

        """ Dynamic Preamble / Sync """
        phy.profile_outputs.MODEM_CTRL5_RESYNCBAUDTRANS.override = 0

        phy.profile_outputs.MODEM_COH0_COHCHPWRTH0.override = 35  # same as LRCHPWRTH3
        phy.profile_outputs.MODEM_COH0_COHCHPWRTH1.override = 42  # same as LRCHPWRTH4
        phy.profile_outputs.MODEM_COH0_COHCHPWRTH2.override = 127

        phy.profile_outputs.MODEM_COH1_SYNCTHRESH0.override = 29  # 33 < COHCHPWRTH0
        phy.profile_outputs.MODEM_COH1_SYNCTHRESH1.override = 32  # 36 COHCHPWRTH0 < x < COHCHPWRTH1
        phy.profile_outputs.MODEM_COH1_SYNCTHRESH2.override = 35  # 39 COHCHPWRTH1 < x < COHCHPWRTH2
        phy.profile_outputs.MODEM_COH1_SYNCTHRESH3.override = 127  # > COHCHPWRTH2

        phy.profile_outputs.MODEM_COH2_SYNCTHRESHDELTA0.override = 0  # < COHCHPWRTH0
        phy.profile_outputs.MODEM_COH2_SYNCTHRESHDELTA1.override = 2  # 2  # COHCHPWRTH0 < x < COHCHPWRTH1
        phy.profile_outputs.MODEM_COH2_SYNCTHRESHDELTA2.override = 4  # 4  # COHCHPWRTH1 < x < COHCHPWRTH2
        phy.profile_outputs.MODEM_COH2_SYNCTHRESHDELTA3.override = 0  # > COHCHPWRTH2
        return phy

    def PHY_IEEE802154_868MHz_BPSK(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='Legacy IEEE 802.15.4 868MHz BPSK from Dumbo', phy_name=phy_name)

        self.IEEE802154_Base(phy, model)

        phy.profile_inputs.agc_hysteresis.value = 0
        phy.profile_inputs.agc_period.value = 3
        phy.profile_inputs.agc_power_target.value = -17
        phy.profile_inputs.base_frequency_hz.value =  868_000_000
        phy.profile_inputs.baudrate_tol_ppm.value = 1875
        phy.profile_inputs.bitrate.value = 20000
        phy.profile_inputs.channel_spacing_hz.value = 0
        phy.profile_inputs.deviation.value = 150000
        phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.RE0
        phy.profile_inputs.dsss_chipping_code.value =  long(0x9AF)
        phy.profile_inputs.dsss_len.value = 15
        phy.profile_inputs.dsss_spreading_factor.value = 15
        phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.INTERNAL_ALWAYS_ON
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.BPSK
        phy.profile_inputs.number_of_timing_windows.value = 11
        phy.profile_inputs.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_2000KHz
        phy.profile_inputs.preamble_length.value = 128
        phy.profile_inputs.rssi_period.value = 7
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Raised_Cosine
        phy.profile_inputs.shaping_filter_param.value = 1.0
        phy.profile_inputs.timing_detection_threshold.value = 30
        phy.profile_inputs.timing_resync_period.value = 4

        phy.profile_outputs.AGC_CTRL7_SUBDEN.override = 7
        phy.profile_outputs.AGC_CTRL7_SUBINT.override = 4
        phy.profile_outputs.AGC_CTRL7_SUBNUM.override = 2
        phy.profile_outputs.AGC_CTRL7_SUBPERIOD.override = 1
        phy.profile_outputs.AGC_GAINSTEPLIM0_CFLOOPSTEPMAX.override = 4
        # Not present in Series 2
        # phy.profile_outputs.AGC_LOOPDEL_LNASLICESDEL.override = 4
        phy.profile_outputs.MODEM_CTRL2_DATAFILTER.override = 0
        phy.profile_outputs.MODEM_TIMING_FASTRESYNC.override = 1
        phy.profile_outputs.MODEM_CTRL4_PREDISTDEB.override = 1
        phy.profile_outputs.MODEM_CTRL4_PREDISTGAIN.override = 3

        return phy

    def PHY_IEEE802154_915MHz_BPSK_40kbps(self, model, phy_name=None):
        phy = self.PHY_IEEE802154_868MHz_BPSK(model, 'PHY_IEEE802154_915MHz_BPSK_40kbps')

        phy.profile_inputs.base_frequency_hz.value = 915_000_000
        phy.profile_inputs.bitrate.value = 40000

        return phy

    def PHY_IEEE802154_868MHz_BPSK_coh(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='Coherent IEEE 802.15.4 868MHz BPSK PHY for Dumbo', phy_name=phy_name)

        self.IEEE802154_Base(phy, model)
        phy.profile_inputs.demod_select.value = model.vars.demod_select.var_enum.COHERENT
        phy.profile_inputs.agc_period.value = 3
        phy.profile_inputs.agc_power_target.value = -17
        phy.profile_inputs.base_frequency_hz.value =  long(868000000)
        phy.profile_inputs.baudrate_tol_ppm.value = 0
        phy.profile_inputs.bitrate.value = 20000
        phy.profile_inputs.channel_spacing_hz.value = 0
        phy.profile_inputs.deviation.value = 150000
        phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.RE0
        phy.profile_inputs.dsss_chipping_code.value =  long(0x9AF)
        phy.profile_inputs.dsss_len.value = 15
        phy.profile_inputs.dsss_spreading_factor.value = 15
        phy.profile_inputs.if_frequency_hz.value = 600000
        phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.DISABLED
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.BPSK
        phy.profile_inputs.number_of_timing_windows.value = 11
        phy.profile_inputs.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_2000KHz
        phy.profile_inputs.preamble_length.value = 32
        phy.profile_inputs.rssi_period.value = 7
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Raised_Cosine
        phy.profile_inputs.shaping_filter_param.value = 1.0
        phy.profile_inputs.timing_detection_threshold.value = 28
        phy.profile_inputs.agc_power_target.value = -27
        phy.profile_inputs.bandwidth_hz.value = 525000

        phy.profile_outputs.MODEM_COH3_COHDSAEN.override = 0

        phy.profile_outputs.AGC_CTRL7_SUBDEN.override = 4
        phy.profile_outputs.AGC_CTRL7_SUBINT.override = 3
        phy.profile_outputs.AGC_CTRL7_SUBNUM.override = 3
        phy.profile_outputs.AGC_CTRL7_SUBPERIOD.override = 1
        # Not present in Series 2
        # phy.profile_outputs.AGC_CTRL2_ADCRSTSTARTUP.override = 0
        # phy.profile_outputs.AGC_CTRL2_FASTLOOPDEL.override = 5
        # phy.profile_outputs.AGC_GAINSTEPLIM0_SLOWDECAYCNT.override = 10
        phy.profile_outputs.AGC_GAINSTEPLIM0_CFLOOPSTEPMAX.override = 5
        # Not present in Series 2
        # phy.profile_outputs.AGC_LOOPDEL_IFPGADEL.override = 7
        # phy.profile_outputs.AGC_LOOPDEL_LNASLICESDEL.override = 7
        # phy.profile_outputs.AGC_LOOPDEL_PKDWAIT.override = 15

        phy.profile_outputs.AGC_RSSISTEPTHR_POSSTEPTHR.override = 3
        phy.profile_outputs.AGC_RSSISTEPTHR_DEMODRESTARTPER.override = 6

        phy.profile_outputs.MODEM_AFCADJLIM_AFCADJLIM.override = 2000
        phy.profile_outputs.MODEM_CTRL0_DUALCORROPTDIS.override = 1
        phy.profile_outputs.MODEM_CTRL1_PHASEDEMOD.override = 3
        phy.profile_outputs.MODEM_CTRL1_RESYNCPER.override = 4
        phy.profile_outputs.MODEM_CTRL3_TSAMPDEL.override = 2
        # Not present in Series 2
        # phy.profile_outputs.MODEM_CTRL5_BBSS.override = 4
        # phy.profile_outputs.MODEM_CTRL5_DSSSCTD.override = 1
        # phy.profile_outputs.MODEM_CTRL5_FOEPREAVG.override = 7
        # phy.profile_outputs.MODEM_CTRL5_LINCORR.override = 1
        # phy.profile_outputs.MODEM_CTRL5_POEPER.override = 4
        phy.profile_outputs.MODEM_CTRL5_TDEDGE.override = 0
        phy.profile_outputs.MODEM_CTRL5_TREDGE.override = 0
        # Not present in Series 2
        # phy.profile_outputs.MODEM_CTRL6_CPLXCORREN.override = 1
        # phy.profile_outputs.MODEM_CTRL6_PREBASES.override = 8
        # phy.profile_outputs.MODEM_CTRL6_PSTIMABORT0.override = 1
        # phy.profile_outputs.MODEM_CTRL6_PSTIMABORT1.override = 1
        # phy.profile_outputs.MODEM_CTRL6_PSTIMABORT2.override = 1
        phy.profile_outputs.MODEM_CTRL6_RXBRCALCDIS.override = 1
        # Not present in Series 2
        # phy.profile_outputs.MODEM_CTRL6_TDREW.override = 60
        # phy.profile_outputs.MODEM_CTRL6_TIMTHRESHGAIN.override = 1
        phy.profile_outputs.MODEM_MODINDEX_MODINDEXE.override = 29
        phy.profile_outputs.MODEM_MODINDEX_MODINDEXM.override = 15
        phy.profile_outputs.MODEM_PRE_PREERRORS.override = 15
        phy.profile_outputs.MODEM_TIMING_ADDTIMSEQ.override = 8
        phy.profile_outputs.MODEM_TIMING_FASTRESYNC.override = 1
        phy.profile_outputs.MODEM_TIMING_TIMINGBASES.override = 8
        phy.profile_outputs.MODEM_TIMING_TIMTHRESH.override = 50
        phy.profile_outputs.MODEM_TXBR_TXBRDEN.override = 1
        phy.profile_outputs.MODEM_TXBR_TXBRNUM.override = 16
        # Not present in Series 2
        # phy.profile_outputs.RAC_IFFILTCTRL_BANDWIDTH.override = 3

        # values mismatching from PHY in RTL
        # Not present in Series 2
        # phy.profile_outputs.MODEM_INTAFC_FOEPREAVG1.override = 1
        # phy.profile_outputs.MODEM_INTAFC_FOEPREAVG2.override = 3
        # phy.profile_outputs.MODEM_INTAFC_FOEPREAVG3.override = 4
        #phy.profile_outputs.MODEM_CF_DEC1.override = 3
        #phy.profile_outputs.MODEM_CF_DEC0.override = 2
        #phy.profile_outputs.MODEM_CF_CFOSR.override = 3
        # MEAWAN phy.profile_outputs.MODEM_SRCCHF_BWSEL.override = 1

        return phy

    def PHY_IEEE802154g_MRFSK_OM1_16bitpre_oneshot_dsafoest(self, model, phy_name=None):
        pass

    def PHY_IEEE802154g_MRFSK_OM1_16bitpre_oneshot_legacyfoest(self, model, phy_name=None):
        pass

    def PHY_IEEE802154g_MRFSK_OM1_32bitpre_oneshot_dsafoest(self, model, phy_name=None):
        pass

    def PHY_IEEE802154g_MRFSK_OM1_32bitpre_oneshot_legacyfoest(self, model, phy_name=None):
        pass

    def PHY_IEEE802154g_MRFSK_OM1_16bitpre(self, model, phy_name=None):
        pass

    def PHY_IEEE802154_RSGFSK_868MHz_500kbps_mi0p76(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='IEEE 802.15.4 868MHz RS-GFSK PHY', phy_name=phy_name)

        phy.profile_inputs.base_frequency_hz.value =  long(868000000)
        phy.profile_inputs.baudrate_tol_ppm.value = 0
        phy.profile_inputs.bitrate.value = 500000
        phy.profile_inputs.channel_spacing_hz.value = 1000000
        phy.profile_inputs.deviation.value = 190000   # 0.76*500000/2
        phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED
        phy.profile_inputs.dsss_chipping_code.value = long(0)
        phy.profile_inputs.dsss_len.value = 0
        phy.profile_inputs.dsss_spreading_factor.value = 0
        phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP0
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.FSK2
        phy.profile_inputs.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_2500KHz
        phy.profile_inputs.preamble_length.value = 4*8   # phyRsGfskPreambleLength multiples of '01010101'
        phy.profile_inputs.preamble_pattern.value = 1
        phy.profile_inputs.preamble_pattern_len.value = 2
        phy.profile_inputs.rx_xtal_error_ppm.value = 25
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Gaussian
        phy.profile_inputs.shaping_filter_param.value = 0.5
        phy.profile_inputs.symbol_encoding.value = model.vars.symbol_encoding.var_enum.NRZ
        phy.profile_inputs.syncword_0.value =  long(0x904E)
        phy.profile_inputs.syncword_1.value =  long(0x9af0)
        phy.profile_inputs.syncword_length.value = 16
        phy.profile_inputs.tx_xtal_error_ppm.value = 25
        phy.profile_inputs.xtal_frequency_hz.value = 38400000
        # FIXME:  temporary force bandwidth to (0.76+1)*500000*0.75 + 2*(25+25)*868
        #phy.profile_inputs.bandwidth_hz.value = 650000
        # FIXME:  temporary force KSI until calculated in py
        phy.profile_outputs.MODEM_VITERBIDEMOD_VITERBIKSI1.override = 97
        phy.profile_outputs.MODEM_VITERBIDEMOD_VITERBIKSI2.override = 65 #72 for forced bw=650000
        phy.profile_outputs.MODEM_VITERBIDEMOD_VITERBIKSI3.override = 49 #50 for forced bw=650000

        return phy

    def IEEE802154_2p4GHz_cohdsa_base(self, phy, model):
        PHY_COMMON_FRAME_154(phy, model)
        # Override min length for 802.15.4E Seq# Suppression
        phy.profile_inputs.var_length_minlength.value = 4
        Phy_Internal_Base.AGC_FAST_LOOP_base(phy, model)
        phy.profile_inputs.demod_select.value = model.vars.demod_select.var_enum.COHERENT
        phy.profile_inputs.bandwidth_hz.value = 2524800
        phy.profile_inputs.base_frequency_hz.value = long(2450000000)
        phy.profile_inputs.baudrate_tol_ppm.value = 0
        phy.profile_inputs.bitrate.value = 250000
        phy.profile_inputs.channel_spacing_hz.value = 5000000
        phy.profile_inputs.deviation.value = 500000
        phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED
        phy.profile_inputs.dsss_chipping_code.value = long(0x744AC39B)
        phy.profile_inputs.dsss_len.value = 32
        phy.profile_inputs.dsss_spreading_factor.value = 8
        phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.DISABLED
        phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP0
        phy.profile_inputs.if_frequency_hz.value = 1370000
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.OQPSK
        phy.profile_inputs.number_of_timing_windows.value = 7
        phy.profile_inputs.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_1500KHz
        phy.profile_inputs.pll_bandwidth_rx.value = model.vars.pll_bandwidth_rx.var_enum.BW_250KHz
        phy.profile_inputs.preamble_length.value = 32
        phy.profile_inputs.preamble_pattern.value = 0
        phy.profile_inputs.preamble_pattern_len.value = 4
        phy.profile_inputs.rssi_period.value = 3
        phy.profile_inputs.rx_xtal_error_ppm.value = 0
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Custom_OQPSK
        phy.profile_inputs.shaping_filter_param.value = 0.0
        phy.profile_inputs.symbol_encoding.value = model.vars.symbol_encoding.var_enum.DSSS
        phy.profile_inputs.symbols_in_timing_window.value = 12
        phy.profile_inputs.syncword_0.value = long(0xe5)
        phy.profile_inputs.syncword_1.value = long(0x0)
        phy.profile_inputs.syncword_length.value = 8
        phy.profile_inputs.target_osr.value = 5
        phy.profile_inputs.timing_detection_threshold.value = 65
        phy.profile_inputs.timing_sample_threshold.value = 0
        phy.profile_inputs.tx_xtal_error_ppm.value = 0
        phy.profile_inputs.xtal_frequency_hz.value = 38400000
        phy.profile_inputs.target_osr.value = 5  # Calc SRC

        # Additional overrides introduced when Series 2 AGC calculations added. These prevent the PHY from changing versus what was used during Validation.

        phy.profile_outputs.AGC_LNABOOST_LNABWADJ.override = 0
        phy.profile_outputs.AGC_CTRL0_PWRTARGET.override = 20
        phy.profile_outputs.AGC_CTRL1_PWRPERIOD.override = 4
        phy.profile_outputs.FRC_AUTOCG_AUTOCGEN.override = 7
        phy.profile_outputs.MODEM_AFC_AFCRXCLR.override = 1
        #phy.profile_outputs.MODEM_AFC_AFCSCALEM.override = 3
        phy.profile_outputs.MODEM_AFCADJLIM_AFCADJLIM.override = 2750
        phy.profile_outputs.MODEM_CGCLKSTOP_FORCEOFF.override = 0x1E00  # 9,10,11,12
        phy.profile_outputs.MODEM_COH0_COHCHPWRTH0.override = 33
        phy.profile_outputs.MODEM_COH0_COHCHPWRTH1.override = 40
        phy.profile_outputs.MODEM_COH0_COHCHPWRTH2.override = 100
        phy.profile_outputs.MODEM_COH0_COHDYNAMICBBSSEN.override = 1
        phy.profile_outputs.MODEM_COH0_COHDYNAMICPRETHRESH.override = 1
        phy.profile_outputs.MODEM_COH0_COHDYNAMICSYNCTHRESH.override = 1
        phy.profile_outputs.MODEM_COH1_SYNCTHRESH0.override = 20
        phy.profile_outputs.MODEM_COH1_SYNCTHRESH1.override = 23
        phy.profile_outputs.MODEM_COH1_SYNCTHRESH2.override = 26
        phy.profile_outputs.MODEM_COH2_DSAPEAKCHPWRTH.override = 200
        phy.profile_outputs.MODEM_COH2_FIXEDCDTHFORIIR.override = 105
        phy.profile_outputs.MODEM_COH2_SYNCTHRESHDELTA1.override = 2
        phy.profile_outputs.MODEM_COH2_SYNCTHRESHDELTA2.override = 4
        phy.profile_outputs.MODEM_COH3_CDSS.override = 4
        phy.profile_outputs.MODEM_COH3_COHDSAADDWNDSIZE.override = 80
        phy.profile_outputs.MODEM_COH3_COHDSAEN.override = 1
        phy.profile_outputs.MODEM_COH3_DSAPEAKINDLEN.override = 4
        phy.profile_outputs.MODEM_COH3_DYNIIRCOEFOPTION.override = 3
        phy.profile_outputs.MODEM_COH3_LOGICBASEDCOHDEMODGATE.override = 1
        phy.profile_outputs.MODEM_COH3_PEAKCHKTIMOUT.override = 18
        phy.profile_outputs.MODEM_CTRL1_PHASEDEMOD.override = 2
        phy.profile_outputs.MODEM_CTRL2_DATAFILTER.override = 7
        phy.profile_outputs.MODEM_CTRL2_SQITHRESH.override = 200
        phy.profile_outputs.MODEM_CTRL5_BBSS.override = 6
        phy.profile_outputs.MODEM_CTRL5_DSSSCTD.override = 1
        phy.profile_outputs.MODEM_CTRL5_FOEPREAVG.override = 7
        phy.profile_outputs.MODEM_CTRL5_LINCORR.override = 1
        phy.profile_outputs.MODEM_CTRL5_POEPER.override = 1
        phy.profile_outputs.MODEM_CTRL6_ARW.override = 1
        phy.profile_outputs.MODEM_CTRL6_DSSS3SYMBOLSYNCEN.override = 1
        phy.profile_outputs.MODEM_CTRL6_PSTIMABORT0.override = 1
        phy.profile_outputs.MODEM_CTRL6_PSTIMABORT1.override = 1
        phy.profile_outputs.MODEM_CTRL6_PSTIMABORT2.override = 1
        phy.profile_outputs.MODEM_CTRL6_RXBRCALCDIS.override = 1
        phy.profile_outputs.MODEM_CTRL6_TDREW.override = 64
        phy.profile_outputs.MODEM_CTRL6_TIMTHRESHGAIN.override = 2
        phy.profile_outputs.MODEM_DIGIGAINCTRL_DIGIGAINEN.override = 1
        phy.profile_outputs.MODEM_DIGIGAINCTRL_DIGIGAINSEL.override = 20
        # phy.profile_outputs.MODEM_DIGMIXCTRL_DIGMIXFREQ.override = 150020 # Calc SRC
        phy.profile_outputs.MODEM_DSACTRL_ARRTHD.override = 4  # Was missed
        phy.profile_outputs.MODEM_INTAFC_FOEPREAVG0.override = 1
        phy.profile_outputs.MODEM_INTAFC_FOEPREAVG1.override = 3
        phy.profile_outputs.MODEM_INTAFC_FOEPREAVG2.override = 5
        phy.profile_outputs.MODEM_INTAFC_FOEPREAVG3.override = 5
        phy.profile_outputs.MODEM_LONGRANGE1_AVGWIN.override = 4
        phy.profile_outputs.MODEM_LONGRANGE1_HYSVAL.override = 3
        phy.profile_outputs.MODEM_LONGRANGE1_PREFILTLEN.override = 3
        phy.profile_outputs.MODEM_LONGRANGE1_LRTIMEOUTTHD.override = 320
        phy.profile_outputs.MODEM_LONGRANGE1_PREFILTLEN.override = 3
        phy.profile_outputs.MODEM_LONGRANGE2_LRCHPWRTH1.override = 20
        phy.profile_outputs.MODEM_LONGRANGE2_LRCHPWRTH2.override = 26
        phy.profile_outputs.MODEM_LONGRANGE2_LRCHPWRTH3.override = 33
        phy.profile_outputs.MODEM_LONGRANGE2_LRCHPWRTH4.override = 40
        phy.profile_outputs.MODEM_LONGRANGE3_LRCHPWRTH5.override = 46
        phy.profile_outputs.MODEM_LONGRANGE3_LRCHPWRTH6.override = 52
        phy.profile_outputs.MODEM_LONGRANGE3_LRCHPWRTH7.override = 59
        phy.profile_outputs.MODEM_LONGRANGE3_LRCHPWRTH8.override = 66
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRSH1.override = 3
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRSH2.override = 4
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRSH3.override = 5
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRSH4.override = 5
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRTH10.override = 80
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRTH9.override = 73
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH10.override = 11
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH11.override = 12
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH5.override = 6
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH6.override = 7
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH7.override = 8
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH8.override = 9
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH9.override = 10
        phy.profile_outputs.MODEM_LONGRANGE6_LRCHPWRSPIKETH.override = 40
        phy.profile_outputs.MODEM_LONGRANGE6_LRSPIKETHD.override = 105

        # phy.profile_outputs.MODEM_SRCCHF_SRCRATIO2.override = 15689 # Calc SRC
        phy.profile_outputs.MODEM_TIMING_TIMTHRESH.override = 35

        #phy.profile_outputs.RAC_PGACTRL_LNAMIXRFPKDTHRESHSEL.override = 2
        phy.profile_outputs.RAC_PGACTRL_PGATHRPKDHISEL.override = 5
        phy.profile_outputs.RAC_PGACTRL_PGATHRPKDLOSEL.override = 1
        #phy.profile_outputs.RAC_SYNTHCTRL_MMDPOWERBALANCEDISABLE.override = 1
        phy.profile_outputs.RAC_SYNTHREGCTRL_MMDLDOVREFTRIM.override = 3
        phy.profile_outputs.RAC_PGACTRL_PGAENLATCHI.override = 1
        phy.profile_outputs.RAC_PGACTRL_PGAENLATCHQ.override = 1
        phy.profile_outputs.SYNTH_LPFCTRL1CAL_OP1BWCAL.override = 11
        phy.profile_outputs.SYNTH_LPFCTRL1CAL_OP1COMPCAL.override = 14
        phy.profile_outputs.SYNTH_LPFCTRL1CAL_RFBVALCAL.override = 0
        phy.profile_outputs.SYNTH_LPFCTRL1CAL_RPVALCAL.override = 0
        phy.profile_outputs.SYNTH_LPFCTRL1CAL_RZVALCAL.override = 9
        # Derived empirically
        # https://confluence.silabs.com/display/RAIL/Panther+Weaksymbols+WifiBlocker+Characterization
        phy.profile_outputs.MODEM_CTRL2_SQITHRESH.override = 56

    def PHY_IEEE802154_2p4GHz_cohdsa(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='802154 2p4GHz cohdsa', phy_name=phy_name)

        self.IEEE802154_2p4GHz_cohdsa_base(phy, model)
        phy.profile_outputs.MODEM_TXBR_TXBRDEN.override = 105
        phy.profile_outputs.MODEM_TXBR_TXBRNUM.override = 252

        return phy

    def PHY_IEEE802154_2p4GHz_cohdsa_diversity(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='802154 2p4GHz cohdsa', phy_name=phy_name)

        self.IEEE802154_2p4GHz_cohdsa_base(phy, model)
        phy.profile_outputs.MODEM_TXBR_TXBRDEN.override = 105
        phy.profile_outputs.MODEM_TXBR_TXBRNUM.override = 252

        return phy

    def PHY_IEEE802154_2p4GHz(self, model,phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='Legacy IEEE 802.15.4 2p4GHz PHY from Jumbo',phy_name=phy_name)
        self.IEEE802154_2p4GHz_base(phy, model)

        phy.profile_outputs.AGC_CTRL2_DISRFPKD.override = 1
        phy.profile_outputs.AGC_CTRL4_RFPKDCNTEN.override = 0
        phy.profile_inputs.demod_select.value = model.vars.demod_select.var_enum.LEGACY

        return phy

    def PHY_IEEE802154_2p4GHz_diversity(self, model,phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='Legacy IEEE 802.15.4 2p4GHz PHY from Jumbo',phy_name=phy_name)
        self.IEEE802154_2p4GHz_base(phy, model)

        phy.profile_outputs.AGC_CTRL2_DISRFPKD.override = 1
        phy.profile_outputs.AGC_CTRL4_RFPKDCNTEN.override = 0
        phy.profile_inputs.demod_select.value = model.vars.demod_select.var_enum.LEGACY

        return phy

    def IEEE802154_Internal_915MHz_cohdsa_base(self, phy, model):
        """ Frequency Planning """
        phy.profile_inputs.base_frequency_hz.value = long(915e6)
        phy.profile_inputs.channel_spacing_hz.value = 5000000
        phy.profile_inputs.bitrate.value = 250000
        phy.profile_inputs.deviation.value = 500000
        phy.profile_inputs.if_frequency_hz.value = 1370000

        phy.profile_inputs.bandwidth_hz.value = 2524800
        phy.profile_inputs.baudrate_tol_ppm.value = 0

        PHY_COMMON_FRAME_154(phy, model)
        # Override min length for 802.15.4E Seq# Suppression
        phy.profile_inputs.var_length_minlength.value = 4
        # Phy_Internal_Base.AGC_FAST_LOOP_base(phy, model)

        """ Modulation """
        phy.profile_inputs.demod_select.value = model.vars.demod_select.var_enum.COHERENT
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.OQPSK

        """ Shaping filter """
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Custom_OQPSK
        phy.profile_inputs.shaping_filter_param.value = 0.0

        """ Symbol mapping and encoding """
        phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP0
        phy.profile_inputs.symbol_encoding.value = model.vars.symbol_encoding.var_enum.DSSS
        phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED

        """ DSSS Configurations """
        phy.profile_inputs.dsss_chipping_code.value = long(0x744AC39B)
        phy.profile_inputs.dsss_len.value = 32
        phy.profile_inputs.dsss_spreading_factor.value = 8

        """ Preamble """
        phy.profile_inputs.preamble_length.value = 32
        phy.profile_inputs.preamble_pattern.value = 0
        phy.profile_inputs.preamble_pattern_len.value = 4

        """ Timing """
        phy.profile_inputs.symbols_in_timing_window.value = 12
        phy.profile_inputs.timing_detection_threshold.value = 65
        phy.profile_inputs.timing_sample_threshold.value = 0
        phy.profile_inputs.number_of_timing_windows.value = 7

        """ Sync """
        phy.profile_inputs.syncword_0.value = long(0xe5)
        phy.profile_inputs.syncword_1.value = long(0x0)
        phy.profile_inputs.syncword_length.value = 8

        """ XO Configuration """
        phy.profile_inputs.xtal_frequency_hz.value = 39000000
        phy.profile_inputs.rx_xtal_error_ppm.value = 0
        phy.profile_inputs.tx_xtal_error_ppm.value = 0

        """ Dynamic BBSS """
        phy.profile_outputs.MODEM_LONGRANGE1_AVGWIN.override = 4

        phy.profile_outputs.MODEM_LONGRANGE2_LRCHPWRTH1.override = 20
        phy.profile_outputs.MODEM_LONGRANGE2_LRCHPWRTH2.override = 26
        phy.profile_outputs.MODEM_LONGRANGE2_LRCHPWRTH3.override = 33
        phy.profile_outputs.MODEM_LONGRANGE2_LRCHPWRTH4.override = 40
        phy.profile_outputs.MODEM_LONGRANGE3_LRCHPWRTH5.override = 46
        phy.profile_outputs.MODEM_LONGRANGE3_LRCHPWRTH6.override = 52
        phy.profile_outputs.MODEM_LONGRANGE3_LRCHPWRTH7.override = 59
        phy.profile_outputs.MODEM_LONGRANGE3_LRCHPWRTH8.override = 66
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRTH9.override = 73
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRTH10.override = 80

        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRSH1.override = 3
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRSH2.override = 4
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRSH3.override = 5
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRSH4.override = 5
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH5.override = 6
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH6.override = 7
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH7.override = 8
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH8.override = 9
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH9.override = 10
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH10.override = 11
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH11.override = 12

        """ Pre-filter """
        phy.profile_outputs.MODEM_LONGRANGE1_PREFILTLEN.override = 3

        """ Coherent DSA """
        phy.profile_outputs.MODEM_LONGRANGE1_LRTIMEOUTTHD.override = 320
        phy.profile_outputs.MODEM_COH2_DSAPEAKCHPWRTH.override = 200

        phy.profile_outputs.MODEM_LONGRANGE6_LRCHPWRSPIKETH.override = 40
        phy.profile_outputs.MODEM_LONGRANGE6_LRSPIKETHD.override = 105
        phy.profile_outputs.MODEM_COH2_FIXEDCDTHFORIIR.override = 105

        """ Dynamic Pre/Sync Threshold """
        phy.profile_outputs.MODEM_COH0_COHCHPWRTH0.override = 33
        phy.profile_outputs.MODEM_COH0_COHCHPWRTH1.override = 40
        phy.profile_outputs.MODEM_COH0_COHCHPWRTH2.override = 100

        phy.profile_outputs.MODEM_COH1_SYNCTHRESH0.override = 20
        phy.profile_outputs.MODEM_COH1_SYNCTHRESH1.override = 23
        phy.profile_outputs.MODEM_COH1_SYNCTHRESH2.override = 26

        phy.profile_outputs.MODEM_COH2_SYNCTHRESHDELTA1.override = 2
        phy.profile_outputs.MODEM_COH2_SYNCTHRESHDELTA2.override = 4

        """  """
        phy.profile_outputs.MODEM_CTRL5_RESYNCBAUDTRANS.override = 0

        """ For Ocelot, need to force adc clock mode to be same as bobcat """
        phy.profile_inputs.adc_clock_mode.value = model.vars.adc_clock_mode.var_enum.VCODIV

        # Derived empirically to improve wifi blocking performance
        # https://confluence.silabs.com/display/RAIL/Panther+Weaksymbols+WifiBlocker+Characterization
        phy.profile_outputs.MODEM_CTRL2_SQITHRESH.override = 56

    # Owner : Young-Joon Choi
    # Internal IEEE802154 PHY to check Bobcat PHY in Ocelot Silicon
    def PHY_Internal_IEEE802154_915MHz_cohdsa(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='802154 915 MHz cohdsa', phy_name=phy_name)
        self.IEEE802154_Internal_915MHz_cohdsa_base(phy, model)
        return phy

    # Owner : Young-Joon Choi
    # Internal IEEE802154 PHY to check Bobcat PHY Antenna Diversity in Ocelot Silicon
    def PHY_Internal_IEEE802154_915MHz_cohdsa_antdiv(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='802154 915 MHz cohdsa AntDiv', phy_name=phy_name)
        self.IEEE802154_Internal_915MHz_cohdsa_base(phy, model)

        """ Antenna Diversity Settings """
        phy.profile_inputs.antdivmode.value = model.vars.antdivmode.var_enum.ANTSELCORR
        phy.profile_inputs.antdivrepeatdis.value = model.vars.antdivrepeatdis.var_enum.NOREPEATFIRST
        return phy