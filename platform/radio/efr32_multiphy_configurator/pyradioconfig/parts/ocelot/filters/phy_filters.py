"""
Lynx specific filters
"""
from pyradioconfig.calculator_model_framework.interfaces.iphy_filter import IPhyFilter


class PhyFilters(IPhyFilter):

    #Studio black list (files removed before Studio distribution)
    customer_phy_groups = [
                            'Phys_secret1',
                            'Phys_secret2',
                            'Phys_secret3',
                            'Phys_secret4',
                            'Phys_secret5',
                            'Phys_secret6',
                            'Phys_secret7',
                            'Phys_secret8',
                            'Phys_Internal_Base_Experimental',
                            'Phys_Internal_Base_Utility',
                            'Phys_Internal_Base_ValOnly',
                            'Phys_Internal_Connect',
                            'Phys_Internal_Longrange',
                            'Phys_Internal_RAIL_Base_Standard_BLE',
                            'Phys_RAIL_Base_Standard_IEEE802154',
                            'Phys_RAIL_Base_Standard_ZWave',
                            'Phys_Studio_IEEE802154_BPSK',
                        ]

    #Studio white list (these PHYs show in Studio as proprietary starting points)
    simplicity_studio_phy_groups = [
                                        'Phys_Studio_Base',
                                        'Phys_Studio_Base_Standard_SUNFSK',
                                        'Phys_Studio_Sidewalk',
                                        'Phys_Studio_Connect',
                                        'Phys_Studio_LongRange',
                                        'Phys_Studio_MBus',
                                        'Phys_Studio_Base_Sigfox',
                                        'phys_studio_wisun_fan_1_0',
                                        'phys_studio_wisun_fan_1_1',
                                        'phys_studio_wisun_fan_1_1_virtual',
                                        'phys_studio_wisun_han',
                                        'Phys_Studio_IEE802154',
                                        'Phys_Studio_Sigfox_TX',
                                        'Phys_Internal_Base_ValOnly_aliases'
                                    ]

    # Special designation for simulation PHYs
    sim_tests_phy_groups = []

    # Special designation for non-functional PHYs
    non_functional_phy_groups = []

    # PHYs to exclude from regression
    virtual_phy_groups = ['phys_studio_wisun_fan_1_1_virtual']