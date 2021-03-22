from pyradioconfig.parts.common.profiles.rainier_regs import build_modem_regs_rainier
from pyradioconfig.parts.rainier.profiles.sw_profile_outputs_common import SwProfileOutputsCommonRainier
from pyradioconfig.parts.bobcat.profiles.Profile_IEEE802154_OQPSK import ProfileIEEE802154OQPSKBobcat
from py_2_and_3_compatibility import *


class ProfileIEEE802154OQPSKRainier(ProfileIEEE802154OQPSKBobcat):

    def __init__(self):
        super().__init__()
        self._profileName = "IEEE802154OQPSK"
        self._readable_name = "IEEE802154OQPSK Profile"
        self._category = ""
        self._description = "Profile used for IEEE802154 OQPSK phys"
        self._default = False
        self._activation_logic = ""
        self._family = "rainier"
        self._sw_profile_outputs_common = SwProfileOutputsCommonRainier()

    def build_register_profile_outputs(self, model, profile):
        build_modem_regs_rainier(model, profile)

    def build_hidden_profile_inputs(self, model, profile):
        super().build_hidden_profile_inputs(model, profile)
        self.make_hidden_input(profile, model.vars.modulator_select, category="Advanced", readable_name="Modulator Select")

    def _build_modem_settings(self, model):
        super()._build_modem_settings(model)

        if model.vars.demod_select.value == model.vars.demod_select.var_enum.ENHANCED_DSSS:
            self._build_fasthop_settings(model)
            self._build_ennhanced_dsss_settings(model)

        model.vars.deviation_tol_ppm.value_forced = 0
        model.vars.manchester_mapping.value_forced = None
        model.vars.directmode_rx.value_forced = model.vars.directmode_rx.var_enum.DISABLED
        model.vars.pll_bandwidth_tx.value_forced = None
        model.vars.pll_bandwidth_rx.value_forced = None

    def _build_feature_settings(self, model):
        if model.profile.inputs.zigbee_feature.var_value == model.vars.zigbee_feature.var_enum.ENHANCED:
            model.vars.demod_select.value_forced = model.vars.demod_select.var_enum.ENHANCED_DSSS
            model.vars.antdivmode.value_forced = model.vars.antdivmode.var_enum.DISABLE
        else:
            super()._build_feature_settings(model)

    def _build_tolerance_setting(self, model):
        if model.profile.inputs.zigbee_feature.var_value == model.vars.zigbee_feature.var_enum.ENHANCED:
            model.vars.bandwidth_hz.value_forced = 2200000
            model.vars.baudrate_tol_ppm.value_forced = 40
            model.vars.tx_xtal_error_ppm.value_forced = 10
            model.vars.rx_xtal_error_ppm.value_forced = 10
        else:
            super()._build_tolerance_setting(model)

    def _build_ennhanced_dsss_settings(self, model):
        model.vars.frequency_comp_mode.value_forced = model.vars.frequency_comp_mode.var_enum.INTERNAL_ALWAYS_ON
        model.vars.AGC_CTRL1_RSSIPERIOD.value_forced = 8
        model.vars.synth_tx_mode.value_forced = model.vars.synth_tx_mode.var_enum.MODE_IEEE802154
        model.vars.timing_detection_threshold.value_forced = 75
        model.vars.MODEM_AFCADJTX_AFCSCALEE.value_forced = 1

        model.vars.var_length_minlength.value_forced = 4
        model.vars.if_frequency_hz.value_forced = 1370000
        model.vars.deviation_tol_ppm.value_forced = 0
        model.vars.adc_clock_mode.value_forced = model.vars.adc_clock_mode.var_enum.HFXOMULT

        model.vars.MODEM_FRMSCHTIME_DSARSTSYCNEN.value_forced = 1

        model.vars.MODEM_PHDMODCTRL_PMDETEN.value_forced = 1
        model.vars.MODEM_PHDMODCTRL_PMDETTHD.value_forced = 3
        model.vars.MODEM_PRE_PREERRORS.value_forced = 0

        ### AFC
        model.vars.MODEM_AFCADJRX_AFCSCALEE.value_forced = 3
        model.vars.MODEM_AFCADJRX_AFCSCALEM.value_forced = 27
        model.vars.MODEM_AFC_AFCGEAR.value_forced = 3

        ### Ehanced DSSS configuration
        model.vars.MODEM_EHDSSSCTRL_EHDSSSEN.value_forced = 1
        model.vars.MODEM_EHDSSSCTRL_DSSSFRQLIM.value_forced = 45
        model.vars.MODEM_EHDSSSCTRL_DUALDSA.value_forced = 0

        model.vars.MODEM_EHDSSSCFG0_DSSSPATT.value_forced = 122

        model.vars.MODEM_EHDSSSCFG1_DSSSEXPSYNCLEN.value_forced = 128

        model.vars.MODEM_EHDSSSCFG2_MAXSCHMODE.value_forced = 0
        model.vars.MODEM_EHDSSSCFG2_DSSSTRACKINGWIN.value_forced = 2

        model.vars.MODEM_EHDSSSCFG3_DSSSDASMAXTHD.value_forced = 1400
        model.vars.MODEM_EHDSSSCFG3_DSSSFOETRACKGEAR.value_forced = 3
        model.vars.MODEM_EHDSSSCFG3_OPMODE.value_forced = 0
        model.vars.MODEM_EHDSSSCFG3_LQIAVGWIN.value_forced = 1

        ### CH power Qualification
        model.vars.MODEM_PHDMODCTRL_CHPWRQUAL.value_forced = 1
        model.vars.MODEM_LONGRANGE1_AVGWIN.value_forced = 3
        # model.vars.MODEM_COH0_COHCHPWRTH0.value_forced = 217
        model.vars.AGC_CTRL7_SUBPERIOD.value_forced = 1
        model.vars.AGC_CTRL7_SUBDEN.value_forced = 2
        model.vars.AGC_CTRL7_SUBINT.value_forced = 8
        model.vars.AGC_CTRL7_SUBNUM.value_forced = 0

        # : TODO specific settings for phy
        ## force
        # Enhanced DSSS demod settings
        model.vars.MODEM_EHDSSSCTRL_DSSSTIMEACQUTHD.value_forced = 16
        model.vars.MODEM_EHDSSSCTRL_FOEBIAS.value_forced = 2
        # enable to correct frequency error crossing packet
        model.vars.MODEM_EHDSSSCTRL_FREQCORREN.value_forced = 3
        model.vars.MODEM_EHDSSSCTRL_DSSSDSATHD.value_forced = 2

        model.vars.MODEM_EHDSSSCFG1_DSSSCORRTHD.value_forced = 400
        model.vars.MODEM_EHDSSSCFG1_DSSSDSAQTHD.value_forced = 700

        # model.vars.MODEM_EHDSSSCFG2_DSSSTIMCORRTHD.value_forced = 600
        # model.vars.MODEM_EHDSSSCFG2_DSSSCORRSCHWIN.value_forced = 8
        # model.vars.MODEM_EHDSSSCFG2_ONESYMBOLMBDD.value_forced = 0
        model.vars.MODEM_EHDSSSCFG2_DSSSDSAQUALEN.value_forced = 1

        model.vars.MODEM_EHDSSSCFG3_DSSSINITIMLEN.value_forced = 3

        # enable CH filter narrow down to 1.8MHz
        model.vars.MODEM_CHFCTRL_SWCOEFFEN.value_forced = 1

        model.vars.MODEM_PHDMODCTRL_FASTHOPPINGEN.value_forced = 0
        model.vars.MODEM_DIGMIXCTRL_FWHOPPING.value_forced = 0
        model.vars.MODEM_DIGMIXCTRL_BLEORZB.value_forced = 1

        ### enable Enhanced DSSS demod & TRECS demod for DSA detection
        model.vars.MODEM_DUALTIM_DUALTIMEN.value_forced = 0
        model.vars.MODEM_DIGMIXCTRL_DSSSCFECOMBO.value_forced = 0  ## change from 2 to 0 to save power
        ### if DSSSCFECOMBO = 0, the following TREECS settings will be not used
        model.vars.MODEM_VTCORRCFG0_EXPECTPATT.value_forced = long(0xE077AE6C)
        model.vars.MODEM_REALTIMCFE_SYNCACQWIN.value_forced = 31
        model.vars.MODEM_REALTIMCFE_MINCOSTTHD.value_forced = 400
        model.vars.MODEM_EXPECTPATTDUAL_EXPECTPATTDUAL.value_forced = long(0xE077AE6C)
        model.vars.MODEM_DUALTIM_SYNCACQWIN2.value_forced = 31
        model.vars.MODEM_DUALTIM_MINCOSTTHD2.value_forced = 400

        model.vars.MODEM_EHDSSSCFG1_DSSSDSAQTHD.value_forced = 500
        model.vars.MODEM_EHDSSSCTRL_DSSSDSATHD.value_forced = 0
        model.vars.MODEM_EHDSSSCFG2_DSSSDSAQUALEN.value_forced = 1
        model.vars.MODEM_PHDMODCTRL_PMDETEN.value_forced = 1
        model.vars.AGC_CTRL7_SUBINT.value_forced = 8

        # clock gating
        model.vars.MODEM_AUTOCG_AUTOCGEN.value_forced = 0
        model.vars.MODEM_CGCLKSTOP_FORCEOFF.value_forced = 56831

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
            model.vars.MODEM_CTRL5_BBSS.value_forced = 0
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
            model.vars.MODEM_INTAFC_FOEPREAVG0.value_forced = 1
            model.vars.MODEM_INTAFC_FOEPREAVG1.value_forced = 3
            model.vars.MODEM_INTAFC_FOEPREAVG2.value_forced = 5
            model.vars.MODEM_INTAFC_FOEPREAVG3.value_forced = 5
            model.vars.MODEM_LONGRANGE1_AVGWIN.value_forced = 4
            model.vars.MODEM_LONGRANGE1_HYSVAL.value_forced = 3
            model.vars.MODEM_LONGRANGE1_PREFILTLEN.value_forced = 3
            model.vars.MODEM_LONGRANGE1_LRTIMEOUTTHD.value_forced = 320
            model.vars.MODEM_LONGRANGE1_PREFILTLEN.value_forced = 3

            model.vars.MODEM_LONGRANGE2_LRCHPWRTH1.value_forced = 17
            model.vars.MODEM_LONGRANGE2_LRCHPWRTH2.value_forced = 23
            model.vars.MODEM_LONGRANGE2_LRCHPWRTH3.value_forced = 30
            model.vars.MODEM_LONGRANGE2_LRCHPWRTH4.value_forced = 37
            model.vars.MODEM_LONGRANGE3_LRCHPWRTH5.value_forced = 43
            model.vars.MODEM_LONGRANGE3_LRCHPWRTH6.value_forced = 49
            model.vars.MODEM_LONGRANGE3_LRCHPWRTH7.value_forced = 56
            model.vars.MODEM_LONGRANGE3_LRCHPWRTH8.value_forced = 63
            model.vars.MODEM_LONGRANGE4_LRCHPWRTH9.value_forced = 70
            model.vars.MODEM_LONGRANGE4_LRCHPWRTH10.value_forced = 77

            model.vars.MODEM_LONGRANGE4_LRCHPWRSH1.value_forced = 2
            model.vars.MODEM_LONGRANGE4_LRCHPWRSH2.value_forced = 3
            model.vars.MODEM_LONGRANGE4_LRCHPWRSH3.value_forced = 4
            model.vars.MODEM_LONGRANGE4_LRCHPWRSH4.value_forced = 5
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH5.value_forced = 6
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH6.value_forced = 7
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH7.value_forced = 8
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH8.value_forced = 9
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH9.value_forced = 10
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH10.value_forced = 11
            model.vars.MODEM_LONGRANGE5_LRCHPWRSH11.value_forced = 12

            model.vars.MODEM_LONGRANGE6_LRCHPWRSPIKETH.value_forced = 40
            model.vars.MODEM_LONGRANGE6_LRSPIKETHD.value_forced = 105

            # model.vars.MODEM_SRCCHF_SRCRATIO2.value_forced = 15689 # Calc SRC
            model.vars.MODEM_TIMING_TIMTHRESH.value_forced = 35

            # model.vars.RAC_TIACTRL_LNAMIXRFPKDTHRESHSEL.value_forced = 2
            model.vars.RAC_TIACTRL0_TIATHRPKDHISEL.value_forced = 6
            model.vars.RAC_TIACTRL0_TIATHRPKDLOSEL.value_forced = 2
            # model.vars.RAC_SYNTHCTRL_MMDPOWERBALANCEDISABLE.value_forced = 1
            # model.vars.RAC_SYNTHREGCTRL_MMDLDOVREFTRIM.value_forced = 3
            model.vars.RAC_TIAEN_TIAENLATCHI.value_forced = 1
            model.vars.RAC_TIAEN_TIAENLATCHQ.value_forced = 1

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

    def _build_agc_fasthop_settings(self, model):
        # AGC Mode
        model.vars.AGC_CTRL0_MODE.value_forced = 2  # Freeze AGC at frame detect

        # PKD Settings
        model.vars.AGC_GAINRANGE_PNGAINSTEP.value_forced = 3  # 6dB adjustments
        model.vars.AGC_AGCPERIOD0_PERIODHISTL.value_forced = 0  # Enable IFPKD high period counter right after gain change
        model.vars.AGC_AGCPERIOD0_MAXHICNTTHD.value_forced = 8  # Maximum count of 8 (after settling time of 6)
        model.vars.AGC_HICNTREGION0_HICNTREGION0.value_forced = 4
        model.vars.AGC_HICNTREGION0_HICNTREGION1.value_forced = 5
        model.vars.AGC_HICNTREGION0_HICNTREGION2.value_forced = 6
        model.vars.AGC_HICNTREGION0_HICNTREGION3.value_forced = 7
        model.vars.AGC_HICNTREGION1_HICNTREGION4.value_forced = 8
        model.vars.AGC_STEPDWN_STEPDWN0.value_forced = 1
        model.vars.AGC_STEPDWN_STEPDWN1.value_forced = 2
        model.vars.AGC_STEPDWN_STEPDWN2.value_forced = 3
        model.vars.AGC_STEPDWN_STEPDWN3.value_forced = 3
        model.vars.AGC_STEPDWN_STEPDWN4.value_forced = 5
        model.vars.AGC_STEPDWN_STEPDWN5.value_forced = 5

        # AGC Period Settings
        model.vars.AGC_CTRL7_SUBPERIOD.value_forced = 1
        model.vars.AGC_CTRL7_SUBDEN.value_forced = 2
        model.vars.AGC_CTRL7_SUBINT.value_forced = 4
        model.vars.AGC_CTRL7_SUBNUM.value_forced = 0

    def _build_fasthop_settings(self, model):
        ### CH power Qualification
        model.vars.MODEM_PHDMODCTRL_CHPWRQUAL.value_forced = 1
        model.vars.MODEM_LONGRANGE1_AVGWIN.value_forced = 3
        model.vars.MODEM_COH0_COHCHPWRTH0.value_forced = 212
        ### Using CH power to detect false DSA detection
        ### After DSA is detected within 2 DSSS symbol
        model.vars.MODEM_COH0_COHCHPWRTH1.value_forced = 64
        ### If CH power is belower than the thrshold that is configured in COH0_COHCHPWRTH0
        ### for 8 chips, the DSA detection will be reset
        model.vars.MODEM_COH0_COHCHPWRTH2.value_forced = 8

        model.vars.MODEM_AUTOCG_AUTOCGEN.value_forced = 1

        model.vars.MODEM_SYNC2_SYNC2.value_forced = 167
        model.vars.MODEM_SYNC3_SYNC3.value_forced = 47
        model.vars.MODEM_SYNCWORDCTRL_SYNCDET2TH.value_forced = 1
        model.vars.MODEM_SYNCWORDCTRL_DUALSYNC2TH.value_forced = 1
        model.vars.MODEM_SYNCWORDCTRL_SYNCBITS2TH.value_forced = 7
        model.vars.MODEM_SYNCWORDCTRL_SYNCSWFEC.value_forced = 0

        model.vars.MODEM_PHDMODCTRL_PMDETEN.value_forced = 0
        model.vars.MODEM_COCURRMODE_CORRCHKMUTE.value_forced = 8

        model.vars.MODEM_FRMSCHTIME_FRMSCHTIME.value_forced = 40

        model.vars.MODEM_REALTIMCFE_MINCOSTTHD.value_forced = 225
        model.vars.MODEM_REALTIMCFE_RTCFEEN.value_forced = 1
        model.vars.MODEM_REALTIMCFE_RTSCHMODE.value_forced = 0
        model.vars.MODEM_REALTIMCFE_RTSCHWIN.value_forced = 4
        model.vars.MODEM_REALTIMCFE_SINEWEN.value_forced = 0
        model.vars.MODEM_REALTIMCFE_SYNCACQWIN.value_forced = 12
        model.vars.MODEM_REALTIMCFE_TRACKINGWIN.value_forced = 7
        model.vars.MODEM_REALTIMCFE_VTAFCFRAME.value_forced = 1

        model.vars.MODEM_VITERBIDEMOD_CORRCYCLE.value_forced = 0
        model.vars.MODEM_VITERBIDEMOD_CORRSTPSIZE.value_forced = 4
        model.vars.MODEM_VITERBIDEMOD_HARDDECISION.value_forced = 0
        model.vars.MODEM_VITERBIDEMOD_SYNTHAFC.value_forced = 1
        model.vars.MODEM_VITERBIDEMOD_VITERBIKSI1.value_forced = 65
        model.vars.MODEM_VITERBIDEMOD_VITERBIKSI2.value_forced = 45
        model.vars.MODEM_VITERBIDEMOD_VITERBIKSI3.value_forced = 24
        model.vars.MODEM_VITERBIDEMOD_VTDEMODEN.value_forced = 1
        model.vars.MODEM_VTBLETIMING_DISDEMODOF.value_forced = 1
        model.vars.MODEM_VTBLETIMING_FLENOFF.value_forced = 0
        model.vars.MODEM_VTBLETIMING_TIMINGDELAY.value_forced = 60
        model.vars.MODEM_VTBLETIMING_VTBLETIMINGSEL.value_forced = 0
        # model.vars.MODEM_VTCORRCFG0_EXPECTPATT.value_forced = 1854799872
        model.vars.MODEM_VTCORRCFG1_EXPECTHT.value_forced = 5
        model.vars.MODEM_VTCORRCFG1_EXPSYNCLEN.value_forced = 74
        model.vars.MODEM_VTCORRCFG1_KSI3SWENABLE.value_forced = 0
        model.vars.MODEM_VTCORRCFG1_VITERBIKSI3WB.value_forced = 33
        model.vars.MODEM_VTCORRCFG1_VTFRQLIM.value_forced = 120
        model.vars.MODEM_VTTRACK_FREQBIAS.value_forced = 0
        model.vars.MODEM_VTTRACK_FREQTRACKMODE.value_forced = 1
        model.vars.MODEM_VTTRACK_HIPWRTHD.value_forced = 1
        model.vars.MODEM_VTTRACK_TIMEACQUTHD.value_forced = 238
        model.vars.MODEM_VTTRACK_TIMGEAR.value_forced = 0
        model.vars.MODEM_VTTRACK_TIMTRACKTHD.value_forced = 2

        model.vars.MODEM_TRECPMDET_PMCOSTVALTHD.value_forced = 2
        model.vars.MODEM_TRECPMDET_PMTIMEOUTSEL.value_forced = 2
        model.vars.MODEM_TRECPMDET_PREAMSCH.value_forced = 0
        ### re-use this PM registers to search DSSS preamble pattern
        model.vars.MODEM_TRECPMDET_PMMINCOSTTHD.value_forced = 350
        model.vars.MODEM_TRECPMPATT_PMEXPECTPATT.value_forced = long(0xE077AE6C)
        model.vars.MODEM_TRECPMDET_PMACQUINGWIN.value_forced = 31

        model.vars.MODEM_TRECSCFG_PMOFFSET.value_forced = 10
        model.vars.MODEM_TRECSCFG_TRECSOSR.value_forced = 4

        model.vars.MODEM_PHDMODCTRL_FASTHOPPINGEN.value_forced = 0
        model.vars.MODEM_DIGMIXCTRL_FWHOPPING.value_forced = 0
        model.vars.MODEM_DIGMIXCTRL_BLEORZB.value_forced = 1
        model.vars.MODEM_SRCCHF_CHMUTETIMER.value_forced = 245

        model.vars.MODEM_EHDSSSCTRL_FREQCORREN.value_forced = 3
        model.vars.MODEM_EHDSSSCTRL_DSSSDSATHD.value_forced = 0
        model.vars.MODEM_EHDSSSCTRL_DSSSFRMTIMEOUT.value_forced = 4

        model.vars.MODEM_EHDSSSCFG0_DSSSPATT.value_forced = 122
        model.vars.MODEM_EHDSSSCFG1_DSSSEXPSYNCLEN.value_forced = 128
        model.vars.MODEM_EHDSSSCFG1_DSSSCORRTHD.value_forced = 400
        model.vars.MODEM_EHDSSSCFG1_DSSSDSAQTHD.value_forced = 700
        model.vars.MODEM_EHDSSSCFG2_DSSSTIMCORRTHD.value_forced = 500
        model.vars.MODEM_EHDSSSCFG2_DSSSFRTCORRTHD.value_forced = 600
        model.vars.MODEM_EHDSSSCFG2_ONESYMBOLMBDD.value_forced = 1
        model.vars.MODEM_EHDSSSCFG2_DSSSDSAQUALEN.value_forced = 1
        model.vars.MODEM_EHDSSSCFG2_DSSSCORRSCHWIN.value_forced = 6

        # # AFC
        model.vars.MODEM_AFCADJRX_AFCSCALEE.value_forced = 3
        model.vars.MODEM_AFCADJRX_AFCSCALEM.value_forced = 27
        model.vars.MODEM_AFC_AFCONESHOT.value_forced = 0
        model.vars.MODEM_AFC_AFCGEAR.value_forced = 3
        model.vars.MODEM_VITERBIDEMOD_SYNTHAFC.value_forced = 1

        model.vars.MODEM_CHFCTRL_ADJGAINWIN.value_forced = 85