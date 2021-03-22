
# -*- coding: utf-8 -*-

from . static import Base_RM_Field


class RM_Field_TXFRONT_NS_IPVERSION_IPVERSION(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_IPVERSION_IPVERSION, self).__init__(register,
            'IPVERSION', 'TXFRONT_NS.IPVERSION.IPVERSION', 'read-only',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_EN_EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_EN_EN, self).__init__(register,
            'EN', 'TXFRONT_NS.EN.EN', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_EN_DISABLING(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_EN_DISABLING, self).__init__(register,
            'DISABLING', 'TXFRONT_NS.EN.DISABLING', 'read-only',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_SWRST_SWRST(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_SWRST_SWRST, self).__init__(register,
            'SWRST', 'TXFRONT_NS.SWRST.SWRST', 'write-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_SWRST_RESETTING(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_SWRST_RESETTING, self).__init__(register,
            'RESETTING', 'TXFRONT_NS.SWRST.RESETTING', 'read-only',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_DISCLK_DISCLK(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_DISCLK_DISCLK, self).__init__(register,
            'DISCLK', 'TXFRONT_NS.DISCLK.DISCLK', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_INT1CFG_RATIO(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_INT1CFG_RATIO, self).__init__(register,
            'RATIO', 'TXFRONT_NS.INT1CFG.RATIO', 'read-write',
            u"",
            0, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_INT1CFG_GAINSHIFT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_INT1CFG_GAINSHIFT, self).__init__(register,
            'GAINSHIFT', 'TXFRONT_NS.INT1CFG.GAINSHIFT', 'read-write',
            u"",
            4, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_INT1COEF01_COEFF0(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_INT1COEF01_COEFF0, self).__init__(register,
            'COEFF0', 'TXFRONT_NS.INT1COEF01.COEFF0', 'read-write',
            u"",
            0, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_INT1COEF01_COEFF1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_INT1COEF01_COEFF1, self).__init__(register,
            'COEFF1', 'TXFRONT_NS.INT1COEF01.COEFF1', 'read-write',
            u"",
            16, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_INT1COEF23_COEFF2(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_INT1COEF23_COEFF2, self).__init__(register,
            'COEFF2', 'TXFRONT_NS.INT1COEF23.COEFF2', 'read-write',
            u"",
            0, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_INT1COEF23_COEFF3(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_INT1COEF23_COEFF3, self).__init__(register,
            'COEFF3', 'TXFRONT_NS.INT1COEF23.COEFF3', 'read-write',
            u"",
            16, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_INT1COEF45_COEFF4(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_INT1COEF45_COEFF4, self).__init__(register,
            'COEFF4', 'TXFRONT_NS.INT1COEF45.COEFF4', 'read-write',
            u"",
            0, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_INT1COEF45_COEFF5(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_INT1COEF45_COEFF5, self).__init__(register,
            'COEFF5', 'TXFRONT_NS.INT1COEF45.COEFF5', 'read-write',
            u"",
            16, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_INT1COEF67_COEFF6(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_INT1COEF67_COEFF6, self).__init__(register,
            'COEFF6', 'TXFRONT_NS.INT1COEF67.COEFF6', 'read-write',
            u"",
            0, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_INT1COEF67_COEFF7(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_INT1COEF67_COEFF7, self).__init__(register,
            'COEFF7', 'TXFRONT_NS.INT1COEF67.COEFF7', 'read-write',
            u"",
            16, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_INT1COEF89_COEFF8(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_INT1COEF89_COEFF8, self).__init__(register,
            'COEFF8', 'TXFRONT_NS.INT1COEF89.COEFF8', 'read-write',
            u"",
            0, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_INT1COEF89_COEFF9(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_INT1COEF89_COEFF9, self).__init__(register,
            'COEFF9', 'TXFRONT_NS.INT1COEF89.COEFF9', 'read-write',
            u"",
            16, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_INT1COEF1011_COEFF10(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_INT1COEF1011_COEFF10, self).__init__(register,
            'COEFF10', 'TXFRONT_NS.INT1COEF1011.COEFF10', 'read-write',
            u"",
            0, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_INT1COEF1011_COEFF11(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_INT1COEF1011_COEFF11, self).__init__(register,
            'COEFF11', 'TXFRONT_NS.INT1COEF1011.COEFF11', 'read-write',
            u"",
            16, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_INT1COEF1213_COEFF12(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_INT1COEF1213_COEFF12, self).__init__(register,
            'COEFF12', 'TXFRONT_NS.INT1COEF1213.COEFF12', 'read-write',
            u"",
            0, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_INT1COEF1213_COEFF13(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_INT1COEF1213_COEFF13, self).__init__(register,
            'COEFF13', 'TXFRONT_NS.INT1COEF1213.COEFF13', 'read-write',
            u"",
            16, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_INT1COEF1415_COEFF14(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_INT1COEF1415_COEFF14, self).__init__(register,
            'COEFF14', 'TXFRONT_NS.INT1COEF1415.COEFF14', 'read-write',
            u"",
            0, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_INT1COEF1415_COEFF15(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_INT1COEF1415_COEFF15, self).__init__(register,
            'COEFF15', 'TXFRONT_NS.INT1COEF1415.COEFF15', 'read-write',
            u"",
            16, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_INT2CFG_RATIO(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_INT2CFG_RATIO, self).__init__(register,
            'RATIO', 'TXFRONT_NS.INT2CFG.RATIO', 'read-write',
            u"",
            0, 10)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_INT2CFG_GAINSHIFT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_INT2CFG_GAINSHIFT, self).__init__(register,
            'GAINSHIFT', 'TXFRONT_NS.INT2CFG.GAINSHIFT', 'read-write',
            u"",
            10, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_SRCCFG_RATIO(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_SRCCFG_RATIO, self).__init__(register,
            'RATIO', 'TXFRONT_NS.SRCCFG.RATIO', 'read-write',
            u"",
            0, 19)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_TXIQIMB_EPSILON(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_TXIQIMB_EPSILON, self).__init__(register,
            'EPSILON', 'TXFRONT_NS.TXIQIMB.EPSILON', 'read-write',
            u"",
            0, 12)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_TXIQIMB_PHI(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_TXIQIMB_PHI, self).__init__(register,
            'PHI', 'TXFRONT_NS.TXIQIMB.PHI', 'read-write',
            u"",
            16, 12)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_TXDCOFFSET_DCOFFSETI(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_TXDCOFFSET_DCOFFSETI, self).__init__(register,
            'DCOFFSETI', 'TXFRONT_NS.TXDCOFFSET.DCOFFSETI', 'read-write',
            u"",
            0, 10)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_TXDCOFFSET_DCOFFSETQ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_TXDCOFFSET_DCOFFSETQ, self).__init__(register,
            'DCOFFSETQ', 'TXFRONT_NS.TXDCOFFSET.DCOFFSETQ', 'read-write',
            u"",
            16, 10)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_TXRAMPUP_MODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_TXRAMPUP_MODE, self).__init__(register,
            'MODE', 'TXFRONT_NS.TXRAMPUP.MODE', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_TXRAMPUP_DELAY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_TXRAMPUP_DELAY, self).__init__(register,
            'DELAY', 'TXFRONT_NS.TXRAMPUP.DELAY', 'read-write',
            u"",
            1, 10)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_TXRAMPUP_STEP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_TXRAMPUP_STEP, self).__init__(register,
            'STEP', 'TXFRONT_NS.TXRAMPUP.STEP', 'read-write',
            u"",
            11, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_TXDCRAMPUP_DELAY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_TXDCRAMPUP_DELAY, self).__init__(register,
            'DELAY', 'TXFRONT_NS.TXDCRAMPUP.DELAY', 'read-write',
            u"",
            0, 10)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_TXDCRAMPUP_DCRI(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_TXDCRAMPUP_DCRI, self).__init__(register,
            'DCRI', 'TXFRONT_NS.TXDCRAMPUP.DCRI', 'read-write',
            u"",
            10, 11)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_TXDCRAMPUP_DCRQ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_TXDCRAMPUP_DCRQ, self).__init__(register,
            'DCRQ', 'TXFRONT_NS.TXDCRAMPUP.DCRQ', 'read-write',
            u"",
            21, 11)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_TXGAIN_GAINDIG(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_TXGAIN_GAINDIG, self).__init__(register,
            'GAINDIG', 'TXFRONT_NS.TXGAIN.GAINDIG', 'read-write',
            u"",
            0, 10)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_TXCLIP_CLIPPOS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_TXCLIP_CLIPPOS, self).__init__(register,
            'CLIPPOS', 'TXFRONT_NS.TXCLIP.CLIPPOS', 'read-write',
            u"",
            0, 12)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_TXCLIP_CLIPNEG(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_TXCLIP_CLIPNEG, self).__init__(register,
            'CLIPNEG', 'TXFRONT_NS.TXCLIP.CLIPNEG', 'read-write',
            u"",
            16, 12)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_TXPREDIST_BETAAMP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_TXPREDIST_BETAAMP, self).__init__(register,
            'BETAAMP', 'TXFRONT_NS.TXPREDIST.BETAAMP', 'read-write',
            u"",
            0, 14)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_TXPREDIST_BETAPHA(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_TXPREDIST_BETAPHA, self).__init__(register,
            'BETAPHA', 'TXFRONT_NS.TXPREDIST.BETAPHA', 'read-write',
            u"",
            16, 14)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_DAC_CONNECTTEST(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_DAC_CONNECTTEST, self).__init__(register,
            'CONNECTTEST', 'TXFRONT_NS.DAC.CONNECTTEST', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_TXFRONT_NS_DAC_DACFORMAT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_TXFRONT_NS_DAC_DACFORMAT, self).__init__(register,
            'DACFORMAT', 'TXFRONT_NS.DAC.DACFORMAT', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


