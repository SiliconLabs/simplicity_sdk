
# -*- coding: utf-8 -*-

__all__ = [ 'RM_Peripheral_AGC_NS' ]

from . static import Base_RM_Peripheral
from . AGC_NS_register import *

class RM_Peripheral_AGC_NS(Base_RM_Peripheral):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Peripheral_AGC_NS, self).__init__(rmio, label,
            0xB800C000, 'AGC_NS',
            u"",
            [])
        self.IPVERSION = RM_Register_AGC_NS_IPVERSION(self.zz_rmio, self.zz_label)
        self.zz_rdict['IPVERSION'] = self.IPVERSION
        self.EN = RM_Register_AGC_NS_EN(self.zz_rmio, self.zz_label)
        self.zz_rdict['EN'] = self.EN
        self.STATUS0 = RM_Register_AGC_NS_STATUS0(self.zz_rmio, self.zz_label)
        self.zz_rdict['STATUS0'] = self.STATUS0
        self.STATUS1 = RM_Register_AGC_NS_STATUS1(self.zz_rmio, self.zz_label)
        self.zz_rdict['STATUS1'] = self.STATUS1
        self.STATUS2 = RM_Register_AGC_NS_STATUS2(self.zz_rmio, self.zz_label)
        self.zz_rdict['STATUS2'] = self.STATUS2
        self.RSSI = RM_Register_AGC_NS_RSSI(self.zz_rmio, self.zz_label)
        self.zz_rdict['RSSI'] = self.RSSI
        self.FRAMERSSI = RM_Register_AGC_NS_FRAMERSSI(self.zz_rmio, self.zz_label)
        self.zz_rdict['FRAMERSSI'] = self.FRAMERSSI
        self.CTRL0 = RM_Register_AGC_NS_CTRL0(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL0'] = self.CTRL0
        self.CTRL1 = RM_Register_AGC_NS_CTRL1(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL1'] = self.CTRL1
        self.CTRL2 = RM_Register_AGC_NS_CTRL2(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL2'] = self.CTRL2
        self.CTRL3 = RM_Register_AGC_NS_CTRL3(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL3'] = self.CTRL3
        self.CTRL4 = RM_Register_AGC_NS_CTRL4(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL4'] = self.CTRL4
        self.CTRL5 = RM_Register_AGC_NS_CTRL5(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL5'] = self.CTRL5
        self.CTRL6 = RM_Register_AGC_NS_CTRL6(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL6'] = self.CTRL6
        self.CTRL7 = RM_Register_AGC_NS_CTRL7(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL7'] = self.CTRL7
        self.RSSISTEPTHR = RM_Register_AGC_NS_RSSISTEPTHR(self.zz_rmio, self.zz_label)
        self.zz_rdict['RSSISTEPTHR'] = self.RSSISTEPTHR
        self.MANGAIN = RM_Register_AGC_NS_MANGAIN(self.zz_rmio, self.zz_label)
        self.zz_rdict['MANGAIN'] = self.MANGAIN
        self.IF = RM_Register_AGC_NS_IF(self.zz_rmio, self.zz_label)
        self.zz_rdict['IF'] = self.IF
        self.IEN = RM_Register_AGC_NS_IEN(self.zz_rmio, self.zz_label)
        self.zz_rdict['IEN'] = self.IEN
        self.CMD = RM_Register_AGC_NS_CMD(self.zz_rmio, self.zz_label)
        self.zz_rdict['CMD'] = self.CMD
        self.GAINRANGE = RM_Register_AGC_NS_GAINRANGE(self.zz_rmio, self.zz_label)
        self.zz_rdict['GAINRANGE'] = self.GAINRANGE
        self.AGCPERIOD0 = RM_Register_AGC_NS_AGCPERIOD0(self.zz_rmio, self.zz_label)
        self.zz_rdict['AGCPERIOD0'] = self.AGCPERIOD0
        self.AGCPERIOD1 = RM_Register_AGC_NS_AGCPERIOD1(self.zz_rmio, self.zz_label)
        self.zz_rdict['AGCPERIOD1'] = self.AGCPERIOD1
        self.HICNTREGION0 = RM_Register_AGC_NS_HICNTREGION0(self.zz_rmio, self.zz_label)
        self.zz_rdict['HICNTREGION0'] = self.HICNTREGION0
        self.HICNTREGION1 = RM_Register_AGC_NS_HICNTREGION1(self.zz_rmio, self.zz_label)
        self.zz_rdict['HICNTREGION1'] = self.HICNTREGION1
        self.STEPDWN = RM_Register_AGC_NS_STEPDWN(self.zz_rmio, self.zz_label)
        self.zz_rdict['STEPDWN'] = self.STEPDWN
        self.GAINSTEPLIM0 = RM_Register_AGC_NS_GAINSTEPLIM0(self.zz_rmio, self.zz_label)
        self.zz_rdict['GAINSTEPLIM0'] = self.GAINSTEPLIM0
        self.GAINSTEPLIM1 = RM_Register_AGC_NS_GAINSTEPLIM1(self.zz_rmio, self.zz_label)
        self.zz_rdict['GAINSTEPLIM1'] = self.GAINSTEPLIM1
        self.PNRFATT0 = RM_Register_AGC_NS_PNRFATT0(self.zz_rmio, self.zz_label)
        self.zz_rdict['PNRFATT0'] = self.PNRFATT0
        self.PNRFATT1 = RM_Register_AGC_NS_PNRFATT1(self.zz_rmio, self.zz_label)
        self.zz_rdict['PNRFATT1'] = self.PNRFATT1
        self.PNRFATT2 = RM_Register_AGC_NS_PNRFATT2(self.zz_rmio, self.zz_label)
        self.zz_rdict['PNRFATT2'] = self.PNRFATT2
        self.PNRFATT3 = RM_Register_AGC_NS_PNRFATT3(self.zz_rmio, self.zz_label)
        self.zz_rdict['PNRFATT3'] = self.PNRFATT3
        self.PNRFATT4 = RM_Register_AGC_NS_PNRFATT4(self.zz_rmio, self.zz_label)
        self.zz_rdict['PNRFATT4'] = self.PNRFATT4
        self.PNRFATT5 = RM_Register_AGC_NS_PNRFATT5(self.zz_rmio, self.zz_label)
        self.zz_rdict['PNRFATT5'] = self.PNRFATT5
        self.PNRFATT6 = RM_Register_AGC_NS_PNRFATT6(self.zz_rmio, self.zz_label)
        self.zz_rdict['PNRFATT6'] = self.PNRFATT6
        self.PNRFATT7 = RM_Register_AGC_NS_PNRFATT7(self.zz_rmio, self.zz_label)
        self.zz_rdict['PNRFATT7'] = self.PNRFATT7
        self.PNRFATTALT = RM_Register_AGC_NS_PNRFATTALT(self.zz_rmio, self.zz_label)
        self.zz_rdict['PNRFATTALT'] = self.PNRFATTALT
        self.LNAMIXCODE0 = RM_Register_AGC_NS_LNAMIXCODE0(self.zz_rmio, self.zz_label)
        self.zz_rdict['LNAMIXCODE0'] = self.LNAMIXCODE0
        self.LNAMIXCODE1 = RM_Register_AGC_NS_LNAMIXCODE1(self.zz_rmio, self.zz_label)
        self.zz_rdict['LNAMIXCODE1'] = self.LNAMIXCODE1
        self.PGACODE0 = RM_Register_AGC_NS_PGACODE0(self.zz_rmio, self.zz_label)
        self.zz_rdict['PGACODE0'] = self.PGACODE0
        self.PGACODE1 = RM_Register_AGC_NS_PGACODE1(self.zz_rmio, self.zz_label)
        self.zz_rdict['PGACODE1'] = self.PGACODE1
        self.LBT = RM_Register_AGC_NS_LBT(self.zz_rmio, self.zz_label)
        self.zz_rdict['LBT'] = self.LBT
        self.MIRRORIF = RM_Register_AGC_NS_MIRRORIF(self.zz_rmio, self.zz_label)
        self.zz_rdict['MIRRORIF'] = self.MIRRORIF
        self.SEQIF = RM_Register_AGC_NS_SEQIF(self.zz_rmio, self.zz_label)
        self.zz_rdict['SEQIF'] = self.SEQIF
        self.SEQIEN = RM_Register_AGC_NS_SEQIEN(self.zz_rmio, self.zz_label)
        self.zz_rdict['SEQIEN'] = self.SEQIEN
        self.RSSIABSTHR = RM_Register_AGC_NS_RSSIABSTHR(self.zz_rmio, self.zz_label)
        self.zz_rdict['RSSIABSTHR'] = self.RSSIABSTHR
        self.LNABOOST = RM_Register_AGC_NS_LNABOOST(self.zz_rmio, self.zz_label)
        self.zz_rdict['LNABOOST'] = self.LNABOOST
        self.ANTDIV = RM_Register_AGC_NS_ANTDIV(self.zz_rmio, self.zz_label)
        self.zz_rdict['ANTDIV'] = self.ANTDIV
        self.DUALRFPKDTHD0 = RM_Register_AGC_NS_DUALRFPKDTHD0(self.zz_rmio, self.zz_label)
        self.zz_rdict['DUALRFPKDTHD0'] = self.DUALRFPKDTHD0
        self.DUALRFPKDTHD1 = RM_Register_AGC_NS_DUALRFPKDTHD1(self.zz_rmio, self.zz_label)
        self.zz_rdict['DUALRFPKDTHD1'] = self.DUALRFPKDTHD1
        self.SPARE = RM_Register_AGC_NS_SPARE(self.zz_rmio, self.zz_label)
        self.zz_rdict['SPARE'] = self.SPARE
        self.PNRFFILT0 = RM_Register_AGC_NS_PNRFFILT0(self.zz_rmio, self.zz_label)
        self.zz_rdict['PNRFFILT0'] = self.PNRFFILT0
        self.PNRFFILT1 = RM_Register_AGC_NS_PNRFFILT1(self.zz_rmio, self.zz_label)
        self.zz_rdict['PNRFFILT1'] = self.PNRFFILT1
        self.PNRFFILT2 = RM_Register_AGC_NS_PNRFFILT2(self.zz_rmio, self.zz_label)
        self.zz_rdict['PNRFFILT2'] = self.PNRFFILT2
        self.PNRFFILT3 = RM_Register_AGC_NS_PNRFFILT3(self.zz_rmio, self.zz_label)
        self.zz_rdict['PNRFFILT3'] = self.PNRFFILT3
        self.PNRFFILT4 = RM_Register_AGC_NS_PNRFFILT4(self.zz_rmio, self.zz_label)
        self.zz_rdict['PNRFFILT4'] = self.PNRFFILT4
        self.PNRFFILT5 = RM_Register_AGC_NS_PNRFFILT5(self.zz_rmio, self.zz_label)
        self.zz_rdict['PNRFFILT5'] = self.PNRFFILT5
        self.PNRFFILT6 = RM_Register_AGC_NS_PNRFFILT6(self.zz_rmio, self.zz_label)
        self.zz_rdict['PNRFFILT6'] = self.PNRFFILT6
        self.PNRFFILT7 = RM_Register_AGC_NS_PNRFFILT7(self.zz_rmio, self.zz_label)
        self.zz_rdict['PNRFFILT7'] = self.PNRFFILT7
        self.FENOTCHATT0 = RM_Register_AGC_NS_FENOTCHATT0(self.zz_rmio, self.zz_label)
        self.zz_rdict['FENOTCHATT0'] = self.FENOTCHATT0
        self.FENOTCHATT1 = RM_Register_AGC_NS_FENOTCHATT1(self.zz_rmio, self.zz_label)
        self.zz_rdict['FENOTCHATT1'] = self.FENOTCHATT1
        self.FENOTCHATT2 = RM_Register_AGC_NS_FENOTCHATT2(self.zz_rmio, self.zz_label)
        self.zz_rdict['FENOTCHATT2'] = self.FENOTCHATT2
        self.FENOTCHATT3 = RM_Register_AGC_NS_FENOTCHATT3(self.zz_rmio, self.zz_label)
        self.zz_rdict['FENOTCHATT3'] = self.FENOTCHATT3
        self.FENOTCHATT4 = RM_Register_AGC_NS_FENOTCHATT4(self.zz_rmio, self.zz_label)
        self.zz_rdict['FENOTCHATT4'] = self.FENOTCHATT4
        self.FENOTCHATT5 = RM_Register_AGC_NS_FENOTCHATT5(self.zz_rmio, self.zz_label)
        self.zz_rdict['FENOTCHATT5'] = self.FENOTCHATT5
        self.FENOTCHATT6 = RM_Register_AGC_NS_FENOTCHATT6(self.zz_rmio, self.zz_label)
        self.zz_rdict['FENOTCHATT6'] = self.FENOTCHATT6
        self.FENOTCHATT7 = RM_Register_AGC_NS_FENOTCHATT7(self.zz_rmio, self.zz_label)
        self.zz_rdict['FENOTCHATT7'] = self.FENOTCHATT7
        self.FENOTCHATT8 = RM_Register_AGC_NS_FENOTCHATT8(self.zz_rmio, self.zz_label)
        self.zz_rdict['FENOTCHATT8'] = self.FENOTCHATT8
        self.FENOTCHATT9 = RM_Register_AGC_NS_FENOTCHATT9(self.zz_rmio, self.zz_label)
        self.zz_rdict['FENOTCHATT9'] = self.FENOTCHATT9
        self.FENOTCHATT10 = RM_Register_AGC_NS_FENOTCHATT10(self.zz_rmio, self.zz_label)
        self.zz_rdict['FENOTCHATT10'] = self.FENOTCHATT10
        self.FENOTCHATT11 = RM_Register_AGC_NS_FENOTCHATT11(self.zz_rmio, self.zz_label)
        self.zz_rdict['FENOTCHATT11'] = self.FENOTCHATT11
        self.FENOTCHFILT0 = RM_Register_AGC_NS_FENOTCHFILT0(self.zz_rmio, self.zz_label)
        self.zz_rdict['FENOTCHFILT0'] = self.FENOTCHFILT0
        self.FENOTCHFILT1 = RM_Register_AGC_NS_FENOTCHFILT1(self.zz_rmio, self.zz_label)
        self.zz_rdict['FENOTCHFILT1'] = self.FENOTCHFILT1
        self.FENOTCHFILT2 = RM_Register_AGC_NS_FENOTCHFILT2(self.zz_rmio, self.zz_label)
        self.zz_rdict['FENOTCHFILT2'] = self.FENOTCHFILT2
        self.FENOTCHFILT3 = RM_Register_AGC_NS_FENOTCHFILT3(self.zz_rmio, self.zz_label)
        self.zz_rdict['FENOTCHFILT3'] = self.FENOTCHFILT3
        self.FENOTCHFILT4 = RM_Register_AGC_NS_FENOTCHFILT4(self.zz_rmio, self.zz_label)
        self.zz_rdict['FENOTCHFILT4'] = self.FENOTCHFILT4
        self.FENOTCHFILT5 = RM_Register_AGC_NS_FENOTCHFILT5(self.zz_rmio, self.zz_label)
        self.zz_rdict['FENOTCHFILT5'] = self.FENOTCHFILT5
        self.FENOTCHFILT6 = RM_Register_AGC_NS_FENOTCHFILT6(self.zz_rmio, self.zz_label)
        self.zz_rdict['FENOTCHFILT6'] = self.FENOTCHFILT6
        self.FENOTCHFILT7 = RM_Register_AGC_NS_FENOTCHFILT7(self.zz_rmio, self.zz_label)
        self.zz_rdict['FENOTCHFILT7'] = self.FENOTCHFILT7
        self.FENOTCHFILT8 = RM_Register_AGC_NS_FENOTCHFILT8(self.zz_rmio, self.zz_label)
        self.zz_rdict['FENOTCHFILT8'] = self.FENOTCHFILT8
        self.FENOTCHFILT9 = RM_Register_AGC_NS_FENOTCHFILT9(self.zz_rmio, self.zz_label)
        self.zz_rdict['FENOTCHFILT9'] = self.FENOTCHFILT9
        self.FENOTCHFILT10 = RM_Register_AGC_NS_FENOTCHFILT10(self.zz_rmio, self.zz_label)
        self.zz_rdict['FENOTCHFILT10'] = self.FENOTCHFILT10
        self.FENOTCHFILT11 = RM_Register_AGC_NS_FENOTCHFILT11(self.zz_rmio, self.zz_label)
        self.zz_rdict['FENOTCHFILT11'] = self.FENOTCHFILT11
        self.CCADEBUG = RM_Register_AGC_NS_CCADEBUG(self.zz_rmio, self.zz_label)
        self.zz_rdict['CCADEBUG'] = self.CCADEBUG
        self.__dict__['zz_frozen'] = True