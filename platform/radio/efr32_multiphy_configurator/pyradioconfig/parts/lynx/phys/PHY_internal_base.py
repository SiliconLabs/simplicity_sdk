from pycalcmodel.core.output import ModelOutput, ModelOutputType

class Phy_Internal_Base_Lynx(object):
    """
    Common internal Lynx Phy functions live here
    """

    def __init__(self):

        #These are the "Modified Lynx Style" AGC Settings
        self.modified_lynx_style_agc_dict = {
        'AGC_AGCPERIOD_MAXHICNTTHD': 9,
        'AGC_AGCPERIOD_PERIODHI': 18,
        'AGC_AGCPERIOD_PERIODLO': 58,
        'AGC_AGCPERIOD_SETTLETIMEIF': 6,
        'AGC_AGCPERIOD_SETTLETIMERF': 14,
        'AGC_CTRL0_ADCATTENCODE': 0,
        'AGC_CTRL0_ADCATTENMODE': 0,
        'AGC_CTRL0_AGCRST': 0,
        'AGC_CTRL0_DISCFLOOPADJ': 1,
        'AGC_CTRL0_DISRESETCHPWR': 0,
        'AGC_CTRL0_DSADISCFLOOP': 0,
        'AGC_CTRL0_DISPNDWNCOMP': 0,
        'AGC_CTRL0_DISPNGAINUP': 0,
        'AGC_CTRL0_ENRSSIRESET': 0,
        'AGC_CTRL0_MODE': 2,
        'AGC_CTRL0_PWRTARGET': 245,
        'AGC_CTRL1_PWRPERIOD': 1,
        'AGC_CTRL1_RSSIPERIOD': 3,
        'AGC_CTRL1_SUBDEN': 0,
        'AGC_CTRL1_SUBINT': 0,
        'AGC_CTRL1_SUBNUM': 0,
        'AGC_CTRL1_SUBPERIOD': 0,
        'AGC_CTRL2_DEBCNTRST': 0,
        'AGC_CTRL2_DISRFPKD': 0,
        'AGC_CTRL2_DMASEL': 0,
        'AGC_CTRL2_PRSDEBUGEN': 0,
        'AGC_CTRL2_REHICNTTHD': 7,
        'AGC_CTRL2_RELBYCHPWR': 3,
        'AGC_CTRL2_RELOTHD': 4,
        'AGC_CTRL2_RELTARGETPWR': 236,
        'AGC_CTRL2_SAFEMODE': 0,
        'AGC_CTRL2_SAFEMODETHD': 3,
        'AGC_CTRL3_IFPKDDEB': 1,
        'AGC_CTRL3_IFPKDDEBPRD': 40,
        'AGC_CTRL3_IFPKDDEBRST': 10,
        'AGC_CTRL3_IFPKDDEBTHD': 1,
        'AGC_CTRL3_RFPKDDEB': 1,
        'AGC_CTRL3_RFPKDDEBPRD': 40,
        'AGC_CTRL3_RFPKDDEBRST': 10,
        'AGC_CTRL3_RFPKDDEBTHD': 1,
        'AGC_CTRL4_FRZPKDEN': 0,
        'AGC_CTRL4_PERIODRFPKD': 4000,
        'AGC_CTRL4_RFPKDCNTEN': 1,
        'AGC_CTRL4_RFPKDPRDGEAR': 4,
        'AGC_CTRL4_RFPKDSEL': 1,
        'AGC_CTRL4_RFPKDSYNCSEL': 1,
        'AGC_CTRL5_PNUPDISTHD': 48,
        'AGC_CTRL5_PNUPRELTHD': 4,
        'AGC_CTRL6_SEQPNUPALLOW': 0,
        'AGC_CTRL6_SEQRFPKDEN': 0,
        'AGC_GAINRANGE_BOOSTLNA': 1,
        'AGC_GAINRANGE_GAININCSTEP': 1,
        'AGC_GAINRANGE_HIPWRTHD': 3,
        'AGC_GAINRANGE_LATCHEDHISTEP': 0,
        'AGC_GAINRANGE_LNABWADJ': 0,
        'AGC_GAINRANGE_LNAINDEXBORDER': 7,
        'AGC_GAINRANGE_PGAINDEXBORDER': 6,
        'AGC_GAINRANGE_PNGAINSTEP': 3,
        'AGC_GAINSTEPLIM_CFLOOPDEL': 0,
        'AGC_GAINSTEPLIM_CFLOOPSTEPMAX': 4,
        'AGC_GAINSTEPLIM_HYST': 3,
        'AGC_GAINSTEPLIM_MAXPWRVAR': 0,
        'AGC_GAINSTEPLIM_PNINDEXMAX': 14,
        'AGC_GAINSTEPLIM_TRANRSTAGC': 0,
        'AGC_HICNTREGION_HICNTREGION0': 4,
        'AGC_HICNTREGION_HICNTREGION1': 5,
        'AGC_HICNTREGION_HICNTREGION2': 6,
        'AGC_HICNTREGION_HICNTREGION3': 7,
        'AGC_HICNTREGION_HICNTREGION4': 8,
        'AGC_LBT_CCARSSIPERIOD': 0,
        'AGC_LBT_ENCCAGAINREDUCED': 0,
        'AGC_LBT_ENCCARSSIMAX': 0,
        'AGC_LBT_ENCCARSSIPERIOD': 0,
        'AGC_LNAMIXCODE0_LNAMIXSLICE1': 61,
        'AGC_LNAMIXCODE0_LNAMIXSLICE2': 46,
        'AGC_LNAMIXCODE0_LNAMIXSLICE3': 36,
        'AGC_LNAMIXCODE0_LNAMIXSLICE4': 28,
        'AGC_LNAMIXCODE0_LNAMIXSLICE5': 21,
        'AGC_LNAMIXCODE1_LNAMIXSLICE6': 17,
        'AGC_LNAMIXCODE1_LNAMIXSLICE7': 12,
        'AGC_LNAMIXCODE1_LNAMIXSLICE8': 10,
        'AGC_LNAMIXCODE1_LNAMIXSLICE9': 6,
        'AGC_LNAMIXCODE1_LNAMIXSLICE10': 5,
        'AGC_MANGAIN_MANGAINEN': 0,
        'AGC_MANGAIN_MANGAINIFPGA': 7,
        'AGC_MANGAIN_MANGAINLNA': 1,
        'AGC_MANGAIN_MANGAINPN': 1,
        'AGC_MANGAIN_MANIFHILATRST': 0,
        'AGC_MANGAIN_MANIFLOLATRST': 0,
        'AGC_MANGAIN_MANRFLATRST': 0,
        'AGC_STEPDWN_STEPDWN0': 1,
        'AGC_STEPDWN_STEPDWN1': 2,
        'AGC_STEPDWN_STEPDWN2': 3,
        'AGC_STEPDWN_STEPDWN3': 3,
        'AGC_STEPDWN_STEPDWN4': 3,
        'AGC_STEPDWN_STEPDWN5': 5,
        'AGC_PGACODE0_PGAGAIN1': 0,
        'AGC_PGACODE0_PGAGAIN2': 1,
        'AGC_PGACODE0_PGAGAIN3': 2,
        'AGC_PGACODE0_PGAGAIN4': 3,
        'AGC_PGACODE0_PGAGAIN5': 4,
        'AGC_PGACODE0_PGAGAIN6': 5,
        'AGC_PGACODE0_PGAGAIN7': 6,
        'AGC_PGACODE0_PGAGAIN8': 7,
        'AGC_PGACODE1_PGAGAIN9': 8,
        'AGC_PGACODE1_PGAGAIN10': 9,
        'AGC_PGACODE1_PGAGAIN11': 10,
        'AGC_PNRFATT0_LNAMIXRFATT1': 0,
        'AGC_PNRFATT0_LNAMIXRFATT2': 1,
        'AGC_PNRFATT0_LNAMIXRFATT3': 3,
        'AGC_PNRFATT0_LNAMIXRFATT4': 6,
        'AGC_PNRFATT0_LNAMIXRFATT5': 9,
        'AGC_PNRFATT1_LNAMIXRFATT6': 13,
        'AGC_PNRFATT1_LNAMIXRFATT7': 16,
        'AGC_PNRFATT1_LNAMIXRFATT8': 22,
        'AGC_PNRFATT1_LNAMIXRFATT9': 30,
        'AGC_PNRFATT1_LNAMIXRFATT10': 32,
        'AGC_PNRFATT2_LNAMIXRFATT11': 43,
        'AGC_PNRFATT2_LNAMIXRFATT12': 51,
        'AGC_PNRFATT2_LNAMIXRFATT13': 63,
        'AGC_PNRFATT2_LNAMIXRFATT14': 126,
        'AGC_PNRFATT3_LNAMIXRFATT15': 127,
        'AGC_PNRFATT3_LNAMIXRFATT16': 246,
        'AGC_PNRFATT3_LNAMIXRFATT17': 500,
        'AGC_RSSISTEPTHR_RSSIFAST': 0,
        'RAC_PGACTRL_PGAENLATCHI': 1,
        'RAC_PGACTRL_PGAENLATCHQ': 1,
        'RAC_PGACTRL_PGATHRPKDHISEL': 5,
        'RAC_PGACTRL_PGATHRPKDLOSEL': 1,
        'RAC_PGACTRL_LNAMIXRFPKDTHRESHSEL': 2
        }

        # These are the "Modified Panther Style" AGC Settings
        self.modified_panther_style_agc_dict = {
        'AGC_AGCPERIOD_MAXHICNTTHD': 9,
        'AGC_AGCPERIOD_PERIODHI': 14,
        'AGC_AGCPERIOD_PERIODLO': 45,
        'AGC_AGCPERIOD_SETTLETIMEIF': 6,
        'AGC_AGCPERIOD_SETTLETIMERF': 14,
        'AGC_CTRL0_AGCRST': 0,
        'AGC_CTRL0_DISCFLOOPADJ': 1,
        'AGC_CTRL0_DISRESETCHPWR': 0,
        'AGC_CTRL0_DSADISCFLOOP': 0,
        'AGC_CTRL0_DISPNDWNCOMP': 0,
        'AGC_CTRL0_DISPNGAINUP': 0,
        'AGC_CTRL0_ENRSSIRESET': 0,
        'AGC_CTRL0_PWRTARGET': 245,
        'AGC_CTRL1_PWRPERIOD': 1,
        'AGC_CTRL1_RSSIPERIOD': 3,
        'AGC_CTRL1_SUBDEN': 0,
        'AGC_CTRL1_SUBINT': 0,
        'AGC_CTRL1_SUBNUM': 0,
        'AGC_CTRL1_SUBPERIOD': 0,
        'AGC_CTRL2_DISRFPKD': 1,
        'AGC_CTRL2_DMASEL': 0,
        'AGC_CTRL2_PRSDEBUGEN': 0,
        'AGC_CTRL2_REHICNTTHD': 7,
        'AGC_CTRL2_RELBYCHPWR': 3,
        'AGC_CTRL2_RELOTHD': 4,
        'AGC_CTRL2_RELTARGETPWR': 236,
        'AGC_CTRL2_SAFEMODE': 0,
        'AGC_CTRL2_SAFEMODETHD': 3,
        'AGC_CTRL3_IFPKDDEB': 1,
        'AGC_CTRL3_IFPKDDEBPRD': 40,
        'AGC_CTRL3_IFPKDDEBRST': 10,
        'AGC_CTRL3_IFPKDDEBTHD': 1,
        'AGC_CTRL3_RFPKDDEB': 1,
        'AGC_CTRL3_RFPKDDEBPRD': 40,
        'AGC_CTRL3_RFPKDDEBRST': 10,
        'AGC_CTRL3_RFPKDDEBTHD': 1,
        'AGC_CTRL4_FRZPKDEN': 1,
        'AGC_CTRL4_PERIODRFPKD': 4000,
        'AGC_CTRL4_RFPKDCNTEN': 0,
        'AGC_CTRL4_RFPKDPRDGEAR': 4,
        'AGC_CTRL4_RFPKDSEL': 0,
        'AGC_CTRL4_RFPKDSYNCSEL': 0,
        'AGC_CTRL5_PNUPDISTHD': 48,
        'AGC_CTRL5_PNUPRELTHD': 4,
        'AGC_CTRL6_SEQPNUPALLOW': 0,
        'AGC_CTRL6_SEQRFPKDEN': 0,
        'AGC_GAINRANGE_BOOSTLNA': 1,
        'AGC_GAINRANGE_GAININCSTEP': 1,
        'AGC_GAINRANGE_HIPWRTHD': 3,
        'AGC_GAINRANGE_LATCHEDHISTEP': 0,
        'AGC_GAINRANGE_LNABWADJ': 0,
        'AGC_GAINRANGE_LNAINDEXBORDER': 1,
        'AGC_GAINRANGE_PGAINDEXBORDER': 5,
        'AGC_GAINRANGE_PNGAINSTEP': 1,
        'AGC_GAINSTEPLIM_CFLOOPDEL': 0,
        'AGC_GAINSTEPLIM_CFLOOPSTEPMAX': 4,
        'AGC_GAINSTEPLIM_HYST': 3,
        'AGC_GAINSTEPLIM_MAXPWRVAR': 0,
        'AGC_GAINSTEPLIM_PNINDEXMAX': 14,
        'AGC_GAINSTEPLIM_TRANRSTAGC': 0,
        'AGC_HICNTREGION_HICNTREGION0': 4,
        'AGC_HICNTREGION_HICNTREGION1': 5,
        'AGC_HICNTREGION_HICNTREGION2': 6,
        'AGC_HICNTREGION_HICNTREGION3': 7,
        'AGC_HICNTREGION_HICNTREGION4': 8,
        'AGC_LBT_CCARSSIPERIOD': 0,
        'AGC_LBT_ENCCAGAINREDUCED': 0,
        'AGC_LBT_ENCCARSSIMAX': 0,
        'AGC_LBT_ENCCARSSIPERIOD': 0,
        'AGC_LNAMIXCODE0_LNAMIXSLICE1': 61,
        'AGC_LNAMIXCODE0_LNAMIXSLICE2': 46,
        'AGC_LNAMIXCODE0_LNAMIXSLICE3': 36,
        'AGC_LNAMIXCODE0_LNAMIXSLICE4': 28,
        'AGC_LNAMIXCODE0_LNAMIXSLICE5': 21,
        'AGC_LNAMIXCODE1_LNAMIXSLICE6': 17,
        'AGC_LNAMIXCODE1_LNAMIXSLICE7': 12,
        'AGC_LNAMIXCODE1_LNAMIXSLICE8': 10,
        'AGC_LNAMIXCODE1_LNAMIXSLICE9': 6,
        'AGC_LNAMIXCODE1_LNAMIXSLICE10': 5,
        'AGC_MANGAIN_MANGAINEN': 0,
        'AGC_MANGAIN_MANGAINIFPGA': 7,
        'AGC_MANGAIN_MANGAINLNA': 1,
        'AGC_MANGAIN_MANGAINPN': 1,
        'AGC_MANGAIN_MANIFHILATRST': 0,
        'AGC_MANGAIN_MANIFLOLATRST': 0,
        'AGC_MANGAIN_MANRFLATRST': 0,
        'AGC_STEPDWN_STEPDWN0': 1,
        'AGC_STEPDWN_STEPDWN1': 2,
        'AGC_STEPDWN_STEPDWN2': 3,
        'AGC_STEPDWN_STEPDWN3': 3,
        'AGC_STEPDWN_STEPDWN4': 3,
        'AGC_STEPDWN_STEPDWN5': 5,
        'AGC_PGACODE0_PGAGAIN1': 0,
        'AGC_PGACODE0_PGAGAIN2': 1,
        'AGC_PGACODE0_PGAGAIN3': 2,
        'AGC_PGACODE0_PGAGAIN4': 3,
        'AGC_PGACODE0_PGAGAIN5': 4,
        'AGC_PGACODE0_PGAGAIN6': 5,
        'AGC_PGACODE0_PGAGAIN7': 6,
        'AGC_PGACODE0_PGAGAIN8': 7,
        'AGC_PGACODE1_PGAGAIN9': 8,
        'AGC_PGACODE1_PGAGAIN10': 9,
        'AGC_PGACODE1_PGAGAIN11': 10,
        'AGC_PNRFATT0_LNAMIXRFATT1': 0,
        'AGC_PNRFATT0_LNAMIXRFATT2': 1,
        'AGC_PNRFATT0_LNAMIXRFATT3': 3,
        'AGC_PNRFATT0_LNAMIXRFATT4': 6,
        'AGC_PNRFATT0_LNAMIXRFATT5': 9,
        'AGC_PNRFATT1_LNAMIXRFATT6': 13,
        'AGC_PNRFATT1_LNAMIXRFATT7': 16,
        'AGC_PNRFATT1_LNAMIXRFATT8': 22,
        'AGC_PNRFATT1_LNAMIXRFATT9': 30,
        'AGC_PNRFATT1_LNAMIXRFATT10': 32,
        'AGC_PNRFATT2_LNAMIXRFATT11': 43,
        'AGC_PNRFATT2_LNAMIXRFATT12': 51,
        'AGC_PNRFATT2_LNAMIXRFATT13': 63,
        'AGC_PNRFATT2_LNAMIXRFATT14': 126,
        'AGC_PNRFATT3_LNAMIXRFATT15': 127,
        'AGC_PNRFATT3_LNAMIXRFATT16': 246,
        'AGC_PNRFATT3_LNAMIXRFATT17': 500,
        'AGC_RSSISTEPTHR_RSSIFAST': 0,
        'RAC_PGACTRL_PGAENLATCHI': 1,
        'RAC_PGACTRL_PGAENLATCHQ': 1,
        'RAC_PGACTRL_PGATHRPKDHISEL': 5,
        'RAC_PGACTRL_PGATHRPKDLOSEL': 1,
        'RAC_PGACTRL_LNAMIXRFPKDTHRESHSEL': 2
        }

        # These are the "Panther Style" AGC Settings
        self.panther_style_agc_dict = dict(self.modified_panther_style_agc_dict)
        self.panther_style_agc_dict['AGC_CTRL2_DISRFPKD'] = 0
        self.panther_style_agc_dict['AGC_CTRL2_DEBCNTRST'] = 1
        self.panther_style_agc_dict['AGC_CTRL4_FRZPKDEN'] = 0
        self.panther_style_agc_dict['AGC_GAINRANGE_LNAINDEXBORDER'] = 7
        self.panther_style_agc_dict['AGC_GAINRANGE_PGAINDEXBORDER'] = 8

        # This list can prevent writes to deleted registers
        self.agc_fastmode_reg_exclude_list = []

    def AGC_MODIFIED_LYNX_STYLE(self, phy, model):

        register_dict = self.modified_lynx_style_agc_dict
        self.set_reg_overrides_from_dict(phy, register_dict)

    def AGC_MODIFIED_PANTHER_STYLE(self, phy, model):

        register_dict = self.modified_panther_style_agc_dict
        self.set_reg_overrides_from_dict(phy, register_dict)

    def AGC_PANTHER_STYLE(self, phy, model):

        register_dict = self.panther_style_agc_dict
        self.set_reg_overrides_from_dict(phy, register_dict)

    def EN_RFPKD(self, phy, model):

        phy.profile_outputs.AGC_CTRL2_DISRFPKD.override = 0
        phy.profile_outputs.AGC_CTRL4_RFPKDCNTEN.override = 1

    def DIS_RFPKD(self, phy, model):

        phy.profile_outputs.AGC_CTRL2_DISRFPKD.override = 1
        phy.profile_outputs.AGC_CTRL4_RFPKDCNTEN.override = 0

    def set_reg_overrides_from_dict(self,phy, register_dict):
        # Use the dictionary of register:value pairs to create the override statements
        for register in register_dict:
            if register not in self.agc_fastmode_reg_exclude_list:
                register_object = getattr(phy.profile_outputs,register)
                register_object.override = register_dict[register]