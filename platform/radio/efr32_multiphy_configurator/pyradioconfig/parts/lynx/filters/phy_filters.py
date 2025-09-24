"""
Lynx specific filters
"""
from pyradioconfig.calculator_model_framework.interfaces.iphy_filter import IPhyFilter


class PhyFilters(IPhyFilter):

    customer_phy_groups = [
                            'Phys_secret1',
                            'Phys_sim_tests',
                            'Phys_Internal',
                            'Phys_secret2',
                            'Phys_secret3',
                            'Phys_Utility',
                        ]

    sim_tests_phy_groups = ['Phys_sim_tests']

    simplicity_studio_phy_groups = ['Phys_Datasheet', 'Phys_Connect', 'Phys_Studio_IEEE802154', 'Phys_Studio_BLE']
