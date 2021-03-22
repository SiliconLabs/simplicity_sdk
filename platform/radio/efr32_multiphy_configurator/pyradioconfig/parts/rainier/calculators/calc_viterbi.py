from pyradioconfig.parts.bobcat.calculators.calc_viterbi import Calc_Viterbi_Bobcat
from pyradioconfig.calculator_model_framework.Utils.LogMgr import LogMgr

class CalcViterbiRainier(Calc_Viterbi_Bobcat):
    acqwin_unit = 1

    def calc_pmoffset_reg(self, model):

        afc_oneshot_enabled = (model.vars.MODEM_AFC_AFCONESHOT.value == 1)
        rtschmode = model.vars.MODEM_REALTIMCFE_RTSCHMODE.value
        osr = model.vars.MODEM_TRECSCFG_TRECSOSR.value

        if (rtschmode == 1) and afc_oneshot_enabled:
            # Special case for dual syncword detection case where hard slicing on syncword is required
            # In this case we choose a minimal PMOFFSET to avoid a bad estimate due to AFC transient
            pmoffset = 2
        else:
            # + 2 for processing delay. Always a function of OSR as per Wentao
            pmoffset = osr * 2 + 2

        self._reg_write(model.vars.MODEM_TRECSCFG_PMOFFSET, pmoffset)

    def calc_realtimcfe_extenschbyp_reg(self, model):
        pass

    def calc_realtimcfe_rtschmode_reg(self, model):
        # This function calculates the RTSCHMODE register field for TRECS

        # Read in model variables
        dualsync = model.vars.syncword_dualsync.value
        demod_select = model.vars.demod_select.value
        ber_force_fdm0 = model.vars.ber_force_fdm0.value
        pmdeten = model.vars.MODEM_PHDMODCTRL_PMDETEN.value
        fast_detect_enable = (model.vars.fast_detect_enable.value == model.vars.fast_detect_enable.var_enum.ENABLED)

        # Calculate the register value based on whether we are using TRECS and whether dual syncword detect is enabled
        if demod_select == model.vars.demod_select.var_enum.TRECS_VITERBI or \
                demod_select == model.vars.demod_select.var_enum.TRECS_SLICER:
            # If dual syncword detection is enabled, then stop using CFE and hard slice syncword w TRECS
            if fast_detect_enable:
                rtschmode = 1
            else:
                rtschmode = 0  # 0 means detect timing again using syncword
        else:
            rtschmode = 0

        # Write the register
        self._reg_write(model.vars.MODEM_REALTIMCFE_RTSCHMODE, rtschmode)

    def calc_swcoeffen_reg(self, model):

        afc1shot_en = model.vars.MODEM_AFC_AFCONESHOT.value
        aox_en = model.vars.aox_enable.value == model.vars.aox_enable.var_enum.ENABLED
        # ksi3swen is do not care if afc1shot_en is False - MCUW_RADIO_CFG-1901
        ksi3swen_donotccare = not afc1shot_en

        if afc1shot_en and aox_en:
            # both AFC oneshot and AoX cannot be simultaneously enabled as they both use the second CHF coefficient set
            LogMgr.Error('both AFC oneshot and AoX cannot be simultaneously enabled')

        swcoeffen = 1 if afc1shot_en or aox_en else 0 # affects the channel filter switching only
        ksi3swenable = afc1shot_en
        # don't switch for aox, as KSI3 switch mechanism is based on dsa/preamble, but the aox channel switch is based on the CTE
        # Don't care about the demodulated data during CTE, so just leave it on the KSI3

        self._reg_write(model.vars.MODEM_CHFCTRL_SWCOEFFEN, swcoeffen)
        self._reg_write(model.vars.MODEM_VTCORRCFG1_KSI3SWENABLE, ksi3swenable, do_not_care=ksi3swen_donotccare)

