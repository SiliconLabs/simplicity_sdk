from pyradioconfig.parts.common.profiles.ocelot_regs import build_modem_regs_ocelot
from pyradioconfig.parts.lynx.profiles.Profile_IEEE802154_OQPSK import ProfileIEEE802154OQPSKLynx
from pyradioconfig.parts.ocelot.profiles.sw_profile_outputs_common import sw_profile_outputs_common_ocelot
from pycalcmodel.core.output import ModelOutput, ModelOutputType
from pyradioconfig.parts.common.profiles.profile_common import buildCrcOutputs, buildFecOutputs, buildFrameOutputs, \
    buildWhiteOutputs


class ProfileIEEE802154OQPSKOcelot(ProfileIEEE802154OQPSKLynx):

    def __init__(self):
        super().__init__()
        self._profileName = "IEEE802154OQPSK"
        self._readable_name = "IEEE802154 OQPSK Profile"
        self._category = ""
        self._description = "Profile used for IEEE802154 OQPSK phys"
        self._default = False
        self._activation_logic = ""
        self._family = "ocelot"
        self._sw_profile_outputs_common = sw_profile_outputs_common_ocelot()

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

        self._sw_profile_outputs_common.buildStudioLogOutput(model, profile)

    def build_register_profile_outputs(self, model, profile):
        build_modem_regs_ocelot(model, profile)

    def build_variable_profile_outputs(self, model, profile):
        # RAIL Outputs
        self._sw_profile_outputs_common.build_rail_outputs(model, profile)

        # IRCal outputs
        self._sw_profile_outputs_common.build_ircal_outputs(model, profile)

    def build_info_profile_outputs(self, model, profile):
        # Informational output
        self._sw_profile_outputs_common.build_info_outputs(model, profile)
        profile.outputs.append(ModelOutput(model.vars.agc_scheme, '', ModelOutputType.INFO, readable_name="AGC backoff scheme"))
        profile.outputs.append(ModelOutput(model.vars.bandwidth_hz, '', ModelOutputType.INFO, readable_name="Acquisition Channel Bandwidth"))
        profile.outputs.append(ModelOutput(model.vars.etsi_cat1_compatible, '', ModelOutputType.INFO, readable_name="ETSI Category 1 Compatibility"))
        profile.outputs.append(ModelOutput(model.vars.firstframe_bitsperword, '', ModelOutputType.INFO, readable_name="Length of the First Word"))
        profile.outputs.append(ModelOutput(model.vars.if_frequency_hz, '', ModelOutputType.INFO, readable_name="IF Frequency"))
        profile.outputs.append(ModelOutput(model.vars.lo_injection_side, '', ModelOutputType.INFO, readable_name="Injection side"))
        profile.outputs.append(ModelOutput(model.vars.pll_bandwidth_rx, '', ModelOutputType.INFO, readable_name="PLL Bandwidth in RX mode"))
        profile.outputs.append(ModelOutput(model.vars.pll_bandwidth_tx, '', ModelOutputType.INFO, readable_name="PLL Bandwidth in TX mode"))
        profile.outputs.append(ModelOutput(model.vars.src_disable, '', ModelOutputType.INFO, readable_name="SRC Operation"))
        profile.outputs.append(ModelOutput(model.vars.var_length_loc, '', ModelOutputType.INFO, readable_name="Byte position of dynamic length byte"))
        profile.outputs.append(ModelOutput(model.vars.fast_detect_enable, '', ModelOutputType.INFO, readable_name="Fast preamble detect"))
        profile.outputs.append(ModelOutput(model.vars.fec_tx_enable, '', ModelOutputType.INFO, readable_name="Enable FEC"))
        profile.outputs.append(ModelOutput(model.vars.preamble_detection_length, '', ModelOutputType.INFO, readable_name="Preamble Detection Length"))
        profile.outputs.append(ModelOutput(model.vars.skip2ant, '', ModelOutputType.INFO, readable_name="Skip 2nd antenna check with phase demod antenna diversity"))

    def _build_frame_settings(self, model):
        super()._build_frame_settings(model)
        model.vars.asynchronous_rx_enable.value_forced = None
        model.vars.frame_coding.value_forced = None

    def _build_modem_settings(self, model):
        self._build_agc_fastloop_settings(model)

        if model.vars.demod_select.value == model.vars.demod_select.var_enum.COHERENT:
            self._build_cohdsa_settings(model)
        else:
            self._build_legacy_settings(model)

        if model.profile.inputs.zigbee_feature.var_value == model.vars.zigbee_feature.var_enum.FCS:
            self._build_fast_switch_settings(model)

        self._build_tolerance_setting(model)

        model.vars.manchester_mapping.value_forced = None
        model.vars.directmode_rx.value_forced = model.vars.directmode_rx.var_enum.DISABLED
        model.vars.frequency_comp_mode.value_forced = None
        model.vars.pll_bandwidth_tx.value_forced = None
        model.vars.pll_bandwidth_rx.value_forced = None

    def _build_tolerance_setting(self, model):
        base_freq_Hz = model.profile.inputs.base_frequency_hz.var_value

        model.vars.deviation_tol_ppm.value_forced = 0
        if 2360e6 <= base_freq_Hz <= 2400e6 or 2400e6 <= base_freq_Hz <= 2483.5e6:
            model.vars.bandwidth_hz.value_forced = 2524800
            if model.vars.demod_select.value == model.vars.demod_select.var_enum.COHERENT:
                model.vars.baudrate_tol_ppm.value_forced = 0
            else:
                model.vars.baudrate_tol_ppm.value_forced = 4000
            model.vars.tx_xtal_error_ppm.value_forced = 0
            model.vars.rx_xtal_error_ppm.value_forced = 0
        elif 902e6 <= base_freq_Hz <= 928e6:
            model.vars.bandwidth_hz.value_forced = 1262400
            model.vars.baudrate_tol_ppm.value_forced = 8000
            model.vars.tx_xtal_error_ppm.value_forced = 25
            model.vars.rx_xtal_error_ppm.value_forced = 25
        elif 868e6 <= base_freq_Hz <= 868.6e6:
            model.vars.bandwidth_hz.value_forced = 504960
            model.vars.baudrate_tol_ppm.value_forced = 8000
            model.vars.tx_xtal_error_ppm.value_forced = 25
            model.vars.rx_xtal_error_ppm.value_forced = 25
        elif 779e6 <= base_freq_Hz <= 787e6:
            model.vars.bandwidth_hz.value_forced = 1262400
            model.vars.baudrate_tol_ppm.value_forced = 8000
            model.vars.tx_xtal_error_ppm.value_forced = 0
            model.vars.rx_xtal_error_ppm.value_forced = 0

    def _build_legacy_settings(self, model):
        base_freq_Hz = model.profile.inputs.base_frequency_hz.var_value

        model.vars.frequency_comp_mode.value_forced = model.vars.frequency_comp_mode.var_enum.INTERNAL_ALWAYS_ON
        model.vars.number_of_timing_windows.value_forced = 3

        if 2360e6 <= base_freq_Hz <= 2400e6 or 2400e6 <= base_freq_Hz <= 2483.5e6:
            super()._build_legacy_settings(model)
            model.vars.number_of_timing_windows.value_forced = None
            model.vars.RAC_SYNTHCTRL_MMDPOWERBALANCEDISABLE.value_forced = None
        elif 902e6 <= base_freq_Hz <= 928e6:
            model.vars.number_of_timing_windows.value_forced = 3
        elif 868e6 <= base_freq_Hz <= 868.6e6:
            model.vars.timing_detection_threshold.value_forced = 35
        elif 779e6 <= base_freq_Hz <= 787e6:
            model.vars.timing_sample_threshold.value_forced = 0
            model.vars.timing_detection_threshold.value_forced = 35
            model.vars.timing_resync_period.value_forced = None

            model.vars.MODEM_CGCLKSTOP_FORCEOFF.value_forced = None
            model.vars.FRC_AUTOCG_AUTOCGEN.value_forced = 7

            model.vars.rssi_period.value_forced = 7
            model.vars.agc_hysteresis.value_forced = 0
            model.vars.agc_power_target.value_forced = -15
            model.vars.AGC_GAINSTEPLIM0_CFLOOPSTEPMAX.value_forced = 4

    def _build_cohdsa_settings(self, model):
        base_freq_Hz = model.profile.inputs.base_frequency_hz.var_value

        if 2360e6 <= base_freq_Hz <= 2400e6 or 2400e6 <= base_freq_Hz <= 2483.5e6:
            super()._build_cohdsa_settings(model)

            # : following register fields are deprecated
            model.vars.AGC_GAINRANGE_BOOSTLNA.value_forced = None
            model.vars.AGC_GAINRANGE_LNABWADJ.value_forced = None
            model.vars.MODEM_AFC_AFCSCALEM.value_forced = None
            model.vars.RAC_PGACTRL_LNAMIXRFPKDTHRESHSEL.value_forced = None
            model.vars.RAC_SYNTHCTRL_MMDPOWERBALANCEDISABLE.value_forced = None
            model.vars.RFCRC_CTRL_INPUTINV.value_forced = None

            model.vars.MODEM_TXBR_TXBRDEN.value_forced = 105
            model.vars.MODEM_TXBR_TXBRNUM.value_forced = 252
        elif 902e6 <= base_freq_Hz <= 928e6:
            model.vars.timing_detection_threshold.value_forced = 35
            model.vars.AGC_CTRL0_PWRTARGET.value_forced = 20
            model.vars.AGC_CTRL1_RSSIPERIOD.value_forced = 3
            model.vars.MODEM_AFCADJLIM_AFCADJLIM.value_forced = 658
            model.vars.MODEM_COH0_COHCHPWRTH1.value_forced = 19
            model.vars.MODEM_COH0_COHCHPWRTH2.value_forced = 79
            model.vars.MODEM_COH0_COHDYNAMICBBSSEN.value_forced = 1
            model.vars.MODEM_COH0_COHDYNAMICPRETHRESH.value_forced = 1
            model.vars.MODEM_COH0_COHDYNAMICSYNCTHRESH.value_forced = 0
            model.vars.MODEM_COH1_SYNCTHRESH0.value_forced = 31
            model.vars.MODEM_COH1_SYNCTHRESH1.value_forced = 21
            model.vars.MODEM_COH1_SYNCTHRESH2.value_forced = 21
            model.vars.MODEM_COH1_SYNCTHRESH3.value_forced = 46
            model.vars.MODEM_COH2_SYNCTHRESHDELTA2.value_forced = 5
            model.vars.MODEM_COH3_CDSS.value_forced = 4
            model.vars.MODEM_COH3_COHDSAADDWNDSIZE.value_forced = 864  # -160
            model.vars.MODEM_COH3_COHDSAEN.value_forced = 1
            model.vars.MODEM_COH3_DYNIIRCOEFOPTION.value_forced = 3
            model.vars.MODEM_CTRL1_COMPMODE.value_forced = 0
            model.vars.MODEM_CTRL1_PHASEDEMOD.value_forced = 2
            model.vars.MODEM_CTRL2_DATAFILTER.value_forced = 7
            model.vars.MODEM_CTRL5_LINCORR.value_forced = 0
            model.vars.MODEM_CTRL5_BBSS.value_forced = 5
            model.vars.MODEM_CTRL5_DSSSCTD.value_forced = 1
            model.vars.MODEM_CTRL5_FOEPREAVG.value_forced = 7
            model.vars.MODEM_CTRL5_POEPER.value_forced = 1
            model.vars.MODEM_CTRL6_ARW.value_forced = 1
            model.vars.MODEM_CTRL6_PSTIMABORT1.value_forced = 1
            model.vars.MODEM_CTRL6_RXBRCALCDIS.value_forced = 1
            model.vars.MODEM_CTRL6_TDREW.value_forced = 32
            model.vars.MODEM_CTRL6_TIMTHRESHGAIN.value_forced = 1
            model.vars.MODEM_CTRL6_PSTIMABORT0.value_forced = 0
            model.vars.MODEM_CTRL6_PSTIMABORT2.value_forced = 0
            model.vars.MODEM_INTAFC_FOEPREAVG0.value_forced = 1
            model.vars.MODEM_INTAFC_FOEPREAVG1.value_forced = 2
            model.vars.MODEM_INTAFC_FOEPREAVG2.value_forced = 4
            model.vars.MODEM_INTAFC_FOEPREAVG3.value_forced = 4
            model.vars.MODEM_LONGRANGE1_AVGWIN.value_forced = 4
            model.vars.MODEM_LONGRANGE1_CHPWRACCUDEL.value_forced = 1
            model.vars.MODEM_LONGRANGE1_HYSVAL.value_forced = 3
            model.vars.MODEM_LONGRANGE1_LRTIMEOUTTHD.value_forced = 160
            model.vars.MODEM_LONGRANGE2_LRCHPWRTH1.value_forced = 19
            model.vars.MODEM_LONGRANGE2_LRCHPWRTH2.value_forced = 25
            model.vars.MODEM_LONGRANGE2_LRCHPWRTH3.value_forced = 31
            model.vars.MODEM_LONGRANGE2_LRCHPWRTH4.value_forced = 37
            model.vars.MODEM_LONGRANGE3_LRCHPWRTH5.value_forced = 43
            model.vars.MODEM_LONGRANGE3_LRCHPWRTH6.value_forced = 49
            model.vars.MODEM_LONGRANGE3_LRCHPWRTH7.value_forced = 55
            model.vars.MODEM_LONGRANGE3_LRCHPWRTH8.value_forced = 61
            model.vars.MODEM_LONGRANGE4_LRCHPWRSH1.value_forced = 3
            model.vars.MODEM_LONGRANGE4_LRCHPWRSH2.value_forced = 4
            model.vars.MODEM_LONGRANGE4_LRCHPWRSH3.value_forced = 5
            model.vars.MODEM_LONGRANGE4_LRCHPWRSH4.value_forced = 6
            model.vars.MODEM_LONGRANGE4_LRCHPWRTH10.value_forced = 73
            model.vars.MODEM_LONGRANGE4_LRCHPWRTH9.value_forced = 67
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH10.value_forced = 12
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH11.value_forced = 13
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH5.value_forced = 7
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH6.value_forced = 8
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH7.value_forced = 9
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH8.value_forced = 10
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH9.value_forced = 11
            model.vars.MODEM_LONGRANGE6_LRCHPWRSPIKETH.value_forced = 127
            model.vars.MODEM_LONGRANGE6_LRSPIKETHD.value_forced = 50
            model.vars.MODEM_MODINDEX_FREQGAINE.value_forced = 7
            model.vars.MODEM_MODINDEX_FREQGAINM.value_forced = 1
            model.vars.MODEM_PRE_PREERRORS.value_forced = 15
            model.vars.MODEM_TIMING_ADDTIMSEQ.value_forced = 6
            model.vars.MODEM_TIMING_OFFSUBDEN.value_forced = 5
            model.vars.MODEM_TIMING_OFFSUBNUM.value_forced = 7
            model.vars.MODEM_TIMING_TIMINGBASES.value_forced = 3
            model.vars.MODEM_TIMING_TIMTHRESH.value_forced = 25
            model.vars.MODEM_CTRL1_RESYNCPER.value_forced = 1
            model.vars.MODEM_LONGRANGE1_PREFILTLEN.value_forced = 1
            model.vars.MODEM_PREFILTCOEFF_PREFILTCOEFF.value_forced = 2736235287
            model.vars.MODEM_COH3_COHDSACMPLX.value_forced = 0
            model.vars.MODEM_SYNCPROPERTIES_STATICSYNCTHRESHEN.value_forced = 1
            model.vars.MODEM_SYNCPROPERTIES_STATICSYNCTHRESH.value_forced = 50
            model.vars.MODEM_COH0_COHCHPWRLOCK.value_forced = 0
            model.vars.MODEM_COH0_COHCHPWRRESTART.value_forced = 1
        elif 868e6 <= base_freq_Hz <= 868.6e6:
            """ Complex Correlation """
            model.vars.MODEM_CTRL6_CPLXCORREN.value_forced = 1
            model.vars.MODEM_COH3_COHDSACMPLX.value_forced = 1

            """ Channel Power Accumulator Setting """
            # : Average and delay
            model.vars.MODEM_LONGRANGE1_AVGWIN.value_forced = 1
            model.vars.MODEM_LONGRANGE1_CHPWRACCUDEL.value_forced = 0

            """ BBSS """
            # : BBSS Channel Power Thresholds
            model.vars.MODEM_LONGRANGE2_LRCHPWRTH1.value_forced = 12
            model.vars.MODEM_LONGRANGE2_LRCHPWRTH2.value_forced = 18
            model.vars.MODEM_LONGRANGE2_LRCHPWRTH3.value_forced = 28
            model.vars.MODEM_LONGRANGE2_LRCHPWRTH4.value_forced = 35
            model.vars.MODEM_LONGRANGE3_LRCHPWRTH5.value_forced = 42
            model.vars.MODEM_LONGRANGE3_LRCHPWRTH6.value_forced = 48
            model.vars.MODEM_LONGRANGE3_LRCHPWRTH7.value_forced = 54
            model.vars.MODEM_LONGRANGE3_LRCHPWRTH8.value_forced = 60
            model.vars.MODEM_LONGRANGE4_LRCHPWRTH9.value_forced = 65
            model.vars.MODEM_LONGRANGE4_LRCHPWRTH10.value_forced = 70
            model.vars.MODEM_LONGRANGE6_LRCHPWRTH11.value_forced = 78

            # : BBSS lookup table
            model.vars.MODEM_LONGRANGE4_LRCHPWRSH1.value_forced = 2
            model.vars.MODEM_LONGRANGE4_LRCHPWRSH2.value_forced = 3
            model.vars.MODEM_LONGRANGE4_LRCHPWRSH3.value_forced = 4
            model.vars.MODEM_LONGRANGE4_LRCHPWRSH4.value_forced = 4
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH5.value_forced = 5
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH6.value_forced = 6
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH7.value_forced = 7
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH8.value_forced = 8
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH9.value_forced = 9
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH10.value_forced = 10
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH11.value_forced = 11
            model.vars.MODEM_LONGRANGE6_LRCHPWRSH12.value_forced = 12

            """ Datafilter """
            # : Per's recommended DATAFILTER value is 7; however, with cplxcorren =1, DATAFILTER = 4 gives improved sens.
            model.vars.MODEM_CTRL2_DATAFILTER.value_forced = 4

            """ DSA Prefilter """
            model.vars.MODEM_PREFILTCOEFF_PREFILTCOEFF.value_forced = 2736235287

            """ Coherent DSA Settings """
            # : Enable coherent demodulator DSA
            model.vars.MODEM_COH3_COHDSAEN.value_forced = 1
            model.vars.MODEM_COH3_CDSS.value_forced = 4

            # : This threshold determines whether to use fixed or dynamic threshold based on channel power.
            # : > 128, FIXED DSA THRESHOLD ALWAYS
            # : = 0  , DYNAMIC DSA THRESHOLD ALWAYS
            # : 0 < x < 128, hybrid - for low channel power, use fixed threshold. For high channel power, use dynamic.
            model.vars.MODEM_LONGRANGE6_LRCHPWRSPIKETH.value_forced = 35

            # : For FIXED DSA mode, this is the correlation threshold
            model.vars.MODEM_LONGRANGE6_LRSPIKETHD.value_forced = 120  # : below 130, floor issues

            # : For dynamic DSA threshold, this is the baseline threshold. Threshold will increase in addition to this
            # : baseline value dependent on the channel power.
            model.vars.MODEM_COH2_FIXEDCDTHFORIIR.value_forced = 120  # Above 120, blocking degradation
            # : Coefficients for IIR, 0 -> 2^-3, 1 -> 2^-4, 2 -> 2^-5 , 3 -> 2^-6, Higher the value slower averaging.
            model.vars.MODEM_COH3_DYNIIRCOEFOPTION.value_forced = 3

            model.vars.MODEM_COH3_PEAKCHKTIMOUT.value_forced = 18

            """ Timing Detection Settings """
            # : Offset between DSA and timing windows
            # : Calculate as OSR*no_of_chips_per_sym*m
            model.vars.MODEM_COH3_COHDSAADDWNDSIZE.value_forced = 864  # -160 ( 864 = 2^10 - 160 )

            """ AFC """
            # : Controls number of bauds to rewind after fixed window timing detection.
            # : baudrate offset range is dependent on this value. Setting to 32 narrows baudrate offset range
            model.vars.MODEM_CTRL6_TDREW.value_forced = 64

            """ Dynamic Preamble / Sync """
            model.vars.MODEM_CTRL5_RESYNCBAUDTRANS.value_forced = 0

            model.vars.MODEM_COH0_COHCHPWRTH0.value_forced = 35  # same as LRCHPWRTH3
            model.vars.MODEM_COH0_COHCHPWRTH1.value_forced = 42  # same as LRCHPWRTH4
            model.vars.MODEM_COH0_COHCHPWRTH2.value_forced = 127

            model.vars.MODEM_COH1_SYNCTHRESH0.value_forced = 29  # 33 < COHCHPWRTH0
            model.vars.MODEM_COH1_SYNCTHRESH1.value_forced = 32  # 36 COHCHPWRTH0 < x < COHCHPWRTH1
            model.vars.MODEM_COH1_SYNCTHRESH2.value_forced = 35  # 39 COHCHPWRTH1 < x < COHCHPWRTH2
            model.vars.MODEM_COH1_SYNCTHRESH3.value_forced = 127  # > COHCHPWRTH2

            model.vars.MODEM_COH2_SYNCTHRESHDELTA0.value_forced = 0  # < COHCHPWRTH0
            model.vars.MODEM_COH2_SYNCTHRESHDELTA1.value_forced = 2  # 2  # COHCHPWRTH0 < x < COHCHPWRTH1
            model.vars.MODEM_COH2_SYNCTHRESHDELTA2.value_forced = 4  # 4  # COHCHPWRTH1 < x < COHCHPWRTH2
            model.vars.MODEM_COH2_SYNCTHRESHDELTA3.value_forced = 0  # > COHCHPWRTH2
        elif 779e6 <= base_freq_Hz == 787e6:
            pass

    def _build_agc_fastloop_settings(self, model):
        base_freq_Hz = model.profile.inputs.base_frequency_hz.var_value

        if 2360e6 <= base_freq_Hz <= 2400e6 or 2400e6 <= base_freq_Hz <= 2483.5e6:
            if model.vars.demod_select.value == model.vars.demod_select.var_enum.COHERENT:
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

                model.vars.AGC_GAINRANGE_GAININCSTEP.value_forced = 1
                model.vars.AGC_GAINRANGE_LATCHEDHISTEP.value_forced = 0
                model.vars.AGC_GAINRANGE_PNGAINSTEP.value_forced = 4
                model.vars.AGC_GAINRANGE_HIPWRTHD.value_forced = 3

                model.vars.AGC_MANGAIN_MANGAINEN.value_forced = 0
                model.vars.AGC_MANGAIN_MANGAINIFPGA.value_forced = 7
                model.vars.AGC_MANGAIN_MANGAINLNA.value_forced = 1
                model.vars.AGC_MANGAIN_MANGAINPN.value_forced = 1

                model.vars.AGC_STEPDWN_STEPDWN0.value_forced = 1
                model.vars.AGC_STEPDWN_STEPDWN1.value_forced = 2
                model.vars.AGC_STEPDWN_STEPDWN2.value_forced = 3
                model.vars.AGC_STEPDWN_STEPDWN3.value_forced = 3
                model.vars.AGC_STEPDWN_STEPDWN4.value_forced = 3
                model.vars.AGC_STEPDWN_STEPDWN5.value_forced = 5

                model.vars.AGC_RSSISTEPTHR_DEMODRESTARTPER.value_forced = 0
                model.vars.AGC_RSSISTEPTHR_DEMODRESTARTTHR.value_forced = 0
                model.vars.AGC_RSSISTEPTHR_POSSTEPTHR.value_forced = 0
            else:
                model.vars.AGC_CTRL2_DISRFPKD.value_forced = 1
                model.vars.AGC_CTRL4_RFPKDCNTEN.value_forced = 0
                model.vars.rssi_period.value_forced = 8
