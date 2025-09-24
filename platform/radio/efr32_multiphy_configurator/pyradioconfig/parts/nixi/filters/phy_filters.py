"""
Nixi specific filters
"""
from pyradioconfig.calculator_model_framework.interfaces.iphy_filter import IPhyFilter


class PhyFilters(IPhyFilter):

    customer_phy_groups = [
                            'Phys_secret1',
                            'Phys_secret2',
                            'Phys_secret3',
                            'Phys_Bluetooth_LE',
                            'Phys_Deprecated',
                            'Phys_secret4',
                            'Phys_secret5',
                            'Phys_IEEE802154',
                            'Phys_secret6',
                            'Phys_secret7',
                            'Phys_secret8',
                            'Phys_Mbus_lab',
                            'Phys_RAIL',
                            'Phys_Utility',
                            'Phys_secret9',
                            'Phys_ZWave',
                            'Phys_sim_tests',
                            'Phys_Internal',
                            'Phys_Internal_calculator_regression',
                        ]

    sim_tests_phy_groups = ['Phys_sim_tests']

    simplicity_studio_phy_groups = ['Phys_Datasheet', 'Phys_Studio', 'Phys_connect', 'Phys_MBus_Studio', 'Phys_Studio_LongRange']
