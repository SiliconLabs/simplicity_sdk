from pyradioconfig.parts.common.profiles.bobcat_regs import build_modem_regs_bobcat
from pyradioconfig.parts.bobcat.profiles.sw_profile_outputs_common import sw_profile_outputs_common_bobcat
from pyradioconfig.parts.ocelot.profiles.Profile_IEEE802154_OQPSK import ProfileIEEE802154OQPSKOcelot


class ProfileIEEE802154OQPSKBobcat(ProfileIEEE802154OQPSKOcelot):

    def __init__(self):
        super().__init__()
        self._profileName = "IEEE802154OQPSK"
        self._readable_name = "IEEE802154OQPSK Profile"
        self._category = ""
        self._description = "Profile used for IEEE802154 OQPSK phys"
        self._default = False
        self._activation_logic = ""
        self._family = "bobcat"
        self._sw_profile_outputs_common = sw_profile_outputs_common_bobcat()

    def build_register_profile_outputs(self, model, profile):
        build_modem_regs_bobcat(model, profile)

    def _build_feature_settings(self, model):
        if model.profile.inputs.zigbee_feature.var_value == model.vars.zigbee_feature.var_enum.STANDARD or \
                model.profile.inputs.zigbee_feature.var_value == model.vars.zigbee_feature.var_enum.FEM or \
                model.profile.inputs.zigbee_feature.var_value == model.vars.zigbee_feature.var_enum.COHERENT:
            model.vars.demod_select.value_forced = model.vars.demod_select.var_enum.COHERENT
        else:
            model.vars.demod_select.value_forced = model.vars.demod_select.var_enum.LEGACY

        if model.profile.inputs.zigbee_feature.var_value == model.vars.zigbee_feature.var_enum.ANTDIV or \
                model.profile.inputs.zigbee_feature.var_value == model.vars.zigbee_feature.var_enum.ANTDIV_FEM:
            model.vars.antdivmode.value_forced = model.vars.antdivmode.var_enum.ANTSELCORR
            model.vars.antdivrepeatdis.value_forced = model.vars.antdivrepeatdis.var_enum.NOREPEATFIRST
        else:
            model.vars.antdivmode.value_forced = model.vars.antdivmode.var_enum.DISABLE

        if model.profile.inputs.zigbee_feature.var_value == model.vars.zigbee_feature.var_enum.FCS:
            model.vars.antdivmode.value_forced = model.vars.antdivmode.var_enum.ANTSELFIRST
            model.vars.antdivrepeatdis.value_forced = model.vars.antdivrepeatdis.var_enum.NOREPEATFIRST

        if model.profile.inputs.zigbee_feature.var_value == model.vars.zigbee_feature.var_enum.FEM:
            model.vars.demod_select.value_forced = model.vars.demod_select.var_enum.COHERENT

    def _build_modem_settings(self, model):
        super()._build_modem_settings(model)

        if model.profile.inputs.zigbee_feature.var_value == model.vars.zigbee_feature.var_enum.FEM:
            self._build_fem_settings(model)

        if model.vars.antdivmode.value_forced != model.vars.antdivmode.var_enum.DISABLE:
            self._build_antdiv_settings(model)

        if model.profile.inputs.zigbee_feature.var_value == model.vars.zigbee_feature.var_enum.FCS:
            self._build_fast_switch_settings(model)

    def _build_cohdsa_settings(self, model):
        base_freq_Hz = model.profile.inputs.base_frequency_hz.var_value

        model.vars.deviation_tol_ppm.value_forced = 0
        if 2360e6 <= base_freq_Hz <= 2400e6 or 2400e6 <= base_freq_Hz <= 2483.5e6:
            model.vars.if_frequency_hz.value_forced = 1370000

            model.vars.frequency_comp_mode.value_forced = model.vars.frequency_comp_mode.var_enum.DISABLED
            model.vars.number_of_timing_windows.value_forced = 7
            model.vars.rssi_period.value_forced = 3
            model.vars.timing_detection_threshold.value_forced = 65
            model.vars.timing_sample_threshold.value_forced = 0

            # Additional overrides introduced when Series 2 AGC calculations added. These prevent the PHY from chan
            # ging versus what was used during Validation.
            model.vars.AGC_LNABOOST_LNABWADJ.value_forced = 0
            model.vars.AGC_CTRL0_PWRTARGET.value_forced = 20
            model.vars.AGC_CTRL1_PWRPERIOD.value_forced = 4
            model.vars.FRC_AUTOCG_AUTOCGEN.value_forced = 7
            model.vars.MODEM_AFC_AFCRXCLR.value_forced = 1
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
            model.vars.MODEM_LONGRANGE1_PREFILTLEN.value_forced = 3
            model.vars.MODEM_LONGRANGE1_LRTIMEOUTTHD.value_forced = 320
            model.vars.MODEM_LONGRANGE1_PREFILTLEN.value_forced = 3
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

            # model.vars.MODEM_SRCCHF_SRCRATIO2.value_forced = 15689 # Calc SRC
            model.vars.MODEM_TIMING_TIMTHRESH.value_forced = 35

            # model.vars.RAC_PGACTRL_LNAMIXRFPKDTHRESHSEL.value_forced = 2
            model.vars.RAC_PGACTRL_PGATHRPKDHISEL.value_forced = 5
            model.vars.RAC_PGACTRL_PGATHRPKDLOSEL.value_forced = 1
            # model.vars.RAC_SYNTHCTRL_MMDPOWERBALANCEDISABLE.value_forced = 1
            model.vars.RAC_SYNTHREGCTRL_MMDLDOVREFTRIM.value_forced = 3
            model.vars.RAC_PGACTRL_PGAENLATCHI.value_forced = 1
            model.vars.RAC_PGACTRL_PGAENLATCHQ.value_forced = 1
            model.vars.SYNTH_LPFCTRL1CAL_OP1BWCAL.value_forced = 11
            model.vars.SYNTH_LPFCTRL1CAL_OP1COMPCAL.value_forced = 14
            model.vars.SYNTH_LPFCTRL1CAL_RFBVALCAL.value_forced = 0
            model.vars.SYNTH_LPFCTRL1CAL_RPVALCAL.value_forced = 0
            model.vars.SYNTH_LPFCTRL1CAL_RZVALCAL.value_forced = 9
            # Derived empirically
            # https://confluence.silabs.com/display/RAIL/Panther+Weaksymbols+WifiBlocker+Characterization
            model.vars.MODEM_CTRL2_SQITHRESH.value_forced = 56

            model.vars.synth_tx_mode.value_forced = model.vars.synth_tx_mode.var_enum.MODE_IEEE802154

            # : Change PN gain step for improved WiFi blocker performance
            model.vars.AGC_GAINRANGE_PNGAINSTEP.value_forced = 3

            # : Increase dsa threshold to remove floor issues at high signal power (> -40 dBm)
            model.vars.MODEM_LONGRANGE6_LRSPIKETHD.value_forced = 115

            # MCUW_RADIO_CFG-1809: miwu: Minimize delay + average for dynamic threshold to avoid undesired correlation gain.
            model.vars.MODEM_LONGRANGE1_AVGWIN.value_forced = 0
            model.vars.MODEM_COH2_FIXEDCDTHFORIIR.value_forced = 125
            model.vars.MODEM_LONGRANGE6_LRCHPWRSPIKETH.value_forced = 35
        else:
            super()._build_cohdsa_settings(model)
            model.vars.bandwidth_tol.value_forced = 0.0

    def _build_legacy_settings(self, model):
        # model.vars.rx_xtal_error_ppm.value_forced = 0
        # model.vars.tx_xtal_error_ppm.value_forced = 0
        # model.vars.baudrate_tol_ppm.value_forced = 4000
        model.vars.deviation_tol_ppm.value_forced = 0

        model.vars.frequency_comp_mode.value_forced = model.vars.frequency_comp_mode.var_enum.INTERNAL_ALWAYS_ON

        model.vars.bandwidth_hz.value_forced = 2524800
        model.vars.if_frequency_hz.value_forced = 1370000
        model.vars.timing_detection_threshold.value_forced = 75
        model.vars.timing_resync_period.value_forced = 2
        model.vars.timing_sample_threshold.value_forced = 0
        model.vars.FRC_AUTOCG_AUTOCGEN.value_forced = 7
        model.vars.MODEM_CGCLKSTOP_FORCEOFF.value_forced = 0x1003  # 0, 1, 12

        if model.part_family.lower() not in ["rainier"]:
            model.vars.SYNTH_LPFCTRL1CAL_OP1BWCAL.value_forced = 11
            model.vars.SYNTH_LPFCTRL1CAL_OP1COMPCAL.value_forced = 14
            model.vars.SYNTH_LPFCTRL1CAL_RFBVALCAL.value_forced = 0
            model.vars.SYNTH_LPFCTRL1CAL_RPVALCAL.value_forced = 0
            model.vars.SYNTH_LPFCTRL1CAL_RZVALCAL.value_forced = 9

        model.vars.AGC_CTRL1_RSSIPERIOD.value_forced = 8
        model.vars.synth_tx_mode.value_forced = model.vars.synth_tx_mode.var_enum.MODE_IEEE802154
        model.vars.rx_sync_delay_ns.value_forced = 6625
        model.vars.rx_eof_delay_ns.value_forced = 6625

    def _build_antdiv_settings(self, model):
        # : Disable phase demod path in order to improve frequency offset tolerance
        model.vars.MODEM_CTRL1_PHASEDEMOD.value_forced = 0

        # : Antenna diversity mode
        model.vars.antdiv_enable_parallel_correlation.value_forced = True

        # : Antenna diversity skip and delay
        model.vars.antdiv_switch_delay_us.value_forced = 4.0
        model.vars.antdiv_switch_skip_us.value_forced = 2.0

        # : Antenna diversity controls
        model.vars.MODEM_ADPC2_ADENCORR32.value_forced = 1
        model.vars.MODEM_CTRL0_DUALCORROPTDIS.value_forced = 1
        model.vars.MODEM_ADPC2_ADPCWNDCNTRST.value_forced = 0

        # : AGC settings
        model.vars.AGC_GAINRANGE_PNGAINSTEP.value_forced = 3  # Change PN gain step for improved WiFi blocker performance
        model.vars.AGC_ANTDIV_GAINMODE.value_forced = 1
        model.vars.AGC_GAINSTEPLIM0_CFLOOPDEL.value_forced = 40

        # : Timing and preamble thresholds
        model.vars.MODEM_TIMING_TIMTHRESH.value_forced = 90
        model.vars.MODEM_CTRL6_TIMTHRESHGAIN.value_forced = 1

        # : Antenna diversity correlation decision settings
        if model.profile.inputs.zigbee_feature.var_value == model.vars.zigbee_feature.var_enum.ANTDIV_FEM:
            model.vars.antdiv_adpcsigampthr.value_forced = 704
        else:
            model.vars.antdiv_adpcsigampthr.value = 288

        model.vars.MODEM_CTRL5_LINCORR.value_forced = 1
        model.vars.MODEM_ADQUAL6_ADBACORRTHR.value_forced = 0
        model.vars.MODEM_ADQUAL8_ADBACORRTHR2.value_forced = 0
        model.vars.MODEM_ADQUAL6_ADBACORRDIFF.value_forced = 500
        model.vars.MODEM_ADQUAL5_ADDIRECTCORR.value_forced = 65535

        # : Antenna diversity RSSI decision settings
        model.vars.MODEM_ADQUAL7_ADBARSSITHR.value_forced = 512
        model.vars.MODEM_ADQUAL7_ADBARSSIDIFF.value_forced = 0
        model.vars.MODEM_ADQUAL4_ADRSSIGRTHR.value_forced = 512

        # : Antenna diversity BBSS settings
        model.vars.MODEM_ADPC3_ADBBSSEN.value_forced = 0
        model.vars.MODEM_ADPC3_ADBBSSAMPEXP.value_forced = 11
        model.vars.MODEM_ADPC3_ADBBSSAMPMANT.value_forced = 0
        model.vars.MODEM_ADPC3_ADBBSSAVGEN.value_forced = 1
        model.vars.MODEM_ADPC3_ADBBSSFILTLENGTH.value_forced = 0
        model.vars.MODEM_ADPC3_ADBBSSAVGPER.value_forced = 2
        model.vars.MODEM_ADPC3_ADBBSSAVGWAIT.value_forced = 0
        model.vars.MODEM_ADPC3_ADBBSSAVGFREEZE.value_forced = 1

        # Antenna Diversity settings
        model.vars.MODEM_ADQUAL4_ADAGCGRTHR.value_forced = 63
        model.vars.MODEM_ADQUAL8_ADBAAGCTHR.value_forced = 0
        model.vars.MODEM_ADQUAL4_ADGRMODE.value_forced = 0
        model.vars.MODEM_ADQUAL8_ADBAMODE.value_forced = 1

        # : For debug: Statistic selection
        model.vars.MODEM_ADFSM0_ADSTAT1SEL.value_forced = 6

        # 1 window 48
        model.vars.MODEM_ADPC8_ADPCOSR.value_forced = 5
        model.vars.MODEM_ADPC8_ADPCANTSAMPOFFSET.value_forced = 3

        model.vars.MODEM_CTRL6_ARW.value_forced = 1
        model.vars.MODEM_TIMING_ADDTIMSEQ.value_forced = 0

        model.vars.AGC_CTRL1_RSSIPERIOD.value_forced = 4

        if model.profile.inputs.zigbee_feature.var_value == model.vars.zigbee_feature.var_enum.FCS:
            model.vars.antdiv_enable_parallel_correlation.value_forced = False
            model.vars.MODEM_ANTDIVCTRL_ENADPRETHRESH.value_forced = 0x1
            model.vars.MODEM_ANTDIVCTRL_ADPRETHRESH.value_forced = 0x12
            model.vars.MODEM_ADCTRL1_ADCTRL1.value_forced = 1074331649
            model.vars.MODEM_ADCTRL2_ADCTRL2.value_forced = 1050625
            model.vars.MODEM_ADPC1_ADPCWNDCNT.value_forced = 2
            model.vars.MODEM_ADPC1_ADPCTIMINGBAUDS.value_forced = 0x40
            model.vars.MODEM_ADPC2_ADPCCORRSAMPLES.value_forced = 0x140
            model.vars.MODEM_ADPC8_ADPCANTSAMPSWITCH.value_forced = 0xb4
            model.vars.MODEM_ADPC8_ADPCANTSAMPWRITE.value_forced = 0x3c
            model.vars.MODEM_ADPC8_ADPCANTSAMPBUF.value_forced = 0x28

    def _build_fem_settings(self, model):
        if model.vars.demod_select.value == model.vars.demod_select.var_enum.COHERENT:
            fem_chpwr_adjustment = 6

            model.vars.MODEM_LONGRANGE2_LRCHPWRTH1.value_forced = 20 + fem_chpwr_adjustment
            model.vars.MODEM_LONGRANGE2_LRCHPWRTH2.value_forced = 26 + fem_chpwr_adjustment
            model.vars.MODEM_LONGRANGE2_LRCHPWRTH3.value_forced = 33 + fem_chpwr_adjustment
            model.vars.MODEM_LONGRANGE2_LRCHPWRTH4.value_forced = 40 + fem_chpwr_adjustment
            model.vars.MODEM_LONGRANGE3_LRCHPWRTH5.value_forced = 46 + fem_chpwr_adjustment
            model.vars.MODEM_LONGRANGE3_LRCHPWRTH6.value_forced = 52 + fem_chpwr_adjustment
            model.vars.MODEM_LONGRANGE3_LRCHPWRTH7.value_forced = 59 + fem_chpwr_adjustment
            model.vars.MODEM_LONGRANGE3_LRCHPWRTH8.value_forced = 66 + fem_chpwr_adjustment
            model.vars.MODEM_LONGRANGE4_LRCHPWRTH9.value_forced = 73 + fem_chpwr_adjustment
            model.vars.MODEM_LONGRANGE4_LRCHPWRTH10.value_forced = 80 + fem_chpwr_adjustment

            model.vars.MODEM_COH0_COHCHPWRTH0.value_forced = 33 + fem_chpwr_adjustment
            model.vars.MODEM_COH0_COHCHPWRTH1.value_forced = 40 + fem_chpwr_adjustment
            model.vars.MODEM_COH0_COHCHPWRTH2.value_forced = 100 + fem_chpwr_adjustment

            fem_bbss_adjustment = 1

            model.vars.MODEM_LONGRANGE4_LRCHPWRSH1.value_forced = 3 + fem_bbss_adjustment
            model.vars.MODEM_LONGRANGE4_LRCHPWRSH2.value_forced = 4 + fem_bbss_adjustment
            model.vars.MODEM_LONGRANGE4_LRCHPWRSH3.value_forced = 5 + fem_bbss_adjustment
            model.vars.MODEM_LONGRANGE4_LRCHPWRSH4.value_forced = 5 + fem_bbss_adjustment
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH5.value_forced = 6 + fem_bbss_adjustment
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH6.value_forced = 7 + fem_bbss_adjustment
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH7.value_forced = 8 + fem_bbss_adjustment
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH8.value_forced = 9 + fem_bbss_adjustment
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH9.value_forced = 10 + fem_bbss_adjustment
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH10.value_forced = 11 + fem_bbss_adjustment
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH11.value_forced = 12 + fem_bbss_adjustment

    def _build_fast_switch_settings(self, model):
        model.vars.synth_settling_mode.value_forced = model.vars.synth_settling_mode.var_enum.FAST

        model.vars.antdiv_adpcsigampthr.value_forced = 288
        model.vars.antdiv_enable_parallel_correlation.value_forced = False
        model.vars.antdiv_switch_delay_us.value_forced = 4.0
        model.vars.antdiv_switch_skip_us.value_forced = 2.0

        model.vars.MODEM_INTAFC_FOEPREAVG2.value_forced = 4
        model.vars.MODEM_INTAFC_FOEPREAVG1.value_forced = 2
        model.vars.MODEM_INTAFC_FOEPREAVG0.value_forced = 1
        model.vars.MODEM_CTRL6_TDREW.value_forced = 0x20
        model.vars.MODEM_TIMING_TIMINGBASES.value_forced = 2
        model.vars.MODEM_CTRL6_PSTIMABORT0.value_forced = 1
        model.vars.MODEM_CTRL6_PSTIMABORT1.value_forced = 1
        model.vars.MODEM_CTRL6_PSTIMABORT2.value_forced = 1
        model.vars.MODEM_CTRL5_LINCORR.value_forced = 0
        model.vars.MODEM_DCCOMP_DCCOMPGEAR.value_forced = 7
        model.vars.MODEM_RXRESTART_ANTSWRSTFLTTDIS.value_forced = 0
        model.vars.AGC_AGCPERIOD0_PERIODHI.value_forced = 7
        model.vars.AGC_AGCPERIOD1_PERIODLOW.value_forced = 21
        model.vars.AGC_HICNTREGION0_HICNTREGION0.value_forced = 4
        model.vars.AGC_HICNTREGION0_HICNTREGION1.value_forced = 6
        model.vars.AGC_HICNTREGION0_HICNTREGION2.value_forced = 6
        model.vars.AGC_HICNTREGION0_HICNTREGION3.value_forced = 7
        model.vars.AGC_HICNTREGION1_HICNTREGION4.value_forced = 7
        model.vars.RAC_SYTRIM1_SYTRIMMMDREGAMPBW.value_forced = 0
        model.vars.RAC_SYTRIM1_SYTRIMMMDREGAMPBIAS.value_forced = 3

    def _build_agc_fastloop_settings(self, model):
        super()._build_agc_fastloop_settings(model)
        base_freq_Hz = model.profile.inputs.base_frequency_hz.var_value

        if 2360e6 <= base_freq_Hz <= 2400e6 or 2400e6 <= base_freq_Hz <= 2483.5e6:
            if model.vars.demod_select.value == model.vars.demod_select.var_enum.LEGACY:
                model.vars.AGC_CTRL2_DISRFPKD.value_forced = 0
                model.vars.AGC_CTRL4_RFPKDCNTEN.value_forced = 1
                model.vars.rssi_period.value_forced = 4
