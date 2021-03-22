
# -*- coding: utf-8 -*-

__all__ = [ 'RM_Peripheral_RAC_NS' ]

from . static import Base_RM_Peripheral
from . RAC_NS_register import *

class RM_Peripheral_RAC_NS(Base_RM_Peripheral):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Peripheral_RAC_NS, self).__init__(rmio, label,
            0xB8020000, 'RAC_NS',
            u"",
            [])
        self.IPVERSION = RM_Register_RAC_NS_IPVERSION(self.zz_rmio, self.zz_label)
        self.zz_rdict['IPVERSION'] = self.IPVERSION
        self.EN = RM_Register_RAC_NS_EN(self.zz_rmio, self.zz_label)
        self.zz_rdict['EN'] = self.EN
        self.RXENSRCEN = RM_Register_RAC_NS_RXENSRCEN(self.zz_rmio, self.zz_label)
        self.zz_rdict['RXENSRCEN'] = self.RXENSRCEN
        self.STATUS = RM_Register_RAC_NS_STATUS(self.zz_rmio, self.zz_label)
        self.zz_rdict['STATUS'] = self.STATUS
        self.CMD = RM_Register_RAC_NS_CMD(self.zz_rmio, self.zz_label)
        self.zz_rdict['CMD'] = self.CMD
        self.CTRL = RM_Register_RAC_NS_CTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL'] = self.CTRL
        self.FORCESTATE = RM_Register_RAC_NS_FORCESTATE(self.zz_rmio, self.zz_label)
        self.zz_rdict['FORCESTATE'] = self.FORCESTATE
        self.IF = RM_Register_RAC_NS_IF(self.zz_rmio, self.zz_label)
        self.zz_rdict['IF'] = self.IF
        self.IEN = RM_Register_RAC_NS_IEN(self.zz_rmio, self.zz_label)
        self.zz_rdict['IEN'] = self.IEN
        self.TESTCTRL = RM_Register_RAC_NS_TESTCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['TESTCTRL'] = self.TESTCTRL
        self.SEQIF = RM_Register_RAC_NS_SEQIF(self.zz_rmio, self.zz_label)
        self.zz_rdict['SEQIF'] = self.SEQIF
        self.SEQIEN = RM_Register_RAC_NS_SEQIEN(self.zz_rmio, self.zz_label)
        self.zz_rdict['SEQIEN'] = self.SEQIEN
        self.STATUS1 = RM_Register_RAC_NS_STATUS1(self.zz_rmio, self.zz_label)
        self.zz_rdict['STATUS1'] = self.STATUS1
        self.STIMER = RM_Register_RAC_NS_STIMER(self.zz_rmio, self.zz_label)
        self.zz_rdict['STIMER'] = self.STIMER
        self.STIMERCOMP = RM_Register_RAC_NS_STIMERCOMP(self.zz_rmio, self.zz_label)
        self.zz_rdict['STIMERCOMP'] = self.STIMERCOMP
        self.SEQCTRL = RM_Register_RAC_NS_SEQCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['SEQCTRL'] = self.SEQCTRL
        self.PRESC = RM_Register_RAC_NS_PRESC(self.zz_rmio, self.zz_label)
        self.zz_rdict['PRESC'] = self.PRESC
        self.SR0 = RM_Register_RAC_NS_SR0(self.zz_rmio, self.zz_label)
        self.zz_rdict['SR0'] = self.SR0
        self.SR1 = RM_Register_RAC_NS_SR1(self.zz_rmio, self.zz_label)
        self.zz_rdict['SR1'] = self.SR1
        self.SR2 = RM_Register_RAC_NS_SR2(self.zz_rmio, self.zz_label)
        self.zz_rdict['SR2'] = self.SR2
        self.SR3 = RM_Register_RAC_NS_SR3(self.zz_rmio, self.zz_label)
        self.zz_rdict['SR3'] = self.SR3
        self.STCTRL = RM_Register_RAC_NS_STCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['STCTRL'] = self.STCTRL
        self.FRCTXWORD = RM_Register_RAC_NS_FRCTXWORD(self.zz_rmio, self.zz_label)
        self.zz_rdict['FRCTXWORD'] = self.FRCTXWORD
        self.FRCRXWORD = RM_Register_RAC_NS_FRCRXWORD(self.zz_rmio, self.zz_label)
        self.zz_rdict['FRCRXWORD'] = self.FRCRXWORD
        self.EM1PCSR = RM_Register_RAC_NS_EM1PCSR(self.zz_rmio, self.zz_label)
        self.zz_rdict['EM1PCSR'] = self.EM1PCSR
        self.SYNTHENCTRL = RM_Register_RAC_NS_SYNTHENCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['SYNTHENCTRL'] = self.SYNTHENCTRL
        self.SYNTHREGCTRL = RM_Register_RAC_NS_SYNTHREGCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['SYNTHREGCTRL'] = self.SYNTHREGCTRL
        self.VCOCTRL = RM_Register_RAC_NS_VCOCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['VCOCTRL'] = self.VCOCTRL
        self.CHPCTRL = RM_Register_RAC_NS_CHPCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHPCTRL'] = self.CHPCTRL
        self.RFSTATUS = RM_Register_RAC_NS_RFSTATUS(self.zz_rmio, self.zz_label)
        self.zz_rdict['RFSTATUS'] = self.RFSTATUS
        self.STATUS2 = RM_Register_RAC_NS_STATUS2(self.zz_rmio, self.zz_label)
        self.zz_rdict['STATUS2'] = self.STATUS2
        self.IFPGACTRL = RM_Register_RAC_NS_IFPGACTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFPGACTRL'] = self.IFPGACTRL
        self.PAENCTRL = RM_Register_RAC_NS_PAENCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['PAENCTRL'] = self.PAENCTRL
        self.APC = RM_Register_RAC_NS_APC(self.zz_rmio, self.zz_label)
        self.zz_rdict['APC'] = self.APC
        self.ANTDIV = RM_Register_RAC_NS_ANTDIV(self.zz_rmio, self.zz_label)
        self.zz_rdict['ANTDIV'] = self.ANTDIV
        self.AUXADCTRIM = RM_Register_RAC_NS_AUXADCTRIM(self.zz_rmio, self.zz_label)
        self.zz_rdict['AUXADCTRIM'] = self.AUXADCTRIM
        self.AUXADCEN = RM_Register_RAC_NS_AUXADCEN(self.zz_rmio, self.zz_label)
        self.zz_rdict['AUXADCEN'] = self.AUXADCEN
        self.AUXADCCTRL0 = RM_Register_RAC_NS_AUXADCCTRL0(self.zz_rmio, self.zz_label)
        self.zz_rdict['AUXADCCTRL0'] = self.AUXADCCTRL0
        self.AUXADCCTRL1 = RM_Register_RAC_NS_AUXADCCTRL1(self.zz_rmio, self.zz_label)
        self.zz_rdict['AUXADCCTRL1'] = self.AUXADCCTRL1
        self.AUXADCOUT = RM_Register_RAC_NS_AUXADCOUT(self.zz_rmio, self.zz_label)
        self.zz_rdict['AUXADCOUT'] = self.AUXADCOUT
        self.CLKMULTEN0 = RM_Register_RAC_NS_CLKMULTEN0(self.zz_rmio, self.zz_label)
        self.zz_rdict['CLKMULTEN0'] = self.CLKMULTEN0
        self.CLKMULTEN1 = RM_Register_RAC_NS_CLKMULTEN1(self.zz_rmio, self.zz_label)
        self.zz_rdict['CLKMULTEN1'] = self.CLKMULTEN1
        self.CLKMULTCTRL = RM_Register_RAC_NS_CLKMULTCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['CLKMULTCTRL'] = self.CLKMULTCTRL
        self.CLKMULTSTATUS = RM_Register_RAC_NS_CLKMULTSTATUS(self.zz_rmio, self.zz_label)
        self.zz_rdict['CLKMULTSTATUS'] = self.CLKMULTSTATUS
        self.IFADCDBG = RM_Register_RAC_NS_IFADCDBG(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFADCDBG'] = self.IFADCDBG
        self.IFADCTRIM0 = RM_Register_RAC_NS_IFADCTRIM0(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFADCTRIM0'] = self.IFADCTRIM0
        self.IFADCTRIM1 = RM_Register_RAC_NS_IFADCTRIM1(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFADCTRIM1'] = self.IFADCTRIM1
        self.IFADCCAL = RM_Register_RAC_NS_IFADCCAL(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFADCCAL'] = self.IFADCCAL
        self.IFADCSTATUS = RM_Register_RAC_NS_IFADCSTATUS(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFADCSTATUS'] = self.IFADCSTATUS
        self.LNAMIXDEBUG = RM_Register_RAC_NS_LNAMIXDEBUG(self.zz_rmio, self.zz_label)
        self.zz_rdict['LNAMIXDEBUG'] = self.LNAMIXDEBUG
        self.LNAMIXTRIM0 = RM_Register_RAC_NS_LNAMIXTRIM0(self.zz_rmio, self.zz_label)
        self.zz_rdict['LNAMIXTRIM0'] = self.LNAMIXTRIM0
        self.LNAMIXTRIM1 = RM_Register_RAC_NS_LNAMIXTRIM1(self.zz_rmio, self.zz_label)
        self.zz_rdict['LNAMIXTRIM1'] = self.LNAMIXTRIM1
        self.LNAMIXTRIM2 = RM_Register_RAC_NS_LNAMIXTRIM2(self.zz_rmio, self.zz_label)
        self.zz_rdict['LNAMIXTRIM2'] = self.LNAMIXTRIM2
        self.LNAMIXTRIM3 = RM_Register_RAC_NS_LNAMIXTRIM3(self.zz_rmio, self.zz_label)
        self.zz_rdict['LNAMIXTRIM3'] = self.LNAMIXTRIM3
        self.LNAMIXTRIM4 = RM_Register_RAC_NS_LNAMIXTRIM4(self.zz_rmio, self.zz_label)
        self.zz_rdict['LNAMIXTRIM4'] = self.LNAMIXTRIM4
        self.LNAMIXCAL = RM_Register_RAC_NS_LNAMIXCAL(self.zz_rmio, self.zz_label)
        self.zz_rdict['LNAMIXCAL'] = self.LNAMIXCAL
        self.LNAMIXEN = RM_Register_RAC_NS_LNAMIXEN(self.zz_rmio, self.zz_label)
        self.zz_rdict['LNAMIXEN'] = self.LNAMIXEN
        self.PRECTRL = RM_Register_RAC_NS_PRECTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['PRECTRL'] = self.PRECTRL
        self.PATRIM0 = RM_Register_RAC_NS_PATRIM0(self.zz_rmio, self.zz_label)
        self.zz_rdict['PATRIM0'] = self.PATRIM0
        self.PATRIM1 = RM_Register_RAC_NS_PATRIM1(self.zz_rmio, self.zz_label)
        self.zz_rdict['PATRIM1'] = self.PATRIM1
        self.PATRIM2 = RM_Register_RAC_NS_PATRIM2(self.zz_rmio, self.zz_label)
        self.zz_rdict['PATRIM2'] = self.PATRIM2
        self.PATRIM3 = RM_Register_RAC_NS_PATRIM3(self.zz_rmio, self.zz_label)
        self.zz_rdict['PATRIM3'] = self.PATRIM3
        self.PATRIM4 = RM_Register_RAC_NS_PATRIM4(self.zz_rmio, self.zz_label)
        self.zz_rdict['PATRIM4'] = self.PATRIM4
        self.PATRIM5 = RM_Register_RAC_NS_PATRIM5(self.zz_rmio, self.zz_label)
        self.zz_rdict['PATRIM5'] = self.PATRIM5
        self.PADEBUG0 = RM_Register_RAC_NS_PADEBUG0(self.zz_rmio, self.zz_label)
        self.zz_rdict['PADEBUG0'] = self.PADEBUG0
        self.TXPOWER = RM_Register_RAC_NS_TXPOWER(self.zz_rmio, self.zz_label)
        self.zz_rdict['TXPOWER'] = self.TXPOWER
        self.TXRAMP = RM_Register_RAC_NS_TXRAMP(self.zz_rmio, self.zz_label)
        self.zz_rdict['TXRAMP'] = self.TXRAMP
        self.PGADEBUG = RM_Register_RAC_NS_PGADEBUG(self.zz_rmio, self.zz_label)
        self.zz_rdict['PGADEBUG'] = self.PGADEBUG
        self.PGATRIM = RM_Register_RAC_NS_PGATRIM(self.zz_rmio, self.zz_label)
        self.zz_rdict['PGATRIM'] = self.PGATRIM
        self.PGACAL = RM_Register_RAC_NS_PGACAL(self.zz_rmio, self.zz_label)
        self.zz_rdict['PGACAL'] = self.PGACAL
        self.PGACTRL = RM_Register_RAC_NS_PGACTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['PGACTRL'] = self.PGACTRL
        self.RFBIASCAL = RM_Register_RAC_NS_RFBIASCAL(self.zz_rmio, self.zz_label)
        self.zz_rdict['RFBIASCAL'] = self.RFBIASCAL
        self.RFBIASCTRL = RM_Register_RAC_NS_RFBIASCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['RFBIASCTRL'] = self.RFBIASCTRL
        self.RADIOEN = RM_Register_RAC_NS_RADIOEN(self.zz_rmio, self.zz_label)
        self.zz_rdict['RADIOEN'] = self.RADIOEN
        self.RFPATHEN0 = RM_Register_RAC_NS_RFPATHEN0(self.zz_rmio, self.zz_label)
        self.zz_rdict['RFPATHEN0'] = self.RFPATHEN0
        self.RFPATHEN1 = RM_Register_RAC_NS_RFPATHEN1(self.zz_rmio, self.zz_label)
        self.zz_rdict['RFPATHEN1'] = self.RFPATHEN1
        self.RX = RM_Register_RAC_NS_RX(self.zz_rmio, self.zz_label)
        self.zz_rdict['RX'] = self.RX
        self.TX = RM_Register_RAC_NS_TX(self.zz_rmio, self.zz_label)
        self.zz_rdict['TX'] = self.TX
        self.SYDEBUG = RM_Register_RAC_NS_SYDEBUG(self.zz_rmio, self.zz_label)
        self.zz_rdict['SYDEBUG'] = self.SYDEBUG
        self.SYTRIM0 = RM_Register_RAC_NS_SYTRIM0(self.zz_rmio, self.zz_label)
        self.zz_rdict['SYTRIM0'] = self.SYTRIM0
        self.SYTRIM1 = RM_Register_RAC_NS_SYTRIM1(self.zz_rmio, self.zz_label)
        self.zz_rdict['SYTRIM1'] = self.SYTRIM1
        self.SYCAL = RM_Register_RAC_NS_SYCAL(self.zz_rmio, self.zz_label)
        self.zz_rdict['SYCAL'] = self.SYCAL
        self.SYEN = RM_Register_RAC_NS_SYEN(self.zz_rmio, self.zz_label)
        self.zz_rdict['SYEN'] = self.SYEN
        self.SYLOEN = RM_Register_RAC_NS_SYLOEN(self.zz_rmio, self.zz_label)
        self.zz_rdict['SYLOEN'] = self.SYLOEN
        self.SYMMDCTRL = RM_Register_RAC_NS_SYMMDCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['SYMMDCTRL'] = self.SYMMDCTRL
        self.SYDIVCTRL = RM_Register_RAC_NS_SYDIVCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['SYDIVCTRL'] = self.SYDIVCTRL
        self.DIGCLKRETIMECTRL = RM_Register_RAC_NS_DIGCLKRETIMECTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['DIGCLKRETIMECTRL'] = self.DIGCLKRETIMECTRL
        self.DIGCLKRETIMESTATUS = RM_Register_RAC_NS_DIGCLKRETIMESTATUS(self.zz_rmio, self.zz_label)
        self.zz_rdict['DIGCLKRETIMESTATUS'] = self.DIGCLKRETIMESTATUS
        self.XORETIMECTRL = RM_Register_RAC_NS_XORETIMECTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['XORETIMECTRL'] = self.XORETIMECTRL
        self.XORETIMESTATUS = RM_Register_RAC_NS_XORETIMESTATUS(self.zz_rmio, self.zz_label)
        self.zz_rdict['XORETIMESTATUS'] = self.XORETIMESTATUS
        self.AGCOVERWRITE0 = RM_Register_RAC_NS_AGCOVERWRITE0(self.zz_rmio, self.zz_label)
        self.zz_rdict['AGCOVERWRITE0'] = self.AGCOVERWRITE0
        self.AGCOVERWRITE1 = RM_Register_RAC_NS_AGCOVERWRITE1(self.zz_rmio, self.zz_label)
        self.zz_rdict['AGCOVERWRITE1'] = self.AGCOVERWRITE1
        self.IFADCPLLCTRL = RM_Register_RAC_NS_IFADCPLLCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFADCPLLCTRL'] = self.IFADCPLLCTRL
        self.IFADCPLLDCO = RM_Register_RAC_NS_IFADCPLLDCO(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFADCPLLDCO'] = self.IFADCPLLDCO
        self.IFADCPLLEN = RM_Register_RAC_NS_IFADCPLLEN(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFADCPLLEN'] = self.IFADCPLLEN
        self.IFADCPLLREG = RM_Register_RAC_NS_IFADCPLLREG(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFADCPLLREG'] = self.IFADCPLLREG
        self.IFADCPLLOUTSTATUS = RM_Register_RAC_NS_IFADCPLLOUTSTATUS(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFADCPLLOUTSTATUS'] = self.IFADCPLLOUTSTATUS
        self.TXOGNDPKD = RM_Register_RAC_NS_TXOGNDPKD(self.zz_rmio, self.zz_label)
        self.zz_rdict['TXOGNDPKD'] = self.TXOGNDPKD
        self.TXOGNDPKDSTATUS = RM_Register_RAC_NS_TXOGNDPKDSTATUS(self.zz_rmio, self.zz_label)
        self.zz_rdict['TXOGNDPKDSTATUS'] = self.TXOGNDPKDSTATUS
        self.SPARE = RM_Register_RAC_NS_SPARE(self.zz_rmio, self.zz_label)
        self.zz_rdict['SPARE'] = self.SPARE
        self.SCRATCH0 = RM_Register_RAC_NS_SCRATCH0(self.zz_rmio, self.zz_label)
        self.zz_rdict['SCRATCH0'] = self.SCRATCH0
        self.SCRATCH1 = RM_Register_RAC_NS_SCRATCH1(self.zz_rmio, self.zz_label)
        self.zz_rdict['SCRATCH1'] = self.SCRATCH1
        self.SCRATCH2 = RM_Register_RAC_NS_SCRATCH2(self.zz_rmio, self.zz_label)
        self.zz_rdict['SCRATCH2'] = self.SCRATCH2
        self.SCRATCH3 = RM_Register_RAC_NS_SCRATCH3(self.zz_rmio, self.zz_label)
        self.zz_rdict['SCRATCH3'] = self.SCRATCH3
        self.SCRATCH4 = RM_Register_RAC_NS_SCRATCH4(self.zz_rmio, self.zz_label)
        self.zz_rdict['SCRATCH4'] = self.SCRATCH4
        self.SCRATCH5 = RM_Register_RAC_NS_SCRATCH5(self.zz_rmio, self.zz_label)
        self.zz_rdict['SCRATCH5'] = self.SCRATCH5
        self.SCRATCH6 = RM_Register_RAC_NS_SCRATCH6(self.zz_rmio, self.zz_label)
        self.zz_rdict['SCRATCH6'] = self.SCRATCH6
        self.SCRATCH7 = RM_Register_RAC_NS_SCRATCH7(self.zz_rmio, self.zz_label)
        self.zz_rdict['SCRATCH7'] = self.SCRATCH7
        self.THMSW = RM_Register_RAC_NS_THMSW(self.zz_rmio, self.zz_label)
        self.zz_rdict['THMSW'] = self.THMSW
        self.DIAGAALTEN = RM_Register_RAC_NS_DIAGAALTEN(self.zz_rmio, self.zz_label)
        self.zz_rdict['DIAGAALTEN'] = self.DIAGAALTEN
        self.DIAGAALTSEL = RM_Register_RAC_NS_DIAGAALTSEL(self.zz_rmio, self.zz_label)
        self.zz_rdict['DIAGAALTSEL'] = self.DIAGAALTSEL
        self.DIAGAALTBRIDGECTRL = RM_Register_RAC_NS_DIAGAALTBRIDGECTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['DIAGAALTBRIDGECTRL'] = self.DIAGAALTBRIDGECTRL
        self.RFLOCK0 = RM_Register_RAC_NS_RFLOCK0(self.zz_rmio, self.zz_label)
        self.zz_rdict['RFLOCK0'] = self.RFLOCK0
        self.RFLOCK1 = RM_Register_RAC_NS_RFLOCK1(self.zz_rmio, self.zz_label)
        self.zz_rdict['RFLOCK1'] = self.RFLOCK1
        self.__dict__['zz_frozen'] = True