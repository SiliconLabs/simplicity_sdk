
# -*- coding: utf-8 -*-

from . static import Base_RM_Field


class RM_Field_SEQ_SPARE0_SPARE0(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_SPARE0_SPARE0, self).__init__(register,
            'SPARE0', 'SEQ.SPARE0.SPARE0', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_SPARE1_SPARE1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_SPARE1_SPARE1, self).__init__(register,
            'SPARE1', 'SEQ.SPARE1.SPARE1', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_SPARE2_SPARE2(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_SPARE2_SPARE2, self).__init__(register,
            'SPARE2', 'SEQ.SPARE2.SPARE2', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_SPARE3_SPARE3(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_SPARE3_SPARE3, self).__init__(register,
            'SPARE3', 'SEQ.SPARE3.SPARE3', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_SPARE4_SPARE4(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_SPARE4_SPARE4, self).__init__(register,
            'SPARE4', 'SEQ.SPARE4.SPARE4', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_MODEMINFO_LEGACY_EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_MODEMINFO_LEGACY_EN, self).__init__(register,
            'LEGACY_EN', 'SEQ.MODEMINFO.LEGACY_EN', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_MODEMINFO_SPARE0(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_MODEMINFO_SPARE0, self).__init__(register,
            'SPARE0', 'SEQ.MODEMINFO.SPARE0', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_MODEMINFO_TRECS_EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_MODEMINFO_TRECS_EN, self).__init__(register,
            'TRECS_EN', 'SEQ.MODEMINFO.TRECS_EN', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_MODEMINFO_BCR_EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_MODEMINFO_BCR_EN, self).__init__(register,
            'BCR_EN', 'SEQ.MODEMINFO.BCR_EN', 'read-write',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_MODEMINFO_COHERENT_EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_MODEMINFO_COHERENT_EN, self).__init__(register,
            'COHERENT_EN', 'SEQ.MODEMINFO.COHERENT_EN', 'read-write',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_MODEMINFO_LONGRANGE_EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_MODEMINFO_LONGRANGE_EN, self).__init__(register,
            'LONGRANGE_EN', 'SEQ.MODEMINFO.LONGRANGE_EN', 'read-write',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_MODEMINFO_ENHDSSS_EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_MODEMINFO_ENHDSSS_EN, self).__init__(register,
            'ENHDSSS_EN', 'SEQ.MODEMINFO.ENHDSSS_EN', 'read-write',
            u"",
            6, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_MODEMINFO_SPARE1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_MODEMINFO_SPARE1, self).__init__(register,
            'SPARE1', 'SEQ.MODEMINFO.SPARE1', 'read-write',
            u"",
            7, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_MODEMINFO_SOFTMODEM_EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_MODEMINFO_SOFTMODEM_EN, self).__init__(register,
            'SOFTMODEM_EN', 'SEQ.MODEMINFO.SOFTMODEM_EN', 'read-write',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_MODEMINFO_BTC_EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_MODEMINFO_BTC_EN, self).__init__(register,
            'BTC_EN', 'SEQ.MODEMINFO.BTC_EN', 'read-write',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_MODEMINFO_SPARE2(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_MODEMINFO_SPARE2, self).__init__(register,
            'SPARE2', 'SEQ.MODEMINFO.SPARE2', 'read-write',
            u"",
            10, 22)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_MISC_RESERVED10(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_MISC_RESERVED10, self).__init__(register,
            'RESERVED10', 'SEQ.MISC.RESERVED10', 'read-write',
            u"",
            10, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_MISC_IFPKDOFF_AFTER_FRAME_EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_MISC_IFPKDOFF_AFTER_FRAME_EN, self).__init__(register,
            'IFPKDOFF_AFTER_FRAME_EN', 'SEQ.MISC.IFPKDOFF_AFTER_FRAME_EN', 'read-write',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_MISC_RFPKDOFF_AFTER_FRAME_EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_MISC_RFPKDOFF_AFTER_FRAME_EN, self).__init__(register,
            'RFPKDOFF_AFTER_FRAME_EN', 'SEQ.MISC.RFPKDOFF_AFTER_FRAME_EN', 'read-write',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_MISC_RESERVED2(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_MISC_RESERVED2, self).__init__(register,
            'RESERVED2', 'SEQ.MISC.RESERVED2', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_MISC_RESERVED1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_MISC_RESERVED1, self).__init__(register,
            'RESERVED1', 'SEQ.MISC.RESERVED1', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_MISC_RESERVED0(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_MISC_RESERVED0, self).__init__(register,
            'RESERVED0', 'SEQ.MISC.RESERVED0', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_PHYINFO_ADDRESS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_PHYINFO_ADDRESS, self).__init__(register,
            'ADDRESS', 'SEQ.PHYINFO.ADDRESS', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


