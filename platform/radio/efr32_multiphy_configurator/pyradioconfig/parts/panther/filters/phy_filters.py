"""
Panther specific filters
"""
from pyradioconfig.calculator_model_framework.interfaces.iphy_filter import IPhyFilter


class PhyFilters(IPhyFilter):

    customer_phy_groups = [
                            'Phys_Bluetooth_LE',
                            'Phys_Internal_Base_Standard_IEEE802154',
                            'Phys_sim_tests',
                            'Phys_Utility',
                            'Phys_Internal',
                            'Phys_Internal_Base_Customer_Signify',
                            'Phys_Studio_IEEE802154'
                        ]

    sim_tests_phy_groups = ['Phys_sim_tests']

    simplicity_studio_phy_groups = ['Phys_Datasheet', 'Phys_Studio']
