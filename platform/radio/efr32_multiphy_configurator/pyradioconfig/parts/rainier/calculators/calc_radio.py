from pyradioconfig.parts.bobcat.calculators.calc_radio import Calc_Radio_Bobcat


class CalcRadioRainier(Calc_Radio_Bobcat):

    def calc_lpfbwtx_reg(self, model):
        pass

    def calc_lnamix_reg(self, model):
        # no PGA block in rainier, replaced with TIA
        self._reg_write(model.vars.RAC_LNAMIXCTRL1_LNAMIXRFPKDTHRESHSELHI, 3)
        self._reg_write(model.vars.RAC_LNAMIXCTRL1_LNAMIXRFPKDTHRESHSELLO, 1)

        self._reg_write(model.vars.RAC_TIACTRL0_TIATHRPKDHISEL, 6)
        self._reg_write(model.vars.RAC_TIACTRL0_TIATHRPKDLOSEL, 2)