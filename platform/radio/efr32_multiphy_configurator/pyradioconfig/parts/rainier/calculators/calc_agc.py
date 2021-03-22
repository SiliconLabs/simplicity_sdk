from pyradioconfig.parts.bobcat.calculators.calc_agc import Calc_AGC_Bobcat
from py_2_and_3_compatibility import *

class CalcAgcRainier(Calc_AGC_Bobcat):

    def calc_agc_series2(self, model):
        # Override method to account for IFPKD settling time
        periodhi_reg = model.vars.AGC_AGCPERIOD0_PERIODHI.value
        periodhistl_reg = model.vars.AGC_AGCPERIOD0_PERIODHISTL.value
        settletimeif_reg = model.vars.AGC_AGCPERIOD0_SETTLETIMEIF.value

        if periodhistl_reg == 0:
            # We start the period counter alongside the IFPKD settling, so we have to acccount for
            # the settling time (blanking period)
            periodhi_usable = periodhi_reg - settletimeif_reg
        else:
            periodhi_usable = periodhi_reg

        # % There are many possible ways to handle the table but the simplest would be:
        # % this is based on sine wave tripping N times at different gain settings
        self._reg_write(model.vars.AGC_STEPDWN_STEPDWN0, 0)
        self._reg_write(model.vars.AGC_STEPDWN_STEPDWN1, 1)
        self._reg_write(model.vars.AGC_STEPDWN_STEPDWN2, 2)
        self._reg_write(model.vars.AGC_STEPDWN_STEPDWN3, 3)
        self._reg_write(model.vars.AGC_STEPDWN_STEPDWN4, 4)
        self._reg_write(model.vars.AGC_STEPDWN_STEPDWN5, 5)

        hicntregion0 = int(py2round(0.55 * periodhi_usable))
        hicntregion1 = int(py2round(0.75 * periodhi_usable))
        hicntregion2 = int(py2round(0.85 * periodhi_usable))
        hicntregion3 = int(py2round(0.90 * periodhi_usable))
        hicntregion4 = int(py2round(0.93 * periodhi_usable))

        if (hicntregion0 > 255):
            print("  WARNING: AGC_HICNTREGION_HICNTREGION 0 calculated beyond range: hicntregion0 {}, hicntregion1 {}, AGC_AGCPERIOD0_PERIODHI {}. Saturating value to 255 !".format(hicntregion0, hicntregion1, periodhi))
            hicntregion0 = 255

        if (hicntregion1 > 255):
            print("  WARNING: AGC_HICNTREGION_HICNTREGION 1 calculated beyond range: hicntregion0 {}, hicntregion1 {}, AGC_AGCPERIOD0_PERIODHI {}. Saturating value to 255 !".format(hicntregion0, hicntregion1, periodhi))
            hicntregion1 = 255

        self._reg_write(model.vars.AGC_HICNTREGION0_HICNTREGION0, hicntregion0)
        self._reg_write(model.vars.AGC_HICNTREGION0_HICNTREGION1, hicntregion1)
        self._reg_write(model.vars.AGC_HICNTREGION0_HICNTREGION2, hicntregion2)
        self._reg_write(model.vars.AGC_HICNTREGION0_HICNTREGION3, hicntregion3)
        self._reg_write(model.vars.AGC_HICNTREGION1_HICNTREGION4, hicntregion4)

        # % safe way of setting this.
        self._reg_write(model.vars.AGC_AGCPERIOD0_MAXHICNTTHD, hicntregion4)

    def calc_rssiperiod_val(self, model):
        rssi_period_val = 3
        model.vars.rssi_period.value = rssi_period_val

    def calc_antdiv_gainmode_reg(self, model):
        self._reg_write(model.vars.AGC_ANTDIV_GAINMODE, 0)

    def calc_antdiv_debouncecntthd(self, model):
        #This is do not care (functionality disabled)
        self._reg_do_not_care(model.vars.AGC_ANTDIV_DEBOUNCECNTTHD)

    # override Bobcat calc_frzpkden_reg (register has been removed in Rainier)
    # Also removed in common/profiles/lynx_regs.py for Rainier only
    def calc_frzpkden_reg(self, model):
        pass

    def calc_gain_schedule_regs(self, model):
        lnaindexborder = 5 # Pre-silicon from Chris 10/23/23
        pgaindexborder = 4 # Pre-silicon from Chris 10/23/23
        pnindexborder = 0  # New for Series3
                           # 0: RFPAD at end of RFPKD schedule
                           # 1: RFPAD at end of IFPKD schedule

        self._reg_write(model.vars.AGC_GAINRANGE_LNAINDEXBORDER, lnaindexborder)
        self._reg_write(model.vars.AGC_GAINRANGE_PGAINDEXBORDER, pgaindexborder)
        self._reg_write(model.vars.AGC_GAINRANGE_PNINDEXBORDER, pnindexborder)

    def calc_lnamixslice_reg(self, model):
        # From Chris Calvo
        lnaindexmax = 9
        self._reg_write(model.vars.AGC_LNAMIXCODE0_LNAMIXSLICE1,46)
        self._reg_write(model.vars.AGC_LNAMIXCODE0_LNAMIXSLICE2,34)
        self._reg_write(model.vars.AGC_LNAMIXCODE0_LNAMIXSLICE3,16)
        self._reg_write(model.vars.AGC_LNAMIXCODE0_LNAMIXSLICE4,12)
        self._reg_write(model.vars.AGC_LNAMIXCODE0_LNAMIXSLICE5,9)
        self._reg_write(model.vars.AGC_LNAMIXCODE1_LNAMIXSLICE6,7)
        self._reg_write(model.vars.AGC_LNAMIXCODE1_LNAMIXSLICE7,5)
        self._reg_write(model.vars.AGC_LNAMIXCODE1_LNAMIXSLICE8,3)
        self._reg_write(model.vars.AGC_LNAMIXCODE1_LNAMIXSLICE9,2)
        self._reg_write_default(model.vars.AGC_LNAMIXCODE1_LNAMIXSLICE10)
        self._reg_write(model.vars.AGC_GAINSTEPLIM1_LNAINDEXMAX, lnaindexmax)


    def calc_lnamixrfatt_reg(self, model):

        #Values from Chris Calvo 10/5/23
        lnamixrfatt = [0, 3, 6, 10, 16, 21, 29, 33, 47, 63, 113, 159, 255, 1023, 2047, 2047]
        pnindexmax = 15 # Set to 15 to include RF PAD

        #Write registers
        self._reg_write(model.vars.AGC_PNRFATT0_LNAMIXRFATT1, lnamixrfatt[0])
        self._reg_write(model.vars.AGC_PNRFATT0_LNAMIXRFATT2, lnamixrfatt[1])
        self._reg_write(model.vars.AGC_PNRFATT1_LNAMIXRFATT3, lnamixrfatt[2])
        self._reg_write(model.vars.AGC_PNRFATT1_LNAMIXRFATT4, lnamixrfatt[3])
        self._reg_write(model.vars.AGC_PNRFATT2_LNAMIXRFATT5, lnamixrfatt[4])
        self._reg_write(model.vars.AGC_PNRFATT2_LNAMIXRFATT6, lnamixrfatt[5])
        self._reg_write(model.vars.AGC_PNRFATT3_LNAMIXRFATT7, lnamixrfatt[6])
        self._reg_write(model.vars.AGC_PNRFATT3_LNAMIXRFATT8, lnamixrfatt[7])
        self._reg_write(model.vars.AGC_PNRFATT4_LNAMIXRFATT9, lnamixrfatt[8])
        self._reg_write(model.vars.AGC_PNRFATT4_LNAMIXRFATT10, lnamixrfatt[9])
        self._reg_write(model.vars.AGC_PNRFATT5_LNAMIXRFATT11, lnamixrfatt[10])
        self._reg_write(model.vars.AGC_PNRFATT5_LNAMIXRFATT12, lnamixrfatt[11])
        self._reg_write(model.vars.AGC_PNRFATT6_LNAMIXRFATT13, lnamixrfatt[12])
        self._reg_write(model.vars.AGC_PNRFATT6_LNAMIXRFATT14, lnamixrfatt[13])
        self._reg_write(model.vars.AGC_PNRFATT7_LNAMIXRFATT15, lnamixrfatt[14])
        self._reg_write(model.vars.AGC_PNRFATT7_LNAMIXRFATT16, lnamixrfatt[15])
        self._reg_write(model.vars.AGC_GAINSTEPLIM1_PNINDEXMAX, pnindexmax)

    def calc_pngaindb_reg(self, model):
        # New in Rainier - initializing to reset values
        # 8-bit signed values including 2-bit fraction
        self._reg_write(model.vars.AGC_PNGAIN1_PNGAINDB1, 38)
        self._reg_write(model.vars.AGC_PNGAIN1_PNGAINDB2, 30)
        self._reg_write(model.vars.AGC_PNGAIN1_PNGAINDB3, 23)
        self._reg_write(model.vars.AGC_PNGAIN1_PNGAINDB4, 16)
        self._reg_write(model.vars.AGC_PNGAIN2_PNGAINDB5, 4)
        self._reg_write(model.vars.AGC_PNGAIN2_PNGAINDB6, 255)
        self._reg_write(model.vars.AGC_PNGAIN2_PNGAINDB7, 248)
        self._reg_write(model.vars.AGC_PNGAIN2_PNGAINDB8, 240)
        self._reg_write(model.vars.AGC_PNGAIN3_PNGAINDB9, 232)
        self._reg_write(model.vars.AGC_PNGAIN3_PNGAINDB10, 224)
        self._reg_write(model.vars.AGC_PNGAIN3_PNGAINDB11, 215)
        self._reg_write(model.vars.AGC_PNGAIN3_PNGAINDB12, 206)
        self._reg_write(model.vars.AGC_PNGAIN4_PNGAINDB13, 197)
        self._reg_write(model.vars.AGC_PNGAIN4_PNGAINDB14, 189)
        self._reg_write(model.vars.AGC_PNGAIN4_PNGAINDB15, 181)
        self._reg_write(model.vars.AGC_PNGAIN4_PNGAINDB16, 181)


    def calc_lnagaindb_reg(self, model):
        # New in Rainier - initializing to reset values
        # 8-bit signed values including 2-bit fraction
        self._reg_write(model.vars.AGC_LNAGAIN1_LNAGAINDB1, 36)
        self._reg_write(model.vars.AGC_LNAGAIN1_LNAGAINDB2, 28)
        self._reg_write(model.vars.AGC_LNAGAIN1_LNAGAINDB3, 20)
        self._reg_write(model.vars.AGC_LNAGAIN1_LNAGAINDB4, 12)
        self._reg_write(model.vars.AGC_LNAGAIN2_LNAGAINDB5, 4)
        self._reg_write(model.vars.AGC_LNAGAIN2_LNAGAINDB6, 252)
        self._reg_write(model.vars.AGC_LNAGAIN2_LNAGAINDB7, 244)
        self._reg_write(model.vars.AGC_LNAGAIN2_LNAGAINDB8, 235)
        self._reg_write(model.vars.AGC_LNAGAIN3_LNAGAINDB9, 235)
        self._reg_write(model.vars.AGC_LNAGAIN3_LNAGAINDB10, 235)

    def calc_lnamixcuren_reg(self, model):
        # Corresponds to lnamix_doublecur1_en
        # LSB->agcindex1, MSB->agcindex10
        lnamixcurr = 0b1111111100

        self._reg_write(model.vars.AGC_LNAMIXCODE2_LNAMIXCUREN, lnamixcurr)

    def calc_tiacomp_reg(self, model):
        # New in Rainier
        self._reg_write(model.vars.AGC_TIACODE0_TIACOMP1, 0)
        self._reg_write(model.vars.AGC_TIACODE0_TIACOMP2, 0)
        self._reg_write(model.vars.AGC_TIACODE0_TIACOMP3, 0)
        self._reg_write(model.vars.AGC_TIACODE0_TIACOMP4, 0)
        self._reg_write(model.vars.AGC_TIACODE0_TIACOMP5, 0)
        self._reg_write(model.vars.AGC_TIACODE0_TIACOMP6, 2)
        self._reg_write(model.vars.AGC_TIACODE1_TIACOMP7, 2)
        self._reg_write(model.vars.AGC_TIACODE1_TIACOMP8, 2)
        self._reg_write(model.vars.AGC_TIACODE1_TIACOMP9, 2)
        self._reg_write(model.vars.AGC_TIACODE1_TIACOMP10, 4)
        self._reg_write(model.vars.AGC_TIACODE1_TIACOMP11, 4)

    def calc_tia_capfb_reg(self, model):
        # New in Rainier
        # for rainier there will be only one BW supported (the PHYs only need 5MHz)
        # these values are currently being written by us and are set as per the confluence page
        # https://confluence.silabs.com/display/IPEM22NM/Rainier+Radio-RX+AGC+Schedule
        # but will eventually come from PTE in phase 3
        self._reg_write(model.vars.AGC_TIACODE2_TIACAPFB1, 16)
        self._reg_write(model.vars.AGC_TIACODE2_TIACAPFB2, 20)
        self._reg_write(model.vars.AGC_TIACODE2_TIACAPFB3, 26)
        self._reg_write(model.vars.AGC_TIACODE2_TIACAPFB4, 33)
        self._reg_write(model.vars.AGC_TIACODE3_TIACAPFB5, 41)
        self._reg_write(model.vars.AGC_TIACODE3_TIACAPFB6, 52)
        self._reg_write(model.vars.AGC_TIACODE3_TIACAPFB7, 64)
        self._reg_write(model.vars.AGC_TIACODE3_TIACAPFB8, 81)
        self._reg_write(model.vars.AGC_TIACODE4_TIACAPFB9, 103)
        self._reg_write(model.vars.AGC_TIACODE4_TIACAPFB10, 126)
        self._reg_write(model.vars.AGC_TIACODE4_TIACAPFB11, 155)

    def calc_pgagaindb_reg(self, model):
        # New in Rainier - initializing to reset values
        # 9-bit signed values including 2-bit fraction
        self._reg_write(model.vars.AGC_PGAGAIN1_PGAGAINDB1, 136)
        self._reg_write(model.vars.AGC_PGAGAIN1_PGAGAINDB2, 128)
        self._reg_write(model.vars.AGC_PGAGAIN1_PGAGAINDB3, 120)
        self._reg_write(model.vars.AGC_PGAGAIN2_PGAGAINDB4, 112)
        self._reg_write(model.vars.AGC_PGAGAIN2_PGAGAINDB5, 104)
        self._reg_write(model.vars.AGC_PGAGAIN2_PGAGAINDB6, 96)
        self._reg_write(model.vars.AGC_PGAGAIN3_PGAGAINDB7, 88)
        self._reg_write(model.vars.AGC_PGAGAIN3_PGAGAINDB8, 80)
        self._reg_write(model.vars.AGC_PGAGAIN3_PGAGAINDB9, 72)
        self._reg_write(model.vars.AGC_PGAGAIN4_PGAGAINDB10, 64)
        self._reg_write(model.vars.AGC_PGAGAIN4_PGAGAINDB11, 56)

    def calc_periodhistl_reg(self, model):
        # New in Rainier
        self._reg_write(model.vars.AGC_AGCPERIOD0_PERIODHISTL, 1)

    def calc_periodlostl_reg(self, model):
        # New in Rainier
        self._reg_write(model.vars.AGC_AGCPERIOD0_PERIODLOSTL, 1)

    def calc_agc_misc(self, model):
        # copied from ocelot, removed deprecated registers
        self._reg_write(model.vars.AGC_AGCPERIOD0_SETTLETIMEIF,6)
        self._reg_write(model.vars.AGC_AGCPERIOD0_SETTLETIMERF,14)
        self._reg_write(model.vars.AGC_CTRL0_AGCRST,0)
        self._reg_write(model.vars.AGC_CTRL0_DISCFLOOPADJ,1)
        self._reg_write(model.vars.AGC_CTRL0_DISRESETCHPWR,0)
        self._reg_write(model.vars.AGC_CTRL0_DSADISCFLOOP,0)
        self._reg_write(model.vars.AGC_CTRL0_DISPNDWNCOMP,0)
        self._reg_write(model.vars.AGC_CTRL0_DISPNGAINUP,0)
        self._reg_write(model.vars.AGC_CTRL0_ENRSSIRESET,0)
        self._reg_write(model.vars.AGC_CTRL1_PWRPERIOD, 1)
        self._reg_write(model.vars.AGC_CTRL2_DISRFPKD,0)
        self._reg_write(model.vars.AGC_CTRL2_DMASEL,0)
        self._reg_write(model.vars.AGC_CTRL2_PRSDEBUGEN,0)
        self._reg_write(model.vars.AGC_CTRL2_REHICNTTHD,7)
        self._reg_write(model.vars.AGC_CTRL2_RELBYCHPWR,3)
        self._reg_write(model.vars.AGC_CTRL2_RELOTHD,4)
        self._reg_write(model.vars.AGC_CTRL2_RELTARGETPWR,236)
        self._reg_write(model.vars.AGC_CTRL2_SAFEMODE,0)
        self._reg_write(model.vars.AGC_CTRL2_SAFEMODETHD,3)
        self._reg_write(model.vars.AGC_CTRL3_IFPKDDEB,1)
        self._reg_write(model.vars.AGC_CTRL3_IFPKDDEBPRD,40)
        self._reg_write(model.vars.AGC_CTRL3_IFPKDDEBRST,10)
        self._reg_write(model.vars.AGC_CTRL3_IFPKDDEBTHD,1)
        self._reg_write(model.vars.AGC_CTRL3_RFPKDDEB,1)
        self._reg_write(model.vars.AGC_CTRL3_RFPKDDEBPRD,40)
        self._reg_write(model.vars.AGC_CTRL3_RFPKDDEBRST,10)
        self._reg_write(model.vars.AGC_CTRL3_RFPKDDEBTHD,1)
        self._reg_write(model.vars.AGC_CTRL4_PERIODRFPKD,4000)
        self._reg_write(model.vars.AGC_CTRL4_RFPKDCNTEN,1)
        self._reg_write(model.vars.AGC_CTRL4_RFPKDPRDGEAR,4)
        self._reg_write(model.vars.AGC_CTRL4_RFPKDSEL,1)
        self._reg_write(model.vars.AGC_CTRL4_RFPKDSYNCSEL,1)
        self._reg_write(model.vars.AGC_CTRL5_PNUPRELTHD,4)
        self._reg_write(model.vars.AGC_CTRL5_SEQPNUPALLOW,0)
        self._reg_write(model.vars.AGC_CTRL5_SEQRFPKDEN,0)
        self._reg_write(model.vars.AGC_GAINRANGE_GAININCSTEP,1)
        self._reg_write(model.vars.AGC_GAINRANGE_HIPWRTHD,3)
        self._reg_write(model.vars.AGC_GAINRANGE_LATCHEDHISTEP,0)
        self._reg_write(model.vars.AGC_GAINSTEPLIM0_MAXPWRVAR,0)
        self._reg_write(model.vars.AGC_GAINSTEPLIM0_TRANRSTAGC,0)
        self._reg_write(model.vars.AGC_LBT_CCARSSIPERIOD,0)
        self._reg_write(model.vars.AGC_LBT_ENCCAGAINREDUCED,0)
        self._reg_write(model.vars.AGC_LBT_ENCCARSSIMAX,0)
        self._reg_write(model.vars.AGC_LBT_ENCCARSSIPERIOD,0)
        self._reg_write(model.vars.AGC_MANGAIN_MANGAINEN,0)
        self._reg_write(model.vars.AGC_MANGAIN_MANGAINIFPGA,0)
        self._reg_write(model.vars.AGC_MANGAIN_MANGAINLNA,0)
        self._reg_write(model.vars.AGC_MANGAIN_MANGAINPN,0)
        self._reg_write(model.vars.AGC_MANGAIN_MANIFHILATRST,0)
        self._reg_write(model.vars.AGC_MANGAIN_MANIFLOLATRST,0)
        self._reg_write(model.vars.AGC_MANGAIN_MANRFLATRST,0)
        self._reg_write(model.vars.AGC_PGACODE0_PGAGAIN1,0)
        self._reg_write(model.vars.AGC_PGACODE0_PGAGAIN2,1)
        self._reg_write(model.vars.AGC_PGACODE0_PGAGAIN3,2)
        self._reg_write(model.vars.AGC_PGACODE0_PGAGAIN4,3)
        self._reg_write(model.vars.AGC_PGACODE0_PGAGAIN5,4)
        self._reg_write(model.vars.AGC_PGACODE0_PGAGAIN6,5)
        self._reg_write(model.vars.AGC_PGACODE0_PGAGAIN7,6)
        self._reg_write(model.vars.AGC_PGACODE0_PGAGAIN8,7)
        self._reg_write(model.vars.AGC_PGACODE1_PGAGAIN9,8)
        self._reg_write(model.vars.AGC_PGACODE1_PGAGAIN10,9)
        self._reg_write(model.vars.AGC_PGACODE1_PGAGAIN11,10)
        self._reg_write(model.vars.AGC_RSSISTEPTHR_RSSIFAST,0)
        self._reg_write(model.vars.AGC_GAINSTEPLIM0_CFLOOPSTEPMAX, 0)
        self._reg_write(model.vars.AGC_GAINSTEPLIM0_HYST, 0)