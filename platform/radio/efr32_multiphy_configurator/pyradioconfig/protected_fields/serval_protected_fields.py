from pyradioconfig.protected_fields.base_protected_fields import ProtectedFieldsBase


class ProtectedFieldsServal(ProtectedFieldsBase):

    """
    PTE has two types of protected fields
    1) cal
    2) set and forget

    For radio configurator,
    1) Protect all PTE cal fields (do not write)
    2) Ensure set and forget fields are same as PTE if radio configurator is also programming the values

    For rail_script,
    1) protect all PTE cal fields
    2) Protect set and forget field, if register contains pte cal field or radio configurator writes to the field

    """

    PTE_PROTECTED_FIELD_DICT = {
    }

    # : List of fields found in PTE, but allow radio configurator to override the value
    PTE_PROTECTED_FIELD_EXCEPTIONS = [
    ]

    # : List of fields that are found in PTE list, but does not need to be protected by RAIL
    RAIL_PROTECTED_FIELD_EXCEPTIONS = [
    ]