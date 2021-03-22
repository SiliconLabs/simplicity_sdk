from pyradioconfig.parts.common.profiles.lynx_regs import build_modem_regs_lynx
from pyradioconfig.parts.panther.profiles.Profile_IEEE802154_OQPSK import ProfileIEEE802154OQPSKPanther


class ProfileIEEE802154OQPSKLynx(ProfileIEEE802154OQPSKPanther):

    def __init__(self):
        super().__init__()
        self._profileName = "IEEE802154OQPSK"
        self._readable_name = "IEEE802154 OQPSK Profile"
        self._category = ""
        self._description = "Profile used for IEEE802154 OQPSK phys"
        self._default = False
        self._activation_logic = ""
        self._family = "lynx"

    def build_register_profile_outputs(self, model, profile):
        build_modem_regs_lynx(model, profile)

    def _build_feature_settings(self, model):
        if model.profile.inputs.zigbee_feature.var_value == model.vars.zigbee_feature.var_enum.STANDARD:
            # : Default demod is Legacy for this device
            model.vars.demod_select.value_forced = model.vars.demod_select.var_enum.LEGACY
        else:
            super()._build_feature_settings(model)

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

        model.vars.FRC_AUTOCG_AUTOCGEN.value_forced = 7
        model.vars.MODEM_CGCLKSTOP_FORCEOFF.value_forced = 0x1003  # 0, 1, 12
        model.vars.RAC_SYNTHCTRL_MMDPOWERBALANCEDISABLE.value_forced = 1
        model.vars.SYNTH_LPFCTRL1CAL_OP1BWCAL.value_forced = 11
        model.vars.SYNTH_LPFCTRL1CAL_OP1COMPCAL.value_forced = 14
        model.vars.SYNTH_LPFCTRL1CAL_RFBVALCAL.value_forced = 0
        model.vars.SYNTH_LPFCTRL1CAL_RPVALCAL.value_forced = 0
        model.vars.SYNTH_LPFCTRL1CAL_RZVALCAL.value_forced = 9

    def _build_agc_fastloop_settings(self, model):
        model.vars.AGC_CTRL2_DISRFPKD.value_forced = 1
        model.vars.AGC_CTRL4_RFPKDCNTEN.value_forced = 0
        model.vars.AGC_CTRL1_RSSIPERIOD.value_forced = 8
